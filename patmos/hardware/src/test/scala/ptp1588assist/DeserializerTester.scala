package ptp1588assist

import Chisel._
import sys.process._
import scala.language.postfixOps
/*commented out Chisel3 tester has changed see https://github.com/schoeberl/chisel-examples/blob/master/TowardsChisel3.md 
class DeserializerTester(dut: Deserializer) extends Tester(dut) {
  // Shifting of discrete numbers for clarity
  poke(dut.io.en, true)
  for (i <- EthernetTesting.mockupPTPEthFrameOverIpUDP.preambleNibbles) {
    poke(dut.io.shiftIn, i)
    peek(dut.io.shiftOut)
    peek(dut.io.done)
    step(1)
  }
  expect(dut.io.done, true)
  poke(dut.io.en, false)
  step(1)
  expect(dut.io.shiftOut, 0xD5)
  poke(dut.io.clr, true)
  step(1)
  expect(dut.countReg, 8 / 4 - 1)
  expect(dut.shiftReg, 0)
  expect(dut.doneReg, false)
}

object DeserializerTester extends App {
  private val pathToVCD = "generated/" + this.getClass.getSimpleName.dropRight(1)
  private val nameOfVCD = this.getClass.getSimpleName.dropRight(7) + ".vcd"

  chiselMainTest(Array("--genHarness", "--test", "--backend", "c",
    "--compile", "--vcd", "--targetDir", "generated/" + this.getClass.getSimpleName.dropRight(1)),
    () => Module(new Deserializer(false, 4, 8))) {
    dut => new DeserializerTester(dut)
  }
  "gtkwave " + pathToVCD + "/" + nameOfVCD + " " + pathToVCD + "/" + "view.sav" !
}*/

