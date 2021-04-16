package ptp1588assist

import Chisel._
import ocp.{OcpCmd, OcpResp}
import sys.process._
import scala.language.postfixOps
/*commented out Chisel3 tester has changed see https://github.com/schoeberl/chisel-examples/blob/master/TowardsChisel3.md 
class RTCTester(dut: RTC, testCycles: Int) extends Tester(dut) {
  var asked: Boolean = false
  var readSeconds: Boolean = false
  for (i <- 0 until testCycles) {
    step(1)
    peek(dut.io.ptpTimestamp)
    peek(dut.io.pps)
    if (!asked && i > testCycles / 10) {
      asked = true
      poke(dut.io.ocp.M.Cmd, OcpCmd.WR.litValue()) //Write
      poke(dut.io.ocp.M.Addr, 0xF00DE820)
      poke(dut.io.ocp.M.Data, -1024)
    } else if (asked && i % 50 == 0) {
      poke(dut.io.ocp.M.Cmd, OcpCmd.RD.litValue()) //Read
      if (!readSeconds) {
        poke(dut.io.ocp.M.Addr, 0xF00DE800)
        readSeconds = true
      } else {
        poke(dut.io.ocp.M.Addr, 0xF00DE804)
        readSeconds = false
      }
    } else if (peek(dut.io.ocp.S.Resp) == OcpResp.DVA.litValue()) {
      poke(dut.io.ocp.M.Cmd, OcpCmd.IDLE.litValue())
    }
  }
  expect(dut.correctionStepReg, 0)
}

object RTCTester extends App {
  private val pathToVCD = "generated/" + this.getClass.getSimpleName.dropRight(1)
  private val nameOfVCD = this.getClass.getSimpleName.dropRight(7) + ".vcd"
  try{
    chiselMainTest(Array("--genHarness", "--test", "--backend", "c",
      "--compile", "--vcd", "--targetDir", "generated/" + this.getClass.getSimpleName.dropRight(1)),
      () => Module(new RTC(80000000, 32, 32, 0x5ac385dcL, 100))) {
      dut => new RTCTester(dut, testCycles = 10000)
    }
  } finally {
    "gtkwave " + pathToVCD + "/" + nameOfVCD + " " + pathToVCD + "/" + "view.sav" !
  }
}*/