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
  val writeAddr32 = Reg(init = UInt(0, 18))
  val masterReg = Reg(next = io.ocp.M)
  val pReg = Reg(init = UInt(0, 8))
  val wReg = Reg(init = UInt(0, 8))
  val bReg = Reg(init = UInt(0, 8))

  // memory
  val memory = Module(new MemorySInt(18, 8))
  val memory32 = Module(new MemorySInt(18, 32))

  // address constants
  val imgAddrZero = UInt(0)
  val weightAddrZero = UInt(784)
  val biasAddrZero = UInt(80184)
  val lastAddr = UInt(159584)

  //Memort test
  val memTestAdr = Reg(init = UInt(784,18))
  val memTestAdr32 = Reg(init = UInt(0,18))

  // network constants

  // states
  val nonn :: loadnn :: idle :: infload :: infrun :: loadpx :: loadw :: loadb :: Nil = Enum(UInt(), 8)
  val stateReg = Reg(init = nonn)

  val outputReg = Reg(init = UInt(15, 32))

  // state machine goes here
  when (masterReg.Cmd === OcpCmd.WR) {
    switch(masterReg.Addr(4,0)) {
      is(0x0.U) { 
      when (stateReg === nonn) {
        outputReg := UInt(1)
        when(masterReg.Data(0) === UInt(0)) {
          stateReg := loadnn
          writeAddr := weightAddrZero
        }
      }

      when (stateReg === loadnn) {
        outputReg := UInt(2)
        memory.io.wrEna := true.B
        memory.io.wrAddr := writeAddr
        memory.io.wrData := (masterReg.Data).asSInt
        writeAddr := writeAddr + UInt(1)
        when (writeAddr === lastAddr) {
          memory.io.wrEna := false.B
          stateReg := idle
        }
      }

      when (stateReg === idle) {
        outputReg := UInt(3)
        /*when(masterReg.Data(0) === UInt(0)) {
          stateReg := loadnn
          writeAddr := weightAddrZero
        }
        .otherwise {
          stateReg := infload
          writeAddr := imgAddrZero
        }*/
      }

      when (stateReg === infload) {
        outputReg := UInt(4)
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
        outputReg := UInt(5)
      }
    }
      //Write to 32 bit memory
    is(0x8.U){
        memory32.io.wrEna := true.B
        memory32.io.wrAddr := writeAddr32
        memory32.io.wrData := (masterReg.Data).asSInt
        writeAddr32 := writeAddr32 + UInt(1)
      }

    }
  }
  
  val respReg = Reg(init = OcpResp.NULL)
  respReg := OcpResp.NULL

  when(masterReg.Cmd === OcpCmd.RD){
    //Read status
    switch(masterReg.Addr(4,0)) {
      is(0x4.U) { 
        outputReg := stateReg
      }
      //Read 8bit memory
      is(0xC.U) { 
        memory.io.rdAddr := memTestAdr
        outputReg := (memory.io.rdData).asUInt
        memTestAdr := memTestAdr + UInt(1)
      }
      //Read 32bit memory
      is(0x10.U) { 
        memory.io.rdAddr := memTestAdr32
        outputReg := (memory32.io.rdData).asUInt
        memTestAdr32 := memTestAdr32 + UInt(1)
      }
    }
  }

  when(masterReg.Cmd === OcpCmd.RD || masterReg.Cmd === OcpCmd.WR) {
    respReg := OcpResp.DVA
  }
  
  io.ocp.S.Data := outputReg
  io.ocp.S.Resp := respReg
}