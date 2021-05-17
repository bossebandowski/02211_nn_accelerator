package io

import Chisel._
import chisel3.Driver

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
  val outputReg = Reg(init = UInt(10, 32))

  val layer1 = Reg(Vec(100, SInt(32.W)))
  val layer2 = Reg(Vec(10, SInt(32.W)))

  val nodeIdx = Reg(init = UInt(0, 8))
  val outputIdx = Reg(init = UInt(0, 8))

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
  val noNN :: loadNN :: idle :: infLoad :: loadpx :: loadw0 :: incNode0  :: addb0 :: incNode1 :: addb1 :: writeRes :: clear :: Nil = Enum(UInt(), 12)
  //  0       1         2       3          4         5         6            7        8           9        10          11       12                                 
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
        stateReg := addb0
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
        memory.io.rdAddr := readAddrW         // update memory address for next cycle
        readAddrW := readAddrW + UInt(1)      // increment read address for next cycle
        nodeIdx := nodeIdx + UInt(1)          // increment node index for next cycle
      }
      .otherwise {
        memory.io.rdAddr := readAddrP         // prepare for pixel load. No need to increment weight address here because that happens in the pixel load state
        stateReg := loadpx
      }
    }

    when (stateReg === addb0) {
      /*
      adding bias to nodes in hidden layer. A simple loop over all nodes, adding bias directly from memory.
      Transition to layer 2 when done.
      */
      when (nodeIdx < UInt(100)) {
                                              // add bias to node (directly from memory to save clock cycles)
        val relu = layer1(nodeIdx) + memory.io.rdData
                                              // standard tensorflow relu: max(0, x)
        when (relu > SInt(0)) {
          layer1(nodeIdx) := relu
        } .otherwise {
          layer1(nodeIdx) := SInt(0)
        }

        memory.io.rdAddr := readAddrB         // update memory address for next cycle
        readAddrB := readAddrB + UInt(1)      // increment read address for next cycle
        nodeIdx := nodeIdx + UInt(1)          // increment node index for next cycle
      }
      .otherwise {
        memory.io.rdAddr := readAddrW         // next value we'll want to read is a weight
        readAddrW := readAddrW + UInt(1)      // increment weight address for next cycle
        nodeIdx := UInt(0)                    // reset node index
        outputIdx := UInt(0)                  // set output index
        stateReg := incNode1
      }
    }

    when (stateReg === incNode1) {
      /*
      calculating second layer nodes. For every node in the hidden layer, load the weights to all
      ten output nodes and multiply-accumulate. When done, transition to adding bias to the output layer
      */
      readAddrW := readAddrW + UInt(1)        // increment read address for next cycle

      when (nodeIdx < UInt(100)) {
        memory.io.rdAddr := readAddrW         // update memory address for next cycle
        when (outputIdx < UInt(10)) {
          layer2(outputIdx) := layer2(outputIdx) + layer1(nodeIdx) * memory.io.rdData
          outputIdx := outputIdx + UInt(1)    // increment output idx  
        }
        .otherwise {
          outputIdx := UInt(0)                // reset output idx
          nodeIdx := nodeIdx + UInt(1)        // increment node index for next cycle
        }
      }
      .otherwise {
        memory.io.rdAddr := readAddrB         // next value will be a bias
        readAddrB := readAddrB + UInt(1)      // increment bias addr
        stateReg := addb1
      }
    }

    when (stateReg === addb1) {
      /*
      adding bias to output nodes.
      */
      when (outputIdx < UInt(10)) {
        layer2(outputIdx) := layer2(outputIdx) + memory.io.rdData
        memory.io.rdAddr := readAddrB         // next value will be a bias
        readAddrB := readAddrB + UInt(1)      // increment bias addr
        outputIdx := outputIdx + UInt(1)
      }
      .otherwise {
        stateReg := writeRes
      }
    }
    when (stateReg === writeRes) {
      /*
      extracting the index of the maximum value in the output layer.
      Just a bunch of multiplexors with >= comparators as flags.
      After this state, outputReg holds the inference result.
      */
      val max01 = Wire(0.U(32.W))
      val val01 = Wire(0.S(32.W))
      val max23 = Wire(0.U(32.W))
      val val23 = Wire(0.S(32.W))
      val max45 = Wire(0.U(32.W))
      val val45 = Wire(0.S(32.W))
      val max67 = Wire(0.U(32.W))
      val val67 = Wire(0.S(32.W))
      val max89 = Wire(0.U(32.W))
      val val89 = Wire(0.S(32.W))

      when (layer2(UInt(0))>=layer2(UInt(1))) {
        max01 := UInt(0)
        val01 := layer2(UInt(0))
      }.otherwise {
        max01 := UInt(1)
        val01 := layer2(UInt(1))
      }

      when (layer2(UInt(2))>=layer2(UInt(3))) {
        max23 := UInt(2)
        val23 := layer2(UInt(2))
      }.otherwise {
        max23 := UInt(3)
        val23 := layer2(UInt(3))
      }

      when (layer2(UInt(4))>=layer2(UInt(5))) {
        max45 := UInt(4)
        val45 := layer2(UInt(4))
      }.otherwise {
        max45 := UInt(5)
        val45 := layer2(UInt(5))
      }

      when (layer2(UInt(6))>=layer2(UInt(7))) {
        max67 := UInt(6)
        val67 := layer2(UInt(6))
      }.otherwise {
        max67 := UInt(7)
        val67 := layer2(UInt(7))
      }

      when (layer2(UInt(8))>=layer2(UInt(9))) {
        max89 := UInt(8)
        val89 := layer2(UInt(8))
      }.otherwise {
        max89 := UInt(9)
        val89 := layer2(UInt(9))
      }

      val max0123 = Wire(0.U(32.W))
      val val0123 = Wire(0.S(32.W))
      val max4567 = Wire(0.U(32.W))
      val val4567 = Wire(0.S(32.W))

      when (val01 >= val23) {
        max0123 := max01
        val0123 := val01
      } .otherwise {
        max0123 := max23
        val0123 := val23
      }

      when (val45 >= val67) {
        max4567 := max45
        val4567 := val45
      } .otherwise {
        max4567 := max67
        val4567 := val67
      }

      val max127 = Wire(0.U(32.W))
      val val127 = Wire(0.S(32.W))

      when (val0123 >= val4567) {
        max127 := max0123
        val127 := val0123
      } .otherwise {
        max127 := max4567
        val127 := val4567
      }

      when (val127 >= val89) {
        outputReg := max127
      }.otherwise {
        outputReg := max89
      }

      stateReg := clear
    }
    when (stateReg === clear) {
      /*
      reset all node registers to 0
      */
      when (outputIdx < UInt(10)) {
        layer2(outputIdx) := SInt(0)
        outputIdx := outputIdx + UInt(1)
      }
      when (nodeIdx < UInt(100)) {
        layer1(nodeIdx) := SInt(0)
        nodeIdx := nodeIdx + UInt(1)
      }
      .otherwise {
        stateReg := idle
      }
    }
  }

  // handle reads. Only for returning results and debugging
  when(masterReg.Cmd === OcpCmd.RD){
    io.ocp.S.Resp := OcpResp.DVA
    switch(masterReg.Addr(4,0)) {
      //Read status
      is(0x4.U) { 
        io.ocp.S.Data := stateReg
      }
      //Read value from memory
      is(0xC.U) { 
        memory.io.rdAddr := memoryAddress
        io.ocp.S.Data := (memory.io.rdData).asUInt
      }
      //Read result
      is(0x8.U) {
        io.ocp.S.Data := outputReg
      }
    }
  }
}

/*
 instantiate component for synthesis
 */
object AcceleratorMain extends App {
  chisel3.Driver.execute(args, () => new Accelerator())
}