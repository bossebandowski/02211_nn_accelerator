package io

import Chisel._

import patmos.Constants._

import ocp._
import util._

object Accelerator extends DeviceObject {

  def init(params: Map[String, String]) = {}

  def create(params: Map[String, String]): Accelerator = Module(new Accelerator())
}

class Accelerator() extends CoreDevice() {
  val readAddrP = Reg(init = UInt(0, 18))
  val readAddrW = Reg(init = UInt(0, 18))
  val readAddrB = Reg(init = UInt(0, 18))
  val writeAddr = Reg(init = UInt(0, 18))
  val masterReg = Reg(next = io.ocp.M)

  val pReg = Reg(init = SInt(0, 32))
  val wReg = Reg(init = SInt(0, 32))
  val bReg = Reg(init = SInt(0, 32))

  val layer1 = Reg(Vec(100, SInt(32.W)))
  val layer2 = Reg(Vec(10, SInt(32.W)))

  val nodeIdx = Reg(init = UInt(0, 8))

  // memory
  val memory = Module(new MemorySInt(18, 32))


  // address constants
  val imgAddrZero = UInt(0)
  val weightAddrZero = UInt(784)
  val biasAddrZero = UInt(80184)
  val lastAddr = UInt(80293)

  // Default OCP response
  io.ocp.S.Resp := OcpResp.NULL
  io.ocp.S.Data := UInt(15, width = 32)

  //Memort test
  val memoryAddress = Reg(init = UInt(783,18))

  // network constants

  // states
  val noNN :: loadNN :: idle :: infLoad :: loadpx :: loadw0 :: incNode0  :: loadb0 :: addb0 :: infInit1 :: loadw1 :: incNode1 :: initb1 :: loadb1 :: addb1 :: writeRes :: clear :: Nil = Enum(UInt(), 17)
  //  0       1         2       3          4         5         6            7         8         9        10          11        12          13        14        15       16          17                                 
  val stateReg = Reg(init = noNN)

  // set memory address. Only for debugging
  when (masterReg.Cmd === OcpCmd.WR && masterReg.Addr(4,0) === 0x10.U) {
    io.ocp.S.Resp := OcpResp.DVA
    memoryAddress := masterReg.Data
  }
  .otherwise {
    // state machine goes here

    when (stateReg === noNN && masterReg.Cmd === OcpCmd.WR) {
      /*
      initial state. No network is loaded, so inference is not possible.
      Waiting for a write command and data == 0 to transition to load network state
      */
      io.ocp.S.Resp := OcpResp.DVA
      when (masterReg.Data === UInt(0)) {
        stateReg := loadNN
        writeAddr := weightAddrZero
      }
    }

    when (stateReg === loadNN && masterReg.Cmd === OcpCmd.WR) {
      /*
      load neural network state. whenever we get a write command, we store the current ocp data val
      in memory and increment the memory address
      */
      io.ocp.S.Resp := OcpResp.DVA
      memory.io.wrEna := true.B
      memory.io.wrAddr := writeAddr
      memory.io.wrData := (masterReg.Data).asSInt
      when (writeAddr === lastAddr) {
        stateReg := idle
      }
      writeAddr := writeAddr + UInt(1)
    }

    when (stateReg === idle && masterReg.Cmd === OcpCmd.WR) {
      /*
      idle state. The accelerator has a network loaded in its memory and it is waiting for
      commands from the patmos. You can either load a new network by sending "0" or start
      the inference process by sending "1"
      */
      io.ocp.S.Resp := OcpResp.DVA
      memory.io.wrEna := false.B
      when(masterReg.Data(0) === UInt(0)) {
        stateReg := loadNN
        writeAddr := weightAddrZero
      }
      .otherwise {
        stateReg := infLoad
        writeAddr := imgAddrZero
      }
    }

    when (stateReg === infLoad && masterReg.Cmd === OcpCmd.WR) {
      /*
      first inference state. In order to run inference, you must first load an image into
      the accelerator memory. Provide sequential pixel values on the ocp data line and they
      will be stored in memory. The address increments automatically.
      */
      io.ocp.S.Resp := OcpResp.DVA
      memory.io.wrEna := true.B
      memory.io.wrAddr := writeAddr
      memory.io.wrData := (masterReg.Data).asSInt
      when (writeAddr === weightAddrZero - UInt(1)) {
        readAddrP := imgAddrZero
        readAddrW := weightAddrZero
        readAddrB := biasAddrZero
        memory.io.rdAddr := imgAddrZero
        stateReg := loadpx
      }
      writeAddr := writeAddr + UInt(1)
    }

    when (stateReg === loadpx) {
      /*
      the image in the memory is analysed pixel by pixel. For every pixel, we load the weights
      to all neurons in the hidden layer, multiply them with the pixel value, and increment the
      value of the respective neuron. Once all pixels have been analysed, we transition to the
      second layer (output layer). No responses to write commands are issued while the accelerator
      is busy with inference.
      */
      when (readAddrP === weightAddrZero) {
        memory.io.rdAddr := readAddrB         // set up bias read
        nodeIdx := UInt(0)                    // reset node idx
        readAddrB := readAddrB + UInt(1)      // increment bias addr
        readAddrW := readAddrW + UInt(1)      // also need to increment weight address
        stateReg := loadb0
      }
      .otherwise {
        memory.io.wrEna := false.B            // disable memory write to prevent data corruption
        pReg := memory.io.rdData              // read pixel value into corresponding register
        readAddrP := readAddrP + UInt(1)      // increment pixel address to be ready for next read
        nodeIdx := UInt(0)                    // for every pixel, we start at node 0 in the hidden layer
        memory.io.rdAddr := readAddrW         // we will read a weight in the next state. Set address in preparation
        readAddrW := readAddrW + UInt(1)      // increment weight addr
        stateReg := incNode0
      }
    }

    when (stateReg === incNode0) {
      /*
      incrementing node values. Transitions back to pixel load when all weights for a single pixel have
      been evaluated. Otherwise moves on to the next weight.
      */
      when (nodeIdx < UInt(100)) {
                                            // increment node value with product of pixel and weight (directly from memory to save clock cycles)
        layer1(nodeIdx) := layer1(nodeIdx) + pReg * memory.io.rdData
        memory.io.rdAddr := readAddrW       // update memory address for next cycle
        readAddrW := readAddrW + UInt(1)    // increment read address for next cycle
        nodeIdx := nodeIdx + UInt(1)        // increment node index for next cycle
      }
      .otherwise {
        memory.io.rdAddr := readAddrP       // prepare for pixel load. No need to increment weight address here because that happens in the pixel load state
        stateReg := loadpx
      }
    }

    when (stateReg === loadb0) {

    }
  }

  // handle reads. Only for returning results and debugging
  when(masterReg.Cmd === OcpCmd.RD){
    io.ocp.S.Resp := OcpResp.DVA
    //Read status
    switch(masterReg.Addr(4,0)) {
      is(0x4.U) { 
        io.ocp.S.Data := stateReg
      }
      //Read value from memory
      is(0xC.U) { 
        memory.io.rdAddr := memoryAddress
        io.ocp.S.Data := (memory.io.rdData).asUInt
      }
      //Read result
    }
  }
}