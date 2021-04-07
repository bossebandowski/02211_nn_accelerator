import chisel3.iotesters.PeekPokeTester
import org.scalatest._

class NeuronTest(dut: Neuron) extends PeekPokeTester(dut) {
    // test spec goes here.
    // 'poke' to set input
    // 'step' to advance clk 
    // 'expect' to check output

    poke(dut.io.inVals(0), 1)
    poke(dut.io.inVals(1), 0)
    poke(dut.io.inVals(2), 0)
    poke(dut.io.weights(0), 1)
    poke(dut.io.weights(1), 0)
    poke(dut.io.weights(2), 0)
    poke(dut.io.biases(0), 1)
    poke(dut.io.biases(1), 0)
    poke(dut.io.biases(2), 0)

    step(1)

    expect(dut.io.dout(0), 1)

}

class NeuronSpec extends FlatSpec with Matchers {
    "Neuron" should "pass" in {
        chisel3.iotesters.Driver(() => new Neuron(3, 16)) { c => new NeuronTest(c)} should be (true)
    }
}
