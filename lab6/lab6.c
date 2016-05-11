/*
 * Chad Benson
 * Nghia Nguyen
 *
 * 315
 * lab6 - mips multi cycle simulator
 * code reused from lab 4 and lab 5
 * https://codeshare.io/hCJiL
 */

/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mips_asm_header.h"

/* Constants */

#define BASE   0x000000000
#define EMPTY 0 
#define FULL 1
#define BUSY 2 

/* Structures */
typedef unsigned int MIPS, *MIPS_PTR;
MB_HDR mb_hdr;		/* Header area */
MIPS mem[4096];	/* Memory space */
char* table[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4",
   "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9",
   "$k0", "$k1", "$gp","$sp", "$fp", "$ra"};

/* Core Mips Multi Cycle Structure */
typedef struct mipsSim
{
   unsigned int regs[32]; /* sim mips registers */
   unsigned int pcValue;   /* program counter */
   int *memp;   /* instruction */
   int numOfInstr;   /* number of instructions in program */
   int numOfRWs;  /* number of read and writes in the program */
   int numClock; /* number of clock cycles */

}  MIPS_SIM;
/* Struct basket for stages fetch and decode */
typedef struct fdBasket 
{
   /* out basket */
   int ir;  /* instruction register */
} FDB;

/* basket for stages decode and execute */
typedef struct dexeBasket 
{
   /* out basket */
   int aReg; /* rs register */
   int bReg; /* rt register */
   int aluOut; /* result from alu computation */


   /* in basket */

} DEB ;

/* basket for stages execute and memory */
typedef struct exeMemBasket 
{

} EMB;

/* basket for stages memory and write back*/
typedef struct memWrbBasket 
{

   /* out basket */
   int mdr; /* memory data register */
} MWB;

/* Function Declarations */
/* Functs for decoding instructions */
int getFunct(int n);
int getrs(int n);
int getrt(int n);
int getrd(int n);
int getshift(int n);
int getimm8(int n);
int getimm16(int n);
//int geteff(int n, int i);
int geteff(MIPS_SIM* sim, FDB* fdBasket);
void decodeR(int n, int opc, int funct, int i, MIPS_SIM* sim);
void decodeIJ(int n, int opc,  int i, MIPS_SIM* sim);
/* Functs for running simulator */
void simDefault(MIPS_SIM* sim);
int prompt();
void initProgram();
int getPC();
void getInstr(int pc, MIPS_SIM* sim);
void printRegs(MIPS_SIM* sim);
/* Functs for 5 multi cycle stages */
void wrb();  /* write back */  
void mwr(); /* memory write */
void exe(); /* execute */
void d(MIPS_SIM* sim, FDB* fdBasket, DEB* dexeBasket); /* instruction decode */
void f(MIPS_SIM* sim, FDB* fdBasket);  /* instruction fetch */





/*************************************** MAIN ******************************/
int main()
{

   MIPS_SIM* sim = malloc(sizeof(MIPS_SIM)); /* init sim structure */
   simDefault(sim); /* set default values */
   FDB* fdBasket = malloc(sizeof(FDB));
   DEB* dexeBasket = malloc(sizeof(DEB));
   EMB* exeMemBasket = malloc(sizeof(EMB));
   MWB* memWrtBacBasket = malloc(sizeof(MWB));
   initProgram(); /* prompt user for file and read in mips program to buffer */

   /* Multi Cycle Loop */
   do
   {

      wrb();  /* write back */  
      mwr(); /* memory write */
      exe(); /* execute */
      if(sim->regs[2] == 10 && mem[(sim->pcValue-4)/4] == 0xC) /* program halts */
         break;
      d(sim, fdBasket, dexeBasket); /* instruction decode */
      f(sim, fdBasket);  /* instruction fetch */
      sim->numClock++;

      //      if(run == 2)
      //         run = prompt();   /* prompt user for command */
      //      if(run == 0)
      //        break;
      //      printf("\n PC: %.8x  - 0x%.8x  - ", sim->pcValue, mem[sim->pcValue/4]);
      // get instr
      //      getInstr(sim->pcValue, sim);
      // print regs
      //      printRegs(sim);
      // print #instruct, readn writes
      //     sim->numOfInstr++;

   } while(1);  /* run until user quits or $v0 is 10 */

   printf("\n Total instunctions: %d\n", sim->numOfInstr);
   printf(" Total read-write instructions: %d\n", sim->numOfRWs);
   printf(" Total clock cycles: %d\n\n", sim->numClock);

   free(sim);
   free(fdBasket);
   free(dexeBasket);
   free(exeMemBasket);
   free(memWrtBacBasket);
   return 0;

}  /* End Main */

