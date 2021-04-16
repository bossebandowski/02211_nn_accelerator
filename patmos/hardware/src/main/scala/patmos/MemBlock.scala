
/*
 * A generic dual-ported (one read-, one write-port) memory block
 *
 * Author: Wolfgang Puffitsch (wpuffitsch@gmail.com)
 *
 */

package patmos

import Chisel._

object MemBlock {
  def apply(size : Int, width : Int, bypass : Boolean = true) = {
    Module(new MemBlock(size, width, bypass))
  }
}

class MemBlockIO(size : Int, width : Int) extends Bundle {
  val rdAddr = UInt(INPUT, log2Up(size))
  val rdData = UInt(OUTPUT, width)
  val wrAddr = UInt(INPUT, log2Up(size))
  val wrEna  = UInt(INPUT, 1)
  val wrData = UInt(INPUT, width)

  var read = false
  var write = false

  def <= (ena : UInt, addr : UInt, data : UInt) = {
    // This memory supports only one write port
    if (write) { throw new Error("Only one write port supported") }
    write = true

    wrAddr := addr
    wrEna := ena
    wrData := data
  }

  def apply(addr : UInt) : UInt = {
    // This memory supports only one read port
    if (read) { throw new Error("Only one read port supported") }
    read = true

    rdAddr := addr
    rdData
  }
}

class MemBlock(size : Int, width : Int, bypass : Boolean = true) extends Module {
  val io = new MemBlockIO(size, width)
  val mem = Mem(UInt(width = width), size)

  // write
  when(io.wrEna === UInt(1)) {
    mem(io.wrAddr) := io.wrData
  }

  // read
  val rdAddrReg = Reg(next = io.rdAddr)
  io.rdData := mem(rdAddrReg)

  if (bypass) {
    // force read during write behavior
    when (Reg(next = io.wrEna) === UInt(1) &&
          Reg(next = io.wrAddr) === rdAddrReg) {
            io.rdData := Reg(next = io.wrData)
          }
  }
}

