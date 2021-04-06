import chisel3._


// an example 3-input neuron
class Neuron extends Module {
    // io stuff
    val io = IO(new Bundle {
        val in0 = Input(UInt(16.W))
        val in1 = Input(UInt(16.W))
        val in2 = Input(UInt(16.W))
        val w0 = Input(UInt(16.W))
        val w1 = Input(UInt(16.W))
        val w2 = Input(UInt(16.W))
        val b0 = Input(UInt(16.W))
        val b1 = Input(UInt(16.W))
        val b2 = Input(UInt(16.W))

        val dout = Output(UInt(16.W))
    })

    // definitions
    val mac = Wire(UInt())
    val mac1 = Wire(UInt())
    val mac2 = Wire(UInt())

    val tmp0 = Wire(UInt())
    val tmp1 = Wire(UInt())
    val tmp2 = Wire(UInt())
    
    // mac
    tmp0 := io.in0 * io.w0 + io.b0
    tmp1 := io.in1 * io.w1 + io.b1
    tmp2 := io.in2 * io.w2 + io.b2

    printf("mac init: %d\n", mac)
    mac := tmp0 + tmp1 + tmp2
    printf("mac after add: %d\n", mac)
    // activation (ReLU)
    
    mac1 := Mux(mac > 1.U, 1.U, mac)
    mac2 := Mux(mac1 < 0.U, 0.U, mac1)
    
    printf("mac after ReLU: %d\n", mac2)

    // output assignment
    io.dout := mac2
}