/* Functs for 5 multi cycle stages */
void wrb(MIPS_SIM* sim)  /* write back */  
{

}
void mwr(MIPS_SIM* sim) /* memory write */
{

}
void exe(MIPS_SIM* sim) /* execute */
{

}
void d(MIPS_SIM* sim, FDB* fdBasket, DEB* dexeBasket) /* instruction decode */
{
   // need flags!
   dexeBasket->aReg = sim->regs[getrs(fdBasket->ir)]; /* load source A */
   dexeBasket->bReg = sim->regs[getrt(fdBasket->ir)]; /* load source B */
   dexeBasket->aluOut = geteff(sim, fdBasket->ir);    /* get effective address */   






}
void f(MIPS_SIM* sim, FDB* fdBasket)  /* instruction fetch */
{
   // flags ?
   fdBasket->ir = mem[sim->pcValue/4];   /* load instruction into ir */   
   sim->numOfInstr++;
   sim->pcValue += 4;   /* increment pc */

}

/* Funct sets simulator to default values */
void simDefault(MIPS_SIM* sim)
{
   int i;
   for(i=0; i<32; i++) 
      sim->regs[i] = 0;
   sim->pcValue = BASE; 
   sim->numClock = 0;
   sim->numOfInstr= 0;
   sim->numOfRWs = 0;

}







/* Function prompt */
int prompt()
{
   int val;
   printf(" \n ******* Welcome to our 315 MIPS simulator: *******\n"
         " enter 1 to run program \n"
         " or enter 2 to step through program \n"
         " or enter 0 to exit ");

   scanf("%d", &val);
   return val;
}

/* Function printRegs - prints all 32 registers */
void printRegs(MIPS_SIM* sim)
{
   int i, j;
   for (i=0; i<4; i++)
   {
      for (j=0; j<8; j++)
      {
         printf(" %-9s", table[j + 8*i]);
      }
      printf("\n");
      for(j=0; j<8; j++)
      {
         printf(" %.8x ", sim->regs[j + 8*i]);
      }
      printf("\n");
   }
}

/* Function getPC - returns pc value */
int getPC(MIPS_SIM* sim)
{
   return sim->pcValue;
}

/* Function getInstr */
void getInstr(int i, MIPS_SIM* sim)
{
   int opc, funct;

   if(mem[i/4] == 0)
   {
      // todo print for nop *****************************************
      printf("Instruction: nop \n"
            " Number of cycles: 2 \n");
      sim->pcValue += 4;
      sim->numClock +=4;
      return;
   }

   // printf("Instruction@%08X : %08X\n", i, mem[i/4]); /* print instruction */
   opc = mem[i/4] >> 26;   /* get opcode */
   if(opc == 0)            /* check if R type instr */
   {
      funct = getFunct(mem[i/4]);
      decodeR(mem[i/4], opc, funct, i, sim); /* decode R type instruction */
   }
   else
      decodeIJ(mem[i/4], opc, i, sim);   /* Decode I or J type instr */
}

/* Funct initProgram prompts user for file and reads into a buffer */
void initProgram()
{
   /* Function load program
    * Prompts and reads mips program
    * from user given filename
    */

   /* Vars */
   FILE *fd;               /* points to a file */
   int n;                  /* for reading instructions */
   int memp;
   char filename[20];      /* for file name */
   /************************************** I / O ******************************/

   printf("Enter a mips file to decode: "); /* prompt user for a file */
   scanf("%s", filename);
   /* format the MIPS Binary header */

   fd = fopen(filename, "rb");
   /* fd = fopen("testcase1.mb", "rb"); */
   if (fd == NULL) { printf("\nCouldn't load test case - quitting.\n"); exit(99); }

   memp = 0;		/* This is the memory pointer, a byte offset */

   /* read the header and verify it. */
   fread((void *) &mb_hdr, sizeof(mb_hdr), 1, fd);
   if (! strcmp(mb_hdr.signature, "~MB") == 0)
   { printf("\nThis isn't really a mips_asm binary file - quitting.\n"); exit(98); }

   printf("\n%s Loaded ok, program size=%d bytes.\n\n", filename, mb_hdr.size);
   /* read the binary code a word at a time */

   do {
      n = fread((void *) &mem[memp/4], 4, 1, fd); /* note div/4 to make word index */
      if (n)
         memp += 4;	/* Increment byte pointer by size of instruction */
      else
         break;
   } while (memp < sizeof(mem)) ;

   fclose(fd);
   return;
}

