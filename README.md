# Dynamic Instruction Scheduling
 A simulator for an out-of-order superscalar processor that fetches and issues N instructions per cycle and models the dynamic scheduling mechanism by assuming perfect caches and perfect branch prediction.

The processor has a Reorder Buffer, Issue Queue, Rename Map Table, and Architectural Register File.
The implemented pipeline and ROB, IQ, RMT, and ARF structures are as below.

<img width="875" alt="Pipeline_and_Structures" src="https://user-images.githubusercontent.com/39558258/224788081-41457ec1-6bbf-4de2-b1b0-7983136a616e.png">

To run the simulator first compile the code by executing the command 'make'.<br/>
The format of the command to run the branch prediction simulator is:

* To run Bimodal Predictor -
>./sim<br/>
>&emsp;&emsp;<ROB_SIZE><br/>
>&emsp;&emsp;<IQ_SIZE><br/>
>&emsp;&emsp;<WIDTH><br/>
>&emsp;&emsp;<trace_file><br/>

Example - The processor specifications are:
* No of entries in Reorder Buffer (ROB_SIZE) = 16
* No of entries in Issue Queue (IQ_SIZE) = 8
* Superscalar width (WIDTH) = 1
* trace_file = val_trace_gcc

```./sim 16 8 1 val_trace_gcc```
