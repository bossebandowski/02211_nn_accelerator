package util

import chisel3._

class Memory() extends Module {
    val SIZE = 18 // power of 2, defines the number of entries in the memory

    val io = IO(new Bundle {
        val rdAddr = Input(UInt (SIZE.W))
        val rdData = Output(UInt (8.W))
        val wrEna = Input(Bool ())
        val wrData = Input(UInt (8.W))
        val wrAddr = Input(UInt (SIZE.W))
    })

    val mem = SyncReadMem (2^SIZE, UInt (8.W))
    
    io.rdData := mem.read(io.rdAddr)
    when(io.wrEna) {
        mem.write(io.wrAddr , io.wrData)
    }
}