/* Function getFunct
 * returns function value for R type instru
 * Assume integer instruction
 */
int getFunct(int n)
{
   n &= 0x3F;
   return n;
}

/* Function DecodeR
 * DecodeR decodes and prints R type instruction
 * Assumes integer instruction, opcode, function value and PC
 */
void decodeR(int n, int opc, int funct, int i, MIPS_SIM* sim) // add struct ptr*
{
   if (funct == 0xC)
   {
      printf(" Syscall\n");
      sim->pcValue += 4;
   }
   else if(funct == 0x20)    /* add */
   {
      printf(" add %s, %s, %s \n",
            table[getrd(n)], table[getrt(n)], table[getrs(n)]);
      sim->regs[getrd(n)] = sim->regs[getrt(n)] + sim->regs[getrs(n)];
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x21)    /* addu */
   {
      printf(" addu %s, %s, %s \n",
            table[getrd(n)], table[getrt(n)], table[getrs(n)]);
      sim->regs[getrd(n)] = sim->regs[getrs(n)] + sim->regs[getrt(n)];
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x22)  /* subtract */
   {
      printf(" subtract %s, %s, %s \n",
            table[getrd(n)], table[getrs(n)], table[getrt(n)]);
      sim->regs[getrd(n)] = sim->regs[getrs(n)] - sim->regs[getrt(n)];
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x23) /* subtract unsigned */
   {
      printf(" subtract unsigned %s, %s, %s \n",
            table[getrd(n)], table[getrs(n)], table[getrt(n)]);
      sim->regs[getrd(n)] = sim->regs[getrs(n)] - sim->regs[getrt(n)];
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x24) /* and */
   {
      printf(" and %s %s, %s \n",
            table[getrd(n)], table[getrs(n)], table[getrt(n)]);
      sim->regs[getrd(n)] = sim->regs[getrs(n)] & sim->regs[getrt(n)];
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x27) /* nor */
   {
      printf(" nor %s, %s, %s \n",
            table[getrd(n)], table[getrs(n)], table[getrt(n)]);
      sim->regs[getrd(n)] = ~(sim->regs[getrs(n)] | sim->regs[getrt(n)]);
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x25) /* or */
   {
      printf(" or %s, %s, %s \n",
            table[getrd(n)], table[getrs(n)], table[getrt(n)]);
      sim->regs[getrd(n)] = (sim->regs[getrs(n)] | sim->regs[getrt(n)]);
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x26) /* xor */
   {
      printf(" xor %s, %s,%s \n",
            table[getrd(n)], table[getrs(n)], table[getrt(n)]);
      sim->regs[getrd(n)] = (sim->regs[getrs(n)] ^ sim->regs[getrt(n)]);
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x00) /* sll */
   {
      printf(" sll %s, %s, %d \n",
            table[getrd(n)], table[getrt(n)], getshift(n));
      sim->regs[getrd(n)] = sim->regs[getrt(n)] << getshift(n);
      printf(" Number of cycles: 4 \n");
      sim->pcValue += 4;
      sim->numClock += 4;
   }
   else if(funct == 0x02) /* srl */
   {
      printf(" srl %s, %s, %d \n",
            table[getrd(n)], table[getrt(n)], getshift(n));
      sim->regs[getrd(n)] = (sim->regs[getrt(n)] >> getshift(n));
      sim->pcValue += 4;
      sim->numClock += 4;
      printf(" Number of cycles: 4 \n");
   }
   else if(funct == 0x03) /* sra */
   {
      int temp = 0x80000000, i=0;
      printf(" sra %s, %s, %d \n",
            table[getrd(n)], table[getrt(n)], getshift(n));
      sim->regs[getrd(n)] = sim->regs[getrt(n)];
      if ((sim->regs[getrt(n)] & temp) != 0)
      {
         while (i < getshift(n))
         {
            sim->regs[getrd(n)] >>= 1;
            sim->regs[getrd(n)] += temp;
            i++;
         }
      }
      else
         sim->regs[getrd(n)] >>= getshift(n);
      sim->pcValue += 4;
      sim->numClock += 4;
      printf(" Number of cycles: 4 \n");
   }
   else if(funct == 0x04) /* sllv */
   {
      printf(" sllv %s, %s, %s \n",
            table[getrd(n)], table[getrt(n)], table[getrs(n)]);
      sim->regs[getrd(n)] = sim->regs[getrt(n)] << sim->regs[getrs(n)];
      sim->pcValue += 4;
      sim->numClock += 4;
      printf(" Number of cycles: 4 \n");
   }
   else if(funct == 0x06) /* srlv */
   {
      printf(" srlv %s, %s, %s \n",
            table[getrd(n)], table[getrt(n)], table[getrs(n)]);
      sim->regs[getrd(n)] = (sim->regs[getrt(n)] >> sim->regs[getrs(n)]);
      sim->pcValue += 4;
      sim->numClock += 4;
      printf(" Number of cycles: 4 \n");
   }
   else if(funct == 0x07) /* srav */
   {
      int temp = 0x80000000, i=0;
      printf(" srav %s, %s, %s \n",
            table[getrd(n)], table[getrt(n)], table[getrs(n)]);
      sim->regs[getrd(n)] = sim->regs[getrt(n)];
      if ((sim->regs[getrt(n)] & temp) != 0)
      {
         while (i < sim->regs[getrs(n)])
         {
            sim->regs[getrd(n)] >>= 1;
            sim->regs[getrd(n)] += temp;
            i++;
         }
      }
      else
         sim->regs[getrd(n)] >>= sim->regs[getrs(n)];
      sim->pcValue += 4;
      sim->numClock += 4;
      printf(" Number of cycles: 4 \n");
   }
   else if(funct == 0x2A) /* slt */
   {
      sim->pcValue += 4;
      sim->numClock += 4;
      printf(" slt %s, %s, %s \n",
            table[getrd(n)], table[getrs(n)], table[getrt(n)]);
      if(sim->regs[getrs(n)] < sim->regs[getrt(n)])
         sim->regs[getrd(n)] = 1;
      else
         sim->regs[getrd(n)] = 0;
      printf(" Number of cycles: 4 \n");
   }
   else if(funct == 0x2B) /* sltu */
   {
      sim->pcValue += 4;
      sim->numClock += 4;
      printf(" sltu %s, %s, %s \n",
            table[getrd(n)], table[getrs(n)], table[getrt(n)]);
      if(sim->regs[getrs(n)] < sim->regs[getrt(n)])
         sim->regs[getrd(n)] = 1;
      else
         sim->regs[getrd(n)] = 0;
      printf(" Number of cycles: 4 \n"); 

   }
   else if(funct == 0x08) /* jump register */
   {
      printf(" jr %s \n", table[getrs(n)]);
      sim->pcValue = sim->regs[getrs(n)];
      sim->numClock += 3;
      printf(" Number of cycles: 3 \n");
   }
   else if(funct == 0x09) /* jalr */
   {
      sim->pcValue += 4; 
      sim->numClock += 3;
      printf(" jalr %s \n", table[getrs(n)]);
      sim->regs[31] = sim->pcValue; 
      sim->pcValue = sim->regs[getrs(n)];
      printf(" Number of cycles: 3 \n");
   }
   else
   {
      printf("Instruction@%08X : %08X is not a valid instruction! \n", i, n);
   }
}

