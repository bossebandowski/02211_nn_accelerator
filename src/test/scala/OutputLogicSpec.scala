import chisel3._
import chisel3.iotesters.PeekPokeTester
import org.scalatest._
import util._

class NeuronTest(dut: OutputLogic) extends PeekPokeTester(dut) {
    // test spec goes here.
    // 'poke' to set input
    // 'step' to advance clk 
    // 'expect' to check output
    val values1 = Vec(10, Reg(SInt(32.W)))
    values1(0) = 12.S
    values1(1) = 125.S
    values1(2) = 354.S
    values1(3) = 1564.S
    values1(4) = 458.S
    values1(5) = 15555.S
    values1(6) = 148.S
    values1(7) = 1234.S
    values1(8) = 11554.S
    values1(9) = 4455.S


    poke(dut.io.enable, 1.U)
    poke(dut.io.values, 0.U)

    step(10)

    expect(dut.io.result, 5)

}

class NeuronSpec extends FlatSpec with Matchers {
    "OutputLogic" should "pass" in {
        chisel3.iotesters.Driver(() => new OutputLogic) { c => new OutputLogicTest(c)} should be (true)
    }
}
