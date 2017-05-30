virtCPU
=======

 

### Description:

>   This project is a Virtual CPU interface made using the 32-bit ARM CPU
>   architecture. The CPU uses a load/store architecture and has a register file
>   of 16 registers. The simulation is at register level, it allows users to
>   test machine level programs written for the CPU (read a binary file into
>   memory, display memory, modify memory, execute instructions and display
>   registers). Instruction types include Load/Store, Data Processing, Branch
>   and Conditional Branch.

 

### Test File Included(documentation/test):

This test program at first executes an Unconditional Branch then loads ‘FE’ into
registers 2 and 3, then executes an add instruction to add the two values and
store the result  ‘1FC’ into register 3, the program executes a conditional
branch at location ‘2C’, finally at location ‘35’ the stop instruction executes
to set the Stop flag to end the program.

 

### Helpful Links(They help understand the code better):

-   In documentation folder the “ARM-Cortex M3 Technical Reference Manual” PDF

-   http://infocenter.arm.com/help/topic/com.arm.doc.dui0552a/CIHDFHCC.html

 
