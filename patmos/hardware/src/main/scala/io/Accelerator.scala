package io

import Chisel._

import patmos.Constants._

import ocp._

object Accelerator extends DeviceObject {

  def init(params: Map[String, String]) = {}

  def create(params: Map[String, String]): Accelerator = Module(new Accelerator())
}

class Accelerator() extends CoreDevice() {
  val readAddr = Reg(init = UInt(0, 18))
  val writeAddr = Reg(init = UInt(0, 18))
  val masterReg = Reg(next = io.ocp.M)
  val slaveReg = Reg(next = io.ocp.S)

  // memory
  // val memory = Module(new Memory())

  // address constants
  val imgAddrZero = UInt(0)
  val weightAddrZero = UInt(784)
  val biasAddrZero = UInt(80184)
  val lastAddr = UInt(159584)

  // network constants

  // states
  val nonn :: loadnn :: idle :: infload :: infrun :: Nil = Enum(UInt(), 5)
  val stateReg = Reg(init = nonn)

  val outputReg = Reg(init = UInt(15, 4))

  // state machine goes here
  when (masterReg.Cmd === OcpCmd.WR) {
    when (stateReg === nonn) {
      when(masterReg.Data(0) === UInt(0)){
        stateReg := loadnn
        writeAddr := weightAddrZero
      }
    }
    when (stateReg === loadnn) {
      // memory.io.wrEna := True
      // memory.io.wrAddr := writeAddr
      // memory.io.wrData := masterReg.Data
      writeAddr := writeAddr + UInt(1)
      when (writeAddr === lastAddr) {
        stateReg := idle
      }
    }
    outputReg := masterReg.Data 
  }
  
  val respReg = Reg(init = OcpResp.NULL)
  respReg := OcpResp.NULL
  when(masterReg.Cmd === OcpCmd.RD || masterReg.Cmd === OcpCmd.WR) {
    respReg := OcpResp.DVA
  }
  
  slaveReg.Data := outputReg  
  slaveReg.Resp := respReg
}
