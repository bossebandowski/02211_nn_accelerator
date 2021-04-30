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

  val layer1 = Vec(100, Reg(init = SInt(0, 32)))
  val layer2 = Vec(10, Reg(init = SInt(0, 32)))

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
  val nonn :: loadnn :: idle :: infload :: infrun :: loadpx :: loadw :: loadb :: clear :: Nil = Enum(UInt(), 9)
  val stateReg = Reg(init = nonn)

  // set memory address. Only for debugging
  when (masterReg.Cmd === OcpCmd.WR && masterReg.Addr(4,0) === 0x10.U) {
    io.ocp.S.Resp := OcpResp.DVA
    memoryAddress := masterReg.Data
  }
  .otherwise {
    // state machine goes here
    when (stateReg === nonn && masterReg.Cmd === OcpCmd.WR) {
        io.ocp.S.Resp := OcpResp.DVA
        when (masterReg.Data === UInt(0)) {
          stateReg := loadnn
          writeAddr := weightAddrZero
      }
    }
    when (stateReg === loadnn && masterReg.Cmd === OcpCmd.WR) {
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
      io.ocp.S.Resp := OcpResp.DVA
      memory.io.wrEna := false.B
      when(masterReg.Data(0) === UInt(0)) {
        stateReg := loadnn
        writeAddr := weightAddrZero
      }
      .otherwise {
        stateReg := infload
        writeAddr := imgAddrZero
      }
    }
    when (stateReg === infload && masterReg.Cmd === OcpCmd.WR) {
      io.ocp.S.Resp := OcpResp.DVA
      memory.io.wrEna := true.B
      memory.io.wrAddr := writeAddr
      memory.io.wrData := (masterReg.Data).asSInt
      when (writeAddr === weightAddrZero - UInt(1)) {
        readAddrP := imgAddrZero
        readAddrW := weightAddrZero
        readAddrB := biasAddrZero
        stateReg := infrun
      }
      writeAddr := writeAddr + UInt(1)
    }
    when (stateReg === infrun) {
        memory.io.wrEna := false.B
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