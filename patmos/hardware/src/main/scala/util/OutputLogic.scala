package util

import Chisel._

import patmos.Constants._

class OutputLogic() extends Module {
    val io = IO(new Bundle {
        val enable = Input(Bool ())
        val values = Input(Vec(10,SInt (32.W)))
        val result = Output(SInt (32.W))
    })
    
    val countReg = Reg(init = UInt(0, 32))
    val maxIndex = UInt(0)
    val maxValue = io.values(0)


    when(io.enable)
    {
        switch(countReg){
            is(1.U){
                when(io.values(1) > maxValue){
                    maxValue := io.values(1)
                    maxIndex := UInt(1)
                }
            }
            is(2.U){
                when(io.values(2) > maxValue){
                    maxValue := io.values(2)
                    maxIndex := UInt(2)
                }
            }
            is(3.U){
                when(io.values(3) > maxValue){
                    maxValue := io.values(3)
                    maxIndex := UInt(3)
                }
            }
            is(4.U){
                when(io.values(4) > maxValue){
                    maxValue := io.values(4)
                    maxIndex := UInt(4)
                }
            }
            is(5.U){
                when(io.values(5) > maxValue){
                    maxValue := io.values(5)
                    maxIndex := UInt(5)
                }
            }
            is(6.U){
                when(io.values(6) > maxValue){
                    maxValue := io.values(6)
                    maxIndex := UInt(6)
                }
            }
            is(7.U){
                when(io.values(7) > maxValue){
                    maxValue := io.values(7)
                    maxIndex := UInt(7)
                }
            }
            is(8.U){
                when(io.values(8) > maxValue){
                    maxValue := io.values(8)
                    maxIndex := UInt(8)
                }
            }
            is(9.U){
                when(io.values(9) > maxValue){
                    maxValue := io.values(9)
                    maxIndex := UInt(9)
                }
            }

        }
        countReg := countReg + UInt(1)
    }
    .otherwise
    {
        countReg := UInt(0)
    }

    io.result := maxIndex
}
