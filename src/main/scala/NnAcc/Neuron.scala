import chisel3._
import chisel3.experimental.FixedPoint


// an example 3-input neuron
class Neuron(edges_in: Int, width: Int) extends Module {
    // io stuff
    val io = IO(new Bundle {
        val inVals = Input(Vec(edges_in, FixedPoint((width.W),5.BP)))
        val weights = Input(Vec(edges_in, FixedPoint((width.W),5.BP)))
        val biases = Input(Vec(edges_in, FixedPoint((width.W),5.BP)))
        val dout = Output(FixedPoint((width.W),5.BP))
    })

    // intermediate results (all incoming edges)
    val tmp = Wire(Vec(edges_in, FixedPoint((width.W),5.BP)))
    val mac = Wire(Vec(edges_in, FixedPoint((width.W),5.BP)))

    for (i <- 0 until edges_in) {
        tmp(i) := io.inVals(i) * io.weights(i) + io.biases(i)
    }

    // sum
    mac(0) := tmp(0)
    for (i <- 1 until edges_in) {
        mac(i) := mac(i - 1) + tmp(i)
    }
    
    // activation (ReLU: linear mapping in [0..1])
    val mac_cap_top = Wire(FixedPoint((width.W),5.BP))
    val mac_cap = Wire(FixedPoint((width.W),5.BP))

    mac_cap_top := Mux(mac(edges_in - 1) > 1.F(width.W,5.BP), 1.F(width.W,5.BP), mac(edges_in - 1))
    mac_cap := Mux(mac_cap_top < 0.F(width.W,5.BP), 0.F(width.W,5.BP), mac_cap_top)

    // output assignment
    io.dout := mac_cap
}