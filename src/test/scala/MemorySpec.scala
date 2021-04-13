import chisel3._
import chisel3.iotesters.PeekPokeTester
import org.scalatest._

class MemoryTest(dut: Memory) extends PeekPokeTester(dut) {
    // Write '132' to addr 165
    poke(dut.io.wrAddr, 165.U)
    poke(dut.io.wrData, 132.U)
    poke(dut.io.wrEna, 1.B)
    step(1)
    // Write '255' to addr 3558
    poke(dut.io.wrAddr, 3558.U)
    poke(dut.io.wrData, 255.U)
    poke(dut.io.wrEna, 1.B)
    step(1)
    // Write '1' to addr 0
    poke(dut.io.wrAddr, 0.U)
    poke(dut.io.wrData, 1.U)
    poke(dut.io.wrEna, 1.B)
    step(1)

    // read from addr 165
    poke(dut.io.rdAddr, 165.U)
    step(1)
    expect(dut.io.rdData, 132)
    // test consistency
    step(10)
    poke(dut.io.rdAddr, 165.U)
    step(1)
    expect(dut.io.rdData, 132)

    // read from addr 3558
    poke(dut.io.rdAddr, 3558.U)
    step(1)
    expect(dut.io.rdData, 255)
    
    // read from addr 0
    poke(dut.io.rdAddr, 0.U)
    step(1)
    expect(dut.io.rdData, 1)
}

class MemorySpec extends FlatSpec with Matchers {
    "Memory" should "pass" in {
        chisel3.iotesters.Driver(() => new Memory) { c => new MemoryTest(c)} should be (true)
    }
}
