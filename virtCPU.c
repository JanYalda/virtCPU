/*
# File: virtCPU.c
# Course: CENG356
# Author: Jan Yalda
# Date: Thursday Apr 04/20/17, 2017   04:08 AM
 */
#include "libCPU.h" 

/* Function to load contents of a file into memory*/
int LoadFile(void * memory, unsigned int max)
{
	char buff2[1024];
	FILE *file;
	size_t readBytes;
	// prompt for a filename 
	printf("\nEnter a file name: ");
	scanf("\n%s", &buff2);
	// checking if file exists  
	if ((file = fopen(buff2, "r")) == NULL)
	{
		fprintf(stderr, "\nCouldn't open the file %s\n", buff2);
		return 0;

	}
	else{
		// struct that returns the size of a buffer
		struct stat st;
		stat(buff2, &st);
		// if the file's size is > than the max memory
		if (st.st_size > max){
			// indicate that the file has been truncated 
			printf("\nFile has been truncated!\n");
		}
		// read the content of the file into memory
		readBytes = fread(memory, 1, max + 1, file);
		// closing the file
		fclose(file);
		printf("\nFile has been loaded into memory.\n");
		return readBytes;
	}

}
/* Funciton to write contents of memory to a file*/
void WriteFile(void * memory)
{
	char buff[1024];
	FILE *file;
	int bytes;
	// prompt for a filename then number of bytes
	printf("\nEnter file name: ");
	scanf("\n%s", &buff);
	printf("\nEnter number of Bytes to be written: ");
	scanf("\n%d", &bytes);
	// struct that will return size of the contents saved in memory
	struct stat st;
	stat(memory, &st);
	// if bytes entered > than size of the memory content 
	if (bytes > st.st_size)
	{
		// indicate that bytes entered is too large
		printf("\nYou've entered a larger size than the actual original file!\n");

	}
	else{
		// open the new file for writing 	
		file = fopen(buff, "w");
		// write the content to it
		fwrite(memory, 1, bytes, file);
		printf("\nFile Created\n");
		// close file
		fclose(file);
	}
}
/* Function to dump a selected part of memory*/
void Memdump(void *memptr, unsigned offset, unsigned length)
{
	int i = (unsigned)offset, j, w = offset, z = offset;
	unsigned char *pc = (unsigned char*)memptr;
	int count = 0, count2 = 0, orig = length;
	/* because incrementing by 16 must add the offset to length,
	   to dunmp memory when ofs and len both are set to 16*/
	length += offset;

	for (i; i < length; i += 16)
	{
		printf("%04x:    ", i);
		/* outputing the address*/
		for (j = 0; j < 16; j++){
			printf("%02x ", pc[w++]);
			/* this part terminates the loop if user
			   doesnt select a # like 16, 32, 64 etc..*/
			count++;
			if (count >= orig)
				break;
		}
		printf("   \n\t ");
		/* outputing the char values*/
		for (j = 0; j < 16; j++)
		{
			/* if a valid char value print it else print '.'*/
			printf(" %c ", ((pc[z] > 0x1F) && (pc[z] < 0x80)) ? pc[z] : '.');
			z++;
			/* this part terminates the loop if user
			   doesnt select a # like 16, 32, 64 etc..*/
			count2++;
			if (count2 >= orig)
				break;
		}
		printf("\n");
	};
}
/* Function to modify specified memory location(s)*/
void MemMod(void *memptr, unsigned addrs)
{
	int i = (unsigned)addrs;
	unsigned char *pc = (unsigned char*)memptr;
	unsigned newval;
	printf("\nAddress: %04x\nExisting Value: %02x\n", i, pc[i]);
	while (1)
	{
		/* promt for the new value*/
		printf("Enter a new value(enter '.' to exit): ");
		scanf(" %x", &newval);
		/* checking the value */
		switch (newval)
		{
		case 0x00 ... 0x2D:
		case 0x2F ... 0xFF:
			/* if a hex value change the original one*/
			*((unsigned char*)memptr + i) = newval;
			//pc[i] = newval;
			i++;
			continue;
		case '.':
			/* exit if '.' entered*/
			return;
		default:
			/* otherwise just increment the address*/
			i++;

		}
	}
}

/* Function to check the Sign & Zero flags*/
void flags(unsigned long alu)
{
	/* If ALU is zero set the zeroFlag else unset*/
	if (alu == 0){
		zeroFlag = 1;
	}
	else{
		zeroFlag = 0;
	}
	/* the is AND with the msb32mask and the MSB is shifted to LSB and assigned to signFlag*/
	signFlag = (alu & MSB32MASK) >> MSBTOLSB;

}

