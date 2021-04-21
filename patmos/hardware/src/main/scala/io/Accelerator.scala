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
  val slaveReg = Reg(next = io.ocp.S)
  val pReg = Reg(init = UInt(0, 8))
  val wReg = Reg(init = UInt(0, 8))
  val bReg = Reg(init = UInt(0, 8))

  // memory
  val memory = Module(new Memory())

  // address constants
  val imgAddrZero = UInt(0)
  val weightAddrZero = UInt(784)
  val biasAddrZero = UInt(80184)
  val lastAddr = UInt(159584)

  // network constants

  // states
  val nonn :: loadnn :: idle :: infload :: infrun :: loadpx :: loadw :: loadb :: Nil = Enum(UInt(), 8)
  val stateReg = Reg(init = nonn)

  val outputReg = Reg(init = UInt(15, 4))

  // state machine goes here
  when (masterReg.Cmd === OcpCmd.WR) {
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
      memory.io.wrData := masterReg.Data
      writeAddr := writeAddr + UInt(1)
      when (writeAddr === lastAddr) {
        memory.io.wrEna := false.B
        stateReg := idle
      }
    }
    when (stateReg === idle) {
      outputReg := UInt(3)
      when(masterReg.Data(0) === UInt(0)) {
        stateReg := loadnn
        writeAddr := weightAddrZero
      }
      .otherwise {
        stateReg := infload
        writeAddr := imgAddrZero
      }
    }
    when (stateReg === infload) {
      outputReg := UInt(4)
      memory.io.wrEna := true.B
      memory.io.wrAddr := writeAddr
      memory.io.wrData := masterReg.Data
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
  
  val respReg = Reg(init = OcpResp.NULL)
  respReg := OcpResp.NULL
  when(masterReg.Cmd === OcpCmd.RD || masterReg.Cmd === OcpCmd.WR) {
    respReg := OcpResp.DVA
  }
  
  slaveReg.Data := outputReg  
  slaveReg.Resp := respReg
}
