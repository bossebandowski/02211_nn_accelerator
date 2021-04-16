1; Define IO hardware componenets in hardware/src/main/scala/io/
2; Add the IO devices to the initialization xml file (See Patmos handbook: page 56)
2; Write Patmos code in C in c/
3; Compile and Start hardware emulation:
```
make tools
make emulator
make comp APP=<APP NAME>
patemu tmp/<APP NAME>.elf

```

