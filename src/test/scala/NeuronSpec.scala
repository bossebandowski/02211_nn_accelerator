import chisel3.iotesters.PeekPokeTester
import org.scalatest._

class NeuronTest(dut: Neuron) extends PeekPokeTester(dut) {
    // test spec goes here.
    // 'poke' to set input
    // 'step' to advance clk 
    // 'expect' to check output

    poke(dut.io.in0, 1)
    poke(dut.io.in1, 0)
    poke(dut.io.in2, 0)
    poke(dut.io.w0, 1)
    poke(dut.io.w1, 0)
    poke(dut.io.w2, 0)
    poke(dut.io.b0, 1)
    poke(dut.io.b1, 0)
    poke(dut.io.b2, 0)

    step(1)

    expect(dut.io.dout, 1)

}

class NeuronSpec extends FlatSpec with Matchers {
    "Neuron" should "pass" in {
        chisel3.iotesters.Driver(() => new Neuron) { c => new NeuronTest(c)} should be (true)
    }
}
