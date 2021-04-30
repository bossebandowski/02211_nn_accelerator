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
  //val writeAddr32 = Reg(init = UInt(0, 18))
  val masterReg = Reg(next = io.ocp.M)
  val pReg = Reg(init = UInt(0, 8))
  val wReg = Reg(init = UInt(0, 8))
  val bReg = Reg(init = UInt(0, 8))

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
  val memTestAdr = Reg(init = UInt(783,18))

  // network constants

  // states
  val nonn :: loadnn :: idle :: infload :: infrun :: loadpx :: loadw :: loadb :: Nil = Enum(UInt(), 8)
  val stateReg = Reg(init = nonn)

  // set memory address. Only for debugging
  when (masterReg.Cmd === OcpCmd.WR && masterReg.Addr(4,0) === 0x10.U) {
    io.ocp.S.Resp := OcpResp.DVA
    memTestAdr := masterReg.Data
  }
  .otherwise {
    // state machine goes here
    when (stateReg === nonn) {
      when (masterReg.Cmd === OcpCmd.WR && masterReg.Data === UInt(0)) {
        io.ocp.S.Resp := OcpResp.DVA
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
      /*when(masterReg.Data(0) === UInt(0)) {
        stateReg := loadnn
        writeAddr := weightAddrZero
      }
      .otherwise {
        stateReg := infload
        writeAddr := imgAddrZero
      }*/
    }
    when (stateReg === infload && masterReg.Cmd === OcpCmd.WR) {
      io.ocp.S.Resp := OcpResp.DVA
      memory.io.wrEna := true.B
      memory.io.wrAddr := writeAddr
      memory.io.wrData := (masterReg.Data).asSInt
      writeAddr := writeAddr + UInt(1)
      when (writeAddr === weightAddrZero) {
        memory.io.wrEna := false.B
        readAddrP := imgAddrZero
        readAddrW := weightAddrZero
        readAddrB := biasAddrZero
        stateReg := infrun
      }
    }
    when (stateReg === infrun) {
      // Behaviour TBD
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
        memory.io.rdAddr := memTestAdr
        io.ocp.S.Data := (memory.io.rdData).asUInt
      }
      //Read result
    }
  }
}