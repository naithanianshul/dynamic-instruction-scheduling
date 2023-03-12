# Dynamic Instruction Scheduling
 A simulator for an out-of-order superscalar processor that fetches and issues N instructions per cycle and models the dynamic scheduling mechanism by assuming perfect caches and perfect branch prediction.

To run the simulator first compile the code by executing the command 'make'.<br/>
The format of the command to run the branch prediction simulator is:

* To run Bimodal Predictor -
>./sim<br/>
>&emsp;&emsp;< ROB_SIZE ><br/>
>&emsp;&emsp;< IQ_SIZE ><br/>
>&emsp;&emsp;< WIDTH ><br/>
>&emsp;&emsp;<trace_file><br/>

Example - The processor specifications are:
* No of entries in Reorder Buffer (ROB_SIZE) = 16
* No of entries in Issue Queue (IQ_SIZE) = 8
* Superscalar width (WIDTH) = 1
* trace_file = val_trace_gcc

```./sim 16 8 1 val_trace_gcc```