/* Function DecodeIJ
 * Decodes and prints I or J type if valid
 */
void decodeIJ(int n, int opc, int i, MIPS_SIM* sim)
{
   if(opc == 0x02 || opc == 0x03)    /* j or jal  */
   {
      //int temp = n;
      //n &= 0xF0000000; /* save PC */
      //temp &= 0x0FFFFFFF; /* obtain 26 address*/
      //temp <<= 2;             /* multiply by 4 */
      //n += temp;              /* effective address */
      n &= 0x03FFFFFF;
      n <<= 2;
      if(opc == 0x02)
      {
         printf(" j %s, %s, 0x%.8x \n",
               table[getrt(n)], table[getrs(n)], n);
         sim->pcValue = n;
         printf(" Number of cycles: 3 \n");
         sim->numClock += 3;
      }
      else
      {
         printf(" jal %s, %s, 0x%.8x \n",
               table[getrt(n)], table[getrs(n)], n);
         sim->regs[31] = sim->pcValue + 4;
         sim->pcValue = n;
         printf(" Number of cycles: 3 \n");
         sim->numClock += 3;
      }
   }
   else if(opc == 0x08)    /* addi */
   {
      printf(" addi %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      sim->regs[getrt(n)] = sim->regs[getrs(n)] + getimm16(n);
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numClock += 4;
   }
   else if(opc == 0x09)    /* addiu */
   {
      printf(" addiu %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      sim->regs[getrt(n)] = sim->regs[getrs(n)] + getimm16(n);
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numClock += 4;
   }
   else if(opc == 0x0C)    /* andi */
   {
      printf(" andi %s, %s, 0x%.8x \n ",
            table[getrt(n)], table[getrs(n)], n & 0X0000FFFF);
      sim->regs[getrt(n)] = sim->regs[getrs(n)] & (n & 0x0000FFFF);
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numClock += 4;
   }
   else if(opc == 0x0D)    /* ori */
   {
      printf(" ori %s, %s 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], n & 0X0000FFFF);
      sim->regs[getrt(n)] = sim->regs[getrs(n)] | (n & 0x0000FFFF);
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numClock += 4;
   }
   else if(opc == 0x0E)    /* xori */
   {
      printf(" xori %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], n & 0X0000FFFF);
      sim->regs[getrt(n)] = sim->regs[getrs(n)] ^ (n & 0x0000FFFF);
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numClock += 4;
   }
   else if(opc == 0x0A)    /* slti */
   {
      printf(" slti %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      sim->regs[getrt(n)] = sim->regs[getrs(n)] < (n & 0x0000FFFF) ? 1 : 0;
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numClock += 4;
   }
   else if(opc == 0x0B)    /* sltiu */
   {
      printf(" sltiu %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      sim->regs[getrt(n)] = sim->regs[getrs(n)] < (n & 0x0000FFFF) ? 1 : 0;
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numClock += 4;
   }
   else if(opc == 0x04)    /* beq */
   {
      printf(" beq %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], geteff(n, i));
      if (sim->regs[getrt(n)] == sim->regs[getrs(n)])
         sim->pcValue = geteff(n,i);
      else
         sim->pcValue += 4 ;
      printf(" Number of cycles: 3 \n");
      sim->numClock += 3;
   }
   else if(opc == 0x05)    /* bne */
   {
      printf(" bne %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], geteff(n, i));
      if (sim->regs[getrt(n)] != sim->regs[getrs(n)])
         sim->pcValue = geteff(n,i);
      else
         sim->pcValue += 4;
      printf(" Number of cycles: 3 \n");
      sim->numClock += 3;
   }
   else if(opc == 0x20)    /* lb */
   {
      printf(" lb, %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], getimm8(n));
      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + getimm8(n))/4];
      if ((sim->regs[getrt(n)] & 0x00000080) != 0)
         sim->regs[getrt(n)] |= 0xFFFFFF00;
      sim->pcValue +=4;
      printf(" Number of cycles: 5 \n");
      sim->numOfRWs ++;
      sim->numClock += 5;
   }
   else if(opc == 0x24)    /* lbu */
   {
      printf(" lbu %s, %s, 1x%.8x \n",
            table[getrt(n)], table[getrs(n)], n & 0x000000FF);
      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + (n & 0x000000FF))/4];
      sim->regs[getrt(n)] &= 0x000000FF;
      sim->pcValue += 4;
      printf(" Number of cycles: 5 \n");
      sim->numOfRWs ++;
      sim->numClock += 5;
   }
   else if(opc == 0x21)    /* lh */
   {
      printf(" lh %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + getimm16(n))/4];
      sim->regs[getrt(n)] <<= 16;
      sim->regs[getrt(n)] >>= 16;
      sim->pcValue += 4;
      printf(" Number of cycles: 5 \n");
      sim->numOfRWs ++;
      sim->numClock += 5;
   }
   else if(opc == 0x21)    /* lhu */
   {
      printf(" lhu %s, %s, 0x%.8x \n",
            table[getrt(n)], table[getrs(n)], n & 0x0000FFFF);
      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + (n & 0x0000FFFF))/4];
      sim->regs[getrt(n)] &= 0x0000FFFF;
      sim->pcValue += 4;
      printf(" Number of cycles: 5 \n");
      sim->numOfRWs ++;
      sim->numClock += 5;
   }
   else if(opc == 0x0F)    /* lui */
   {
      printf(" lui %s, 0x%.8x \n",
            table[getrt(n)], getimm16(n));
      sim->regs[getrt(n)] = n << 16;
      sim->pcValue += 4;
      printf(" Number of cycles: 5 \n");
      sim->numOfRWs ++;
      sim->numClock += 5;
   }
   else if(opc == 0x23)    /* lw */
   {
      printf(" lw %s, %s,0x%.8x \n",
            table[getrs(n)], table[getrt(n)], getimm16(n));
      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + getimm16(n))/4];
      sim->pcValue += 4;
      printf(" Number of cycles: 5 \n");
      sim->numOfRWs ++;
      sim->numClock += 5;
   }
   else if(opc == 0x28)    /* sb */
   {
      printf(" sb %s, 0x%.8x (%s) \n",
            table[getrt(n)], getimm8(n), table[getrs(n)]);
      mem[(getimm8(n) + sim->regs[getrs(n)])/4] = sim->regs[getrt(n)];
      mem[(getimm8(n) + sim->regs[getrs(n)])/4]&= 0x000000FF;
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numOfRWs ++;
      sim->numClock += 4;
   }
   else if(opc == 0x29)    /* sh */
   {
      printf(" lw %s, 0x%.8x (%s)\n",
            table[getrt(n)], getimm16(n), table[getrs(n)]);
      mem[(getimm16(n) + sim->regs[getrs(n)])/4] = sim->regs[getrt(n)];
      mem[(getimm16(n) + sim->regs[getrs(n)])/4] &= 0x0000FFFF;
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numOfRWs ++;
      sim->numClock += 4;
   }
   else if(opc == 0x2B)    /* sw */
   {
      printf(" lw %s, 0x%.8x (%s) \n",
            table[getrt(n)], getimm16(n), table[getrs(n)]);
      mem[(getimm16(n) + sim->regs[getrs(n)])/4] = sim->regs[getrt(n)];
      sim->pcValue += 4;
      printf(" Number of cycles: 4 \n");
      sim->numOfRWs ++;
      sim->numClock += 4;
   }
   else
   {
      printf("Instruction@%08X : %08X is not a valid instruction! \n \n", i, n);
      sim->pcValue += 4;
   }
}
/* Function getimm8 - get immediate byte sign ext*/
int getimm8(int n)
{
   int extBit, temp;
   extBit = n & 0x00000080;
   extBit >>= 7;
   if(extBit) /* sign extend if 7th bit is set */
   {
      temp = n & 0x000000FF;
      return temp += 0xFFFFFF00;
   } else   /* zero sign extend */
   {
      return n &= 0x000000FF;
   }
}
/* Function getimm16 - get immediate halfword sign ext */
int getimm16(int n)
{
   int extBit, temp;
   extBit = n & 0x00008000;
   extBit >>= 15;
   if(extBit) /* sign extend if bit is set */
   {
      temp = n & 0x0000FFFF;
      return temp += 0xFFFF0000;
   } else   /* zero sign extend */
   {
      return n &= 0x0000FFFF;
   }
}

//*Function geteff returns effective address */
//int geteff(int n, int i)
//{
//   i += 4;  /* advance PC */
//   n = getimm16(n); /* get immediate value (might be sign extended) */
//   n <<= 2;       /* word align the value */
//   return n + i;  /* add PC to Branch Address */
//}

/*Function geteff returns effective address */
int geteff(MIPS_SIM* sim, FDB* fdBasket)
{
   int n;
   n = getimm16(fdBasket->ir); /* get immediate value (might be sign extended) */
   n <<= 2;       /* word align the value */
   return n + sim->pcValue;  /* add PC to Branch Address */
}


/* Functions get registers and get shift */
int getrs(int n)
{
   n &= 0x03E00000;  /* return $rs register */
   return n >> 21;
}

int getrt(int n)
{
   n &= 0x001F0000;  /* return $rt register */
   return n >> 16;
}

int getrd(int n)
{
   n &= 0x0000F800;  /* return $rd register */
   return n >> 11;
}
int getshift(int n)
{
   n &= 0x000007C0; /* return shift amount */
   return n >> 6;
}
