# One-Way Shared Memory

These applications are used for the evaluation section of the following submitted paper:

Martin Schoeberl and Rasmus Ulslev Pedersen, One-Way Memory, submitted to the
Journal of Systems Architecture.

We use the T-CREST multicore to evaluate the one-way shared memory.
General build instructions of T-CREST in [Main README](../../../README.md).

Before building the Patmos processor,  tests, add the following lines after `<frequency Hz="80000000"/>` in 
[altde2-115.xml](../../../hardware/config/altde2-115.xml):
```
<cores count="4" />
<cmp device="2" />
<pipeline dual="false" />
```

The C programs for the tests are found at 
[hello_oneway.c](hello_oneway.c)

To run the tests on a DE2-115 board, first connect it, 
and then from `t-crest/patmos` run 
```bash
make gen synth
```
This creates Patmos. To configure the FPGA with Patmos, run:
```bash
make config
```
Afterwards run:
```bash
make app download APP=oneway 
```
This compiles and downloads a simple test for the one-way shared memory.

Further application examples are available at following repository:
[One-way memory usecases](https://github.com/schoeberl/one-way-shared-memory/tree/master/usecases)


To ensure that you have the exact version of T-CREST that we have used in the
evaluation section of the paper, use the following `git` command to checkout that version:

```bash
git checkout `git rev-list -n 1 --before="2018-05-17" master`
```

This can be done in all T-CREST repositories. However, it is most important
in `patmos`.
