name := "Patmos"

scalaVersion := "2.12.12"

scalacOptions ++= Seq("-Xsource:2.11", "-unchecked", "-deprecation", "-feature", "-language:reflectiveCalls")

libraryDependencies += scalaVersion("org.scala-lang" % "scala-compiler" % _).value

resolvers ++= Seq(
  Resolver.sonatypeRepo("snapshots"),
  Resolver.sonatypeRepo("releases")
)

// Chisel 3.3
libraryDependencies += "edu.berkeley.cs" %% "chisel-iotesters" % "1.4.3"
libraryDependencies += "edu.berkeley.cs" %% "chisel3" % "3.1.2"