/**********************************************************
  iscarry()- determine if carry is generated by addition: op1+op2+C
  C can only have value of 1 or 0.
  ***********************************************************/
int iscarry(unsigned long op1, unsigned long op2, unsigned long C)
{
	if ((op2 == MAX32) && (C == 1))
		return (1); // special case where op2 is at MAX32
	return ((op1 > (MAX32 - op2 - C)) ? 1 : 0);
}

/* Function that help implement the load instruction*/
int loadReg(int marVal, void *memory)
{
	int i;
	mar = marVal;

	for (i = 0; i < CYCLES; i++, mar++)
	{
		mbr = mbr << SHIFT_BYTE;
		mbr += *((unsigned char*)memory + mar);
	}

	return mbr;
}

/* Function fetchs the inst from memory*/
void fetch(void * memory){

	ir = loadReg(PC, memory);
	/* PC + 1 instruction */
	PC += REG_SIZE;
}
 

/* Function that implements the execute tage of the CPU*/
void execute(void *memory)
{
	int i;

	if (LOAD_STORE)
	{

		if (LOAD_BIT){
			/*Load Byte*/
			if (BYTE_BIT){
				regfile[RD] = loadReg(regfile[RN], memory);
				regfile[RD] = regfile[RD] & BYTE_MASK;
			}
			/*Load DWORD*/
			else{
				regfile[RD] = loadReg(regfile[RN], memory);
			}
		}
		else{
			mbr = regfile[RD];
			/*Store one byte of reg into memory*/
			if (BYTE_BIT){
				mar = regfile[RN];
				mbr = regfile[RD];
				*((unsigned char*)memory + mar) = (unsigned char)mbr & BYTE_MASK;
			}
			/*Store DWORD*/
			else{

				for (i = CYCLES - 1; i >= 0; i--) {
					*((unsigned char*)memory + mar++) = (unsigned char)(mbr >> SHIFT_BYTE * i & BYTE_MASK);
				}
			}
		}
	}
	/* Branch inst.*/
	else if (BRANCH)
	{
		/* Jumps to a specified location determined form the inst.*/
		if (LINK_BIT){
			// set the return address in the link reg.
			LR = PC;
		}
		// set PC to offset determined from the inst.
		PC = OFFSET12;
		flag_ir = 0;
	}
	/* Cond. Branch inst.*/
	else if (COND_BRANCH)
	{
		// check condition code
		if (checkbran()){
			// setting alu to the result of PC + FF
			alu = PC + (int)COND_ADDR;
			// if IR1 !=0 pc will be pc + alu - 3 + 1 
			if (flag_ir != 0){

				flag_ir = 0;
				alu = alu + ~THUMB_SIZE + 1;
			}
			// set PC to result of alu
			PC = alu;
		}
	}
	/* Data processing*/
	else if (DATA_PROC)
	{
		/* ADD inst.*/
		if (DATA_ADD){
			alu = regfile[RD] + regfile[RN];
			flags(alu);
			flag_carry = iscarry(regfile[RD], ~regfile[RN], 0);
			regfile[RD] = alu;
		}
		/* SUB inst.*/
		else if (DATA_SUB){
			alu = regfile[RD] + ~regfile[RN] + 1;
			flags(alu);
			flag_carry = iscarry(regfile[RD], ~regfile[RN], 1);
			regfile[RD] = alu;
		}
	}
	else if (STOP)
	{
		stopFlag = 1; // Setting the stop flag
	}
}

/* Function which determines the condition code suffixes for Cond. Branching*/
int checkbran(){
	// Equal is based on Zero flag = 1
	if (EQ){
		if (zeroFlag){
			return 1;
		}
	}
	// Not equal is based on Zero flag = 0
	else if (NE){

		if (zeroFlag == 0){
			return 1;
		}
	}
	// Higher or same based on carry = 1
	else if (CS){

		if (flag_carry){
			return 1;
		}
	}
	// Lower based on carry = 0
	else if (CC){

		if (!flag_carry){
			return 1;
		}
	}
	// Negative based on sign = 1
	else if (MI){

		if (signFlag){
			return 1;
		}
	}
	// Positive based on sign = 0
	else if (PL){

		if (!signFlag){
			return 1;
		}
	}
	// Higher based on carry = 1 and zero = 0
	else if (HI){

		if (flag_carry && zeroFlag == 0){
			return 1;
		}
	}
	// Lower based on carry = 0 or zero = 1
	else if (LS){

		if (flag_carry == 0 || zeroFlag){
			return 1;
		}
	}
	// Alaways is the default when no suffix specified 
	else if (AL){

		return 1;
	}

	return 0;
}

