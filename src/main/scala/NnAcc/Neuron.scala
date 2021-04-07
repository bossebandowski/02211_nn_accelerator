import chisel3._


// an example 3-input neuron
class Neuron(edges_in: Int, width: Int) extends Module {
    // io stuff
    val io = IO(new Bundle {
        val inVals = Input(Vec(edges_in, UInt(width.W)))
        val weights = Input(Vec(edges_in, UInt(width.W)))
        val biases = Input(Vec(edges_in, UInt(width.W)))
        val dout = Output(UInt(16.W))
    })

    // intermediate results (all incoming edges)
    val tmp = Wire(Vec(edges_in, UInt(width.W)))
    val mac = Wire(Vec(edges_in, UInt(width.W)))

    for (i <- 0 until edges_in) {
        tmp(i) := io.inVals(i) * io.weights(i) + io.biases(i)
    }

    // sum
    mac(0) := tmp(0)
    for (i <- 1 until edges_in) {
        mac(i) := mac(i - 1) + tmp(i)
    }
    
    // activation (ReLU: linear mapping in [0..1])
    val mac_cap_top = Wire(UInt())
    val mac_cap = Wire(UInt())

    mac_cap_top := Mux(mac(edges_in - 1) > 1.U, 1.U, mac(edges_in - 1))
    mac_cap := Mux(mac_cap_top < 0.U, 0.U, mac_cap_top)

    // output assignment
    io.dout := mac_cap
}