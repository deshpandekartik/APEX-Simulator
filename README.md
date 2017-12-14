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

## Setup

```
make clean
make all
```

## Sample Output

```
➜  APEX-Simulator git:(master) ✗ ./apex.out testcases/test.txt 
1. initialize : This is to initialize Program Counter to point to first instruction 
2. simulate : This is simulate the no of cycles specified as <n> 
3. display : Displays the contents of each stage in the pipeline, all registers, and the contents of the first 100 memory locations 
4. Exit : To terminate simulator 
 
initialize
Initialization success 

1. initialize : This is to initialize Program Counter to point to first instruction 
2. simulate : This is simulate the no of cycles specified as <n> 
3. display : Displays the contents of each stage in the pipeline, all registers, and the contents of the first 100 memory locations 
4. Exit : To terminate simulator 
 
simulate 20
Simulation success 

1. initialize : This is to initialize Program Counter to point to first instruction 
2. simulate : This is simulate the no of cycles specified as <n> 
3. display : Displays the contents of each stage in the pipeline, all registers, and the contents of the first 100 memory locations 
4. Exit : To terminate simulator 
 
display

Cycle 1
Fetch : (I0) MOVC,R10,#400
Decode : Empty
INTFU : Empty
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : Empty
DIV4 : Empty
Memory : Empty
Witeback : Empty

Cycle 2
Fetch : (I1) ADD,R9,R10,R0
Decode : (I0) MOVC,R10,#400
INTFU : Empty
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : Empty
DIV4 : Empty
Memory : Empty
Witeback : Empty

Cycle 3
Fetch : (I2) MUL,R2,R9,R00
Decode : (I1) ADD,R9,R10,R0
INTFU : (I0) MOVC,R10,#400
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : Empty
DIV4 : Empty
Memory : Empty
Witeback : Empty

Cycle 4
Fetch : (I3) HALT
Decode : (I2) MUL,R2,R9,R00
INTFU : (I1) ADD,R9,R10,R0
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : Empty
DIV4 : Empty
Memory : (I0) MOVC,R10,#400
Witeback : Empty

Cycle 5
Fetch : Empty
Decode : (I3) HALT
INTFU : Empty
Mul1 : (I2) MUL,R2,R9,R00
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : Empty
DIV4 : Empty
Memory : (I1) ADD,R9,R10,R0
Witeback : (I0) MOVC,R10,#400

Cycle 6
Fetch : Empty
Decode : Empty
INTFU : Empty
Mul1 : Empty
Mul2 : (I2) MUL,R2,R9,R00
DIV1 : (I3) HALT
DIV2 : Empty
DIV3 : Empty
DIV4 : Empty
Memory : Empty
Witeback : (I1) ADD,R9,R10,R0

Cycle 7
Fetch : Empty
Decode : Empty
INTFU : Empty
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : (I3) HALT
DIV3 : Empty
DIV4 : Empty
Memory : (I2) MUL,R2,R9,R00
Witeback : Empty

Cycle 8
Fetch : Empty
Decode : Empty
INTFU : Empty
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : (I3) HALT
DIV4 : Empty
Memory : Empty
Witeback : (I2) MUL,R2,R9,R00

Cycle 9
Fetch : Empty
Decode : Empty
INTFU : Empty
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : Empty
DIV4 : (I3) HALT
Memory : Empty
Witeback : Empty

Cycle 10
Fetch : Empty
Decode : Empty
INTFU : Empty
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : Empty
DIV4 : Empty
Memory : (I3) HALT
Witeback : Empty

Cycle 11
Fetch : Empty
Decode : Empty
INTFU : Empty
Mul1 : Empty
Mul2 : Empty
DIV1 : Empty
DIV2 : Empty
DIV3 : Empty
DIV4 : Empty
Memory : Empty
Witeback : (I3) HALT
REGISTER [ 0 ] = 0
REGISTER [ 1 ] = 0
REGISTER [ 2 ] = 0
REGISTER [ 3 ] = 0
REGISTER [ 4 ] = 0
REGISTER [ 5 ] = 0
REGISTER [ 6 ] = 0
REGISTER [ 7 ] = 0
REGISTER [ 8 ] = 0
REGISTER [ 9 ] = 400
REGISTER [ 10 ] = 400
REGISTER [ 11 ] = 0
REGISTER [ 12 ] = 0
REGISTER [ 13 ] = 0
REGISTER [ 14 ] = 0
REGISTER [ 15 ] = 0
Data memory Location [0] = 0
Data memory Location [4] = 0
Data memory Location [8] = 0
Data memory Location [12] = 0
Data memory Location [16] = 0
Data memory Location [20] = 0
Data memory Location [24] = 0
Data memory Location [28] = 0
Data memory Location [32] = 0
Data memory Location [36] = 0
Data memory Location [40] = 0
Data memory Location [44] = 0
Data memory Location [48] = 0
Data memory Location [52] = 0
Data memory Location [56] = 0
Data memory Location [60] = 0
Data memory Location [64] = 0
Data memory Location [68] = 0
Data memory Location [72] = 0
Data memory Location [76] = 0
Data memory Location [80] = 0
Data memory Location [84] = 0
Data memory Location [88] = 0
Data memory Location [92] = 0
Data memory Location [96] = 0
Data memory Location [100] = 0
Data memory Location [104] = 0
Data memory Location [108] = 0
Data memory Location [112] = 0
Data memory Location [116] = 0
Data memory Location [120] = 0
Data memory Location [124] = 0
Data memory Location [128] = 0
Data memory Location [132] = 0
Data memory Location [136] = 0
Data memory Location [140] = 0
Data memory Location [144] = 0
Data memory Location [148] = 0
Data memory Location [152] = 0
Data memory Location [156] = 0
Data memory Location [160] = 0
Data memory Location [164] = 0
Data memory Location [168] = 0
Data memory Location [172] = 0
Data memory Location [176] = 0
Data memory Location [180] = 0
Data memory Location [184] = 0
Data memory Location [188] = 0
Data memory Location [192] = 0
Data memory Location [196] = 0
Data memory Location [200] = 0
Data memory Location [204] = 0
Data memory Location [208] = 0
Data memory Location [212] = 0
Data memory Location [216] = 0
Data memory Location [220] = 0
Data memory Location [224] = 0
Data memory Location [228] = 0
Data memory Location [232] = 0
Data memory Location [236] = 0
Data memory Location [240] = 0
Data memory Location [244] = 0
Data memory Location [248] = 0
Data memory Location [252] = 0
Data memory Location [256] = 0
Data memory Location [260] = 0
Data memory Location [264] = 0
Data memory Location [268] = 0
Data memory Location [272] = 0
Data memory Location [276] = 0
Data memory Location [280] = 0
Data memory Location [284] = 0
Data memory Location [288] = 0
Data memory Location [292] = 0
Data memory Location [296] = 0
Data memory Location [300] = 0
Data memory Location [304] = 0
Data memory Location [308] = 0
Data memory Location [312] = 0
Data memory Location [316] = 0
Data memory Location [320] = 0
Data memory Location [324] = 0
Data memory Location [328] = 0
Data memory Location [332] = 0
Data memory Location [336] = 0
Data memory Location [340] = 0
Data memory Location [344] = 0
Data memory Location [348] = 0
Data memory Location [352] = 0
Data memory Location [356] = 0
Data memory Location [360] = 0
Data memory Location [364] = 0
Data memory Location [368] = 0
Data memory Location [372] = 0
Data memory Location [376] = 0
Data memory Location [380] = 0
Data memory Location [384] = 0
Data memory Location [388] = 0
Data memory Location [392] = 0
Data memory Location [396] = 0
Data memory Location [400] = 0


```



### Contact
- [Kartik Deshpande](https://www.linkedin.com/in/kartik-deshpande/)
