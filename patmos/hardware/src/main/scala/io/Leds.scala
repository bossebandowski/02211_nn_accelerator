/*
 * Simple I/O module for LEDs
 *
 * Authors: Wolfgang Puffitsch (wpuffitsch@gmail.com)
 *
 */

package io

import Chisel._

import ocp._

object Leds extends DeviceObject {
  var ledCount = -1

  def init(params: Map[String, String]) = {
    ledCount = getPosIntParam(params, "ledCount")
  }

  def create(params: Map[String, String]) : Leds = {
    Module(new Leds(ledCount))
  }
}

class Leds(ledCount : Int) extends CoreDevice() {

  override val io = new CoreDeviceIO() with patmos.HasPins {
    override val pins = new Bundle() {
      val led = Bits(OUTPUT, ledCount)
    }
  }

  val ledReg = Reg(init = Bits(0, ledCount))

  // Default response
  val respReg = Reg(init = OcpResp.NULL)
  respReg := OcpResp.NULL

  // Write to LEDs
  when(io.ocp.M.Cmd === OcpCmd.WR) {
    respReg := OcpResp.DVA
    ledReg := io.ocp.M.Data(ledCount-1, 0)
  }

  // Read current state of LEDs
  when(io.ocp.M.Cmd === OcpCmd.RD) {
    respReg := OcpResp.DVA
  }

  // Connections to master
  io.ocp.S.Resp := respReg
  io.ocp.S.Data := ledReg

  // Connection to pins
  io.pins.led := Reg(next = ledReg)
}
