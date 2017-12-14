# APEX-Simulator

A cycle-by-cycle simulator for an in-order APEX pipeline with three different function units.

![alt text](https://github.com/deshpandekartik/APEX-Simulator/blob/master/images/apexFUs.png)

## Summary

- The Integer FU has a latency of 1 cycle and implements all operations that involve integer arithmetic (ADD,
SUB, ADDC, address computation of LOADs and STOREs etc.) 
- The MUL (for multiply) instruction, which is a register-to-register instruction like the ADD, uses two pipeline stages in sequence, MUL1 and
MUL2, each with a one cycle latency. 
- The DIV ( for division ) instruction uses four pipeline stages in sequence


## Registers and Memory

**Registers** Use of 16 architectural registers, R0 through R15. 
**Memory** Memory for data is viewed as a linear array of integer values (4 Bytes wide). Data
memory ranges from 0 through 3999 and memory addresses correspond to a Byte address that begins the
first Byte of the 4-Byte group that makes up a 4 Byte data item. 

## Instruction Set:

The instructions supported are:
-  Register-to-register instructions: ADD, SUB, MUL, DIV, MOVC, AND, OR, EX-OR (all done on the Integer
ALU). 
- MOVC <register> <literal>, moves literal value into specified register. The MOVC uses the ALU
stages to add 0 to the literal and updates the destination register from the WB stage.
- Memory instructions: LOAD, STORE - both include a literal value whose content is added to a register
to compute the memory address.
- Control flow instructions: BZ, BNZ, JUMP, JAL and HALT. Instructions following a BZ, BNZ, JAL and JUMP
instruction in the pipeline will be flushed on a taken branch. 

## Data Forwarding
The forwarding happens at the end of
the clock cycle when the instruction producing the results (including flag values) are in final stages of the
respective FUs (integer, multiplier, divider). If the forwarded result(s) is/are the only source data that the
instruction in D/RF was waiting for, it can be issued at the beginning of the clock cycle that immediately
follows the one when forwarding took place. 


## Classes

- **Code memory**
Class used as and array of objects, each object is stored as a instruction. 
	
- **Register**
Class used as array of objects rangining from 0 – 15  ( each corresponding to R0, R1, R2, R3.... R15). Each register will have a value field and a status field.  Status used to check if corresponding register is valid or invalid for interlocking to work

- **Data memory**
Class used as an array of object each object corresponding to a data memory location.


## Structures

- **Display class**
This structure will store all data to be displayed in it while simulate() is running.

- **Forwarding bus**
This structure will store data forwarded from execute stage to decode stage. The stored data is then read in decode stage. The struct has 3 fields register number, value and status
 

## Working

- The project uses a bottom up approach, where writeback stage is executed first followed by memory , execute, decode and fetch.
- Each stage will have its own buffer space where it can store currently processed instruction.
- When a instruction enters into execute stage it will use the executer_buffer class array to append the current instruction in its first cycle. On the next cycle it will append another instruction ( if there is any ) to the array and send the  instruction to memory stage based on priority , DIV -> MUL -> ALLOTHERINSTRUCTIONS.

![alt text](https://github.com/deshpandekartik/APEX-Simulator/blob/master/images/dataflow.png)


### Contact
- [Kartik Deshpande](https://www.linkedin.com/in/kartik-deshpande/)