/* Function that fetchs the inst. and executes it*/
void instCycle(void * memory){
	/* Determine which IR to use via IR Active flag */
	if (flag_ir == 0){
		flag_ir = 1;
		/* Fetch new set of instructions */
		fetch(memory);
		/* Current instruction is now IR0 */
		cir = IR0;
		execute(memory);
	}
	else{
		flag_ir = 0;
		cir = IR1;
		execute(memory);
	}
}

/* Function to display the registers, flags and non-visible registers*/
int dumpReg(){
	unsigned int i;

	/* Print regsiter file */
	for (i = 0; i < RF_SIZE; i++){
		if (i % LINE_BREAK == 0){
			printf("\n");
		}
		if (i == RF_SP){
			printf(" SP:%08X ", SP);
		}
		else if (i == RF_LR){
			printf(" LR:%08X ", LR);
		}
		else if (i == RF_PC){
			printf(" PC:%08X ", PC);
		}
		else{
			printf("r%02d:%08X ", i, regfile[i]);
		}
	}

	/* Print flags */
	printf("\t SZC:%d%d%d", signFlag, zeroFlag, flag_carry);

	/* Print non-visible registers */
	printf("\n   MAR:%08X   MBR:%08X   IR0:%04X   IR1:%04X   Stop:%0d   IR Flag:%01d\n", mar, mbr, IR0, IR1, stopFlag, flag_ir);

	return 0;
}

/* Function to reset all the registers, flags and non-visible registers*/
int reset(){
     int i;
    // Reset visible registers
    for(i = 0; i < RF_SIZE; i++){
        regfile[i] = 0;
    }
    // Reset flags
    signFlag = 0;
    zeroFlag = 0;
    flag_carry = 0;
    stopFlag = 0;
    flag_ir = 0;
    // Non-visible registers
    mar = 0;
    mbr = 0;
    ir = 0;
    cir = 0;
    
    return 0;
}

// the menu
void menu()
{
	printf("d:\tdump memory\n");
	printf("g:\tgo - run the entire program\n");
	printf("l:\tload a file into memory\n");
	printf("m:\tmemory modify\n");
	printf("q:\tquit\n");
	printf("r:\tdisplay registers\n");
	printf("t:\ttrace - execute one instruction\n");
	printf("w:\twrite file\n");
	printf("z:\treset all registers to zero\n");
	printf("?,h:\tdisplay list of commands\n");
}

int main()
{
	char option, buff[MEMSIZE];
	char *p, *pw;
	unsigned ofs, strt, len;
	// Just to have a clean view of the menu!
	system("clear");
	// printing the menu
	reset(); /* reseting all the regs*/
	printf("\t\t--Jan Yalda--\n");
	menu();
	while (1){
		printf("Select an option: ");
		//  prompt user for an option
		scanf(" %c", &option);
		switch (option)
		{
		case 'd':
		case 'D':
			// prompt user for offset
			printf("Enter the Offset: ");
			scanf("%x", &ofs);//0x000
			// prompt user for length
			printf("\nEnter the Length: ");
			scanf("%x", &len);//48
			// call the function passing the arguments
			Memdump(&buff, ofs, len);
			continue;
		case 'g':
		case 'G':
				// run entire program until stop inst.
			 	while(!stopFlag){
                    instCycle(buff);
                 }

			continue;
		case 'l':
		case 'L':
			// make the pointer point to the memory 
			p = buff;
			// calling the function passing its arguments 
			int i = LoadFile(p, sizeof(buff));
			// check if returned value is zero then error ocurred while opening file 
			if (i == 0){}
			else{
				// display the bytes read in decimal and hex
				printf("\nNumber of bytes read: %d Bytes \t Hex: %x\n", i, i);
				//buff[1024]='\0';
				//printf("\nbuff conent: %s\n",buff);
			}
			continue;
		case 'm':
		case 'M':
			// prompt for a starting address
			printf("Enter a starting address: ");
			scanf("%x", &strt);
			// call the function passing the arguments
			MemMod(&buff, strt);
			continue;
		case 'q':
		case 'Q':
			return;
		case 'r':
		case 'R':
				dumpReg();
			continue;
		case 't':
		case 'T':
			// run an inst. then display registers
			instCycle(buff);
			dumpReg();
			continue;
		case 'w':
		case 'W':
			// check if memory has any content
			if (buff[0] == '\0')
			{
				// if nothing found inform the user
				printf("\nNothing has been loaded into memory.\n");
			}
			else{
				// make the pointer point to the memory content
				pw = buff;
				// call the function
				WriteFile(pw);
			}
			continue;
		case 'z':
		case 'Z':
			    reset();
                printf("Registers have been reseted\n");
			continue;
		case '?':
		case 'h':
			menu();
			continue;
		default:
			printf("\nInvalid Selection...\n");

		}
	}


}
