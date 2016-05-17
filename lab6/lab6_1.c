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
   /* flag */
   int dBusy;
   /* fetch out basket */
   int ir;  /* instruction register */
} FDB;



/* basket for stages decode and execute */
typedef struct deBasket 
{

   int eBusy;
   int ir;
   int aReg;
   int bReg; 
   int aluOut; 

} DEB;

/* basket for stages execute and memory */
typedef struct emBasket 
{
   int mBusy;
   int ir;
   int bReg; 
   int aluOut; 

} EMB; 

/* basket for stages memory and write back*/
typedef struct mwBasket 
{
   int wBusy;
   /* Mem in basket */
   int ir;

   /* Mem out basket - WB in basket */
   int type; /* flage for either R, I, or LW type */
   int mdr; /* memory data register */
   int aluOut;

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
int geteff(MIPS_SIM* sim, DB* dBasket);
void procR(SIM* sim, DEB* deBasket, MWB* mwBasket);
void procI(MIPS_SIM* sim, DEB* deBasket, MWB* mwBbasket);
/* Functs for running simulator */
void simDefault(MIPS_SIM* sim);
int prompt();
void initProgram();
int getPC();
int getInstr(MIPS_SIM* sim, DEB* deBasket);
void printRegs(MIPS_SIM* sim);
/* Functs for 5 multi cycle stages */
void wb(MIPS_SIM* sim, DEB* deBasket, EMB* emBasket, MWB* mwBasket);  /* write back */  
void mem(MIPS_SIM* sim, DEB* deBasket, EMB* emBasket, MWB* mwBasket); /* memory write */
void exe(MIPS_SIM* sim, DEB* deBasket, EMB* emBasket, MWB* mwBasket); /* execute */
void d(MIPS_SIM* sim, FDB* fdBasket, DEB* deBasket); /* instruction decode */
void f(MIPS_SIM* sim, FDB* fdBasket);  /* instruction fetch */


/*************************************** MAIN ******************************/
int main()
{

   MIPS_SIM* sim = malloc(sizeof(MIPS_SIM)); /* init sim structure */
   simDefault(sim); /* set default values */
   FDB* fdBasket = malloc(sizeof(FDB));
   DEB* deBasket = malloc(sizeof(DEB));
   EMB* emBasket = malloc(sizeof(EMB));
   MWB* mwBasket = malloc(sizeof(MWB));
   basketDefault(fdBasket, deBasket, emBasket, mwBasket); /* default values for baskets */
   initProgram(); /* prompt user for file and read in mips program to buffer */

   /* Multi Cycle Loop */
   do
   {

      wb(sim, deBasket, mwBastket);  /* write back */  
      mem(sim, emBasket); /* memory write */
      exe(sim, deBasket, emBasket, mwBasket); /* execute */
      if(sim->regs[2] == 10 && mem[(sim->pcValue-4)/4] == 0xC) /* program halts */
         break;
      d(sim, fdBasket, deBasket); /* instruction decode */
      f(sim, fdBasket);  /* instruction fetch */
      sim->numClock++;


   } while(1);  /* run until user quits or $v0 is 10 */

   printf("\n Total instunctions: %d\n", sim->numOfInstr);
   printf(" Total read-write instructions: %d\n", sim->numOfRWs);
   printf(" Total clock cycles: %d\n\n", sim->numClock);

   free(sim);
   free(fdBasket);
   free(deBasket);
   free(emBasket);
   free(mwBasket);
   return 0;

}  /* End Main */

/* Functs for 5 multi cycle stages */

void wb(MIPS_SIM* sim, DEB* deBasket, MWB* mwBasket)  /* write back */  
{

   /* Check in Basket */
   if(mwBasket->mdr != 0 || mwBasket->aluOut != 0)
   {

      switch (mwBasket->type)
      {
         case 0: /* R type */
            sim->regs[getrd(mwBasket->ir)] = mwBasket->aluOut;
            mwBasket->wBusy = 0;
            mwBasket->aluOut = 0; 


            break;

         case 1: /* I type */

            sim->regs[getrt(mwBasket->ir)] = mwBasket->aluOut;
            mwBasket->wBusy = 0;
            mwBasket->aluOut = 0; 



            break;

         case 2: /* Load Word */

            sim->regs[getrt(mwBasket->ir)] = mwBasket->mdr;
            mwBasket->wBusy = 0;
            mwBasket->mdr = 0; 



            break;
      }
      else
         return;
   }    

   void mem(MIPS_SIM* sim, EMB* emBasket, MWB* mwBasket) /* memory write */
   {
     mem[(getimm8(n) + sim->regs[getrs(n)])/4] = sim->regs[getrt(n)];
               mem[(getimm8(n) + sim->regs[getrs(n)])/4]&= 0x000000FF;



   }


   void exe(MIPS_SIM* sim, DEB* deBasket, EMB* emBasket, MWB* mwBasket) 
   {
      // four cases: J and Branch (0), R type (1), store word (2), load word (3), I type (4)

      if(deBasket->ir != 0)
      {

      
      int value;
      value = getInstr(sim, deBasket);
      switch (value)
      {
         int opc = deBasket->ir >> 26;
         // Jump or Branch Type
         case 0:

         if(opc == 0x02) /* Jump */
         {  

         }
         else if(opc == 0x03) /* Jump and Link */
         {

         }
         else if(opc == 0x04) /* Branch on Equal */
         {

         }
         else  /* Branch not equal */
         {

         }


         //      if(opc == 0x02)
         //      {
         //         printf(" j %s, %s, 0x%.8x \n",
         //               table[getrt(n)], table[getrs(n)], n);
         //         sim->pcValue = n;
         //         printf(" Number of cycles: 3 \n");
         //      }
         //      else
         //      {
         //         printf(" jal %s, %s, 0x%.8x \n",
         //               table[getrt(n)], table[getrs(n)], n);
         //         sim->regs[31] = sim->pcValue + 4;
         //         sim->pcValue = n;
         //         printf(" Number of cycles: 3 \n");
         //         sim->numClock += 3;
         //      }
         //
         //
         //else if(opc == 0x04)    /* beq */
         //   {
         //      printf(" beq %s, %s, 0x%.8x \n",
         //            table[getrt(n)], table[getrs(n)], geteff(n, i));
         //      if (sim->regs[getrt(n)] == sim->regs[getrs(n)])
         //         sim->pcValue = geteff(n,i);
         //      else
         //         printf(" Number of cycles: 3 \n");
         //   }
         //   else if(opc == 0x05)    /* bne */
         //   {
         //      printf(" bne %s, %s, 0x%.8x \n",
         //            table[getrt(n)], table[getrs(n)], geteff(n, i));
         //      if (sim->regs[getrt(n)] != sim->regs[getrs(n)])
         //         sim->pcValue = geteff(n,i);
         //      else
         //         printf(" Number of cycles: 3 \n");
         //   }

         break;

         // Process R Type Instuctrion
         case 1:
         if(mwBasket->wBusy)
            return;
         else
         {
            procR(sim, deBasket, mwBasket);
         }

         break;

         // Process Mem type
         case 2:
         if(emBasket->mBusy)
            return;
         else
         {

          if(opc == 0x28)    /* sb */
            {
               mem[(getimm8(n) + sim->regs[getrs(n)])/4] = sim->regs[getrt(n)];
               mem[(getimm8(n) + sim->regs[getrs(n)])/4]&= 0x000000FF;

               emBasket->mBusy = 1;
               emBasket->ir = deBasket->ir;
               emBasket->aluOut = getimm8(n) + sim->regs[getrs()]/4
               sim->numOfRWs ++;
            }
            else if(opc == 0x29)    /* sh */
            {
               printf(" lw %s, 0x%.8x (%s)\n",
                     table[getrt(n)], getimm16(n), table[getrs(n)]);
               mem[(getimm16(n) + sim->regs[getrs(n)])/4] = sim->regs[getrt(n)];
               mem[(getimm16(n) + sim->regs[getrs(n)])/4] &= 0x0000FFFF;
               printf(" Number of cycles: 4 \n");
               sim->numOfRWs ++;
            }
            else if(opc == 0x2B)    /* sw */
            {
               printf(" lw %s, 0x%.8x (%s) \n",
                     table[getrt(n)], getimm16(n), table[getrs(n)]);
               mem[(getimm16(n) + sim->regs[getrs(n)])/4] = sim->regs[getrt(n)];
               printf(" Number of cycles: 4 \n");
               sim->numOfRWs ++;
            }
         




         }
         break;

         // Process Load type
         case 3:






         //
         //   else if(opc == 0x20)    /* lb */
         //   {
         //      printf(" lb, %s, %s, 0x%.8x \n",
         //            table[getrt(n)], table[getrs(n)], getimm8(n));
         //      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + getimm8(n))/4];
         //      if ((sim->regs[getrt(n)] & 0x00000080) != 0)
         //         sim->regs[getrt(n)] |= 0xFFFFFF00;
         //      printf(" Number of cycles: 5 \n");
         //      sim->numOfRWs ++;
         //   }
         //
         //
         //
         // else if(opc == 0x24)    /* lbu */
         //   {
         //      printf(" lbu %s, %s, 1x%.8x \n",
         //            table[getrt(n)], table[getrs(n)], n & 0x000000FF);
         //      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + (n & 0x000000FF))/4];
         //      sim->regs[getrt(n)] &= 0x000000FF;
         //      printf(" Number of cycles: 5 \n");
         //      sim->numOfRWs ++;
         //   }
         //   else if(opc == 0x21)    /* lh */
         //   {
         //      printf(" lh %s, %s, 0x%.8x \n",
         //            table[getrt(n)], table[getrs(n)], getimm16(n));
         //      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + getimm16(n))/4];
         //      sim->regs[getrt(n)] <<= 16;
         //      sim->regs[getrt(n)] >>= 16;
         //      printf(" Number of cycles: 5 \n");
         //      sim->numOfRWs ++;
         //   }
         //   else if(opc == 0x21)    /* lhu */
         //   {
         //      printf(" lhu %s, %s, 0x%.8x \n",
         //            table[getrt(n)], table[getrs(n)], n & 0x0000FFFF);
         //      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + (n & 0x0000FFFF))/4];
         //      sim->regs[getrt(n)] &= 0x0000FFFF;
         //      printf(" Number of cycles: 5 \n");
         //      sim->numOfRWs ++;
         //   }
         //   else if(opc == 0x0F)    /* lui */
         //   {
         //      printf(" lui %s, 0x%.8x \n",
         //            table[getrt(n)], getimm16(n));
         //      sim->regs[getrt(n)] = n << 16;
         //      printf(" Number of cycles: 5 \n");
         //      sim->numOfRWs ++;
         //   }
         //   else if(opc == 0x23)    /* lw */
         //   {
         //      printf(" lw %s, %s,0x%.8x \n",
         //            table[getrs(n)], table[getrt(n)], getimm16(n));
         //      sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + getimm16(n))/4];
         //      printf(" Number of cycles: 5 \n");
         //      sim->numOfRWs ++;
         //   }
         //

         break;

         // Process I Type Instuctrion
         case 4:
         if(mwBasket->wBusy)
            return;
         else
         {
            procI(sim, deBasket, mwBasket);
         }


         break;

      }
      }
   }

   void d(MIPS_SIM* sim, FDB* fdBasket, DEB* deBasket) /* instruction decode */
   {

      if(fdBasket->ir == 0) return; /* No Op - Special case */
      if(deBasket->eBusy) /* Execute if next stage is not busy */
         return;
      else
      {
         deBasket->ir = fdBasket->ir; /* pass ir */
         deBasket->aReg = sim->regs[getrs(fdBasket->ir)]; /* load source A */
         deBasket->bReg = sim->regs[getrt(fdBasket->ir)]; /* load source B */
         deBasket->aluOut = geteff(sim, fdBasket->ir);    /* get effective address */   

         fdBasket->dBusy = 1;
         fdBasket->ir = 0;

      }

   }

   void f(MIPS_SIM* sim, FDB* fdBasket)  /* instruction fetch */
   {

      if(fdBasket->dBusy)  
         return;
      else
      {
         fdBasket->ir = mem[sim->pcValue/4];   /* load instruction into ir */   
         sim->numOfInstr++;
         sim->pcValue += 4;   /* increment pc */

      } 

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

   /* Funct sets baskets to default values */
   void basketDefault(FDB* fdBasket, DEB* deBasket, EMB* emBasket,  MWB* mwBasket)
   {
      fdBasket->ir = 0;
      fdBasket->dBusy = 0;

      deBasket->eBusy = 0;
      deBasket->ir = 0;
      deBasket->aReg = 0;
      deBasket->bReg = 0;
      deBasket->aluOut = 0;

      emBasket->mBusy = 0;
      emBasket->ir = 0;
      emBasket->bReg = 0;
      emBasket->aluOut = 0;

      mwBasket->wBusy= 0;
      mwBasket->ir = 0;
      mwBasket->type= 0;
      mwBasket->mdr = 0;
      mwBasket->aluOut= 0;


   }


   /* Function getPC - returns pc value */
   int getPC(MIPS_SIM* sim)
   {
      return sim->pcValue;
   }

   /* Function getInstr */
   int getInstr(MIPS_SIM* sim, DEB* deBasket)
   {
      int opc;
      opc = deBasket->ir >> 26;   /* get opcode */

      /* check for Jump or Branch type */
      if(isJB(opc))       
         return 0;
      else if(opc == 0) /* check if R type instr */
         return 1;
      else if(isStore(opc))   /* Check if Store type */
         return 2;
      else if(isLoad(opc))    /* Check if Load type */
         return 3;
      else
         return 4;            /* I type or Invalid Instruction */
   }

   ///* Function getInstr */
   //void getInstr(MIPS_SIM* sim, DEB* deBasket)
   //{
   //   int opc, funct;
   //
   //   // printf("Instruction@%08X : %08X\n", i, mem[i/4]); /* print instruction */
   //   opc = mem[sim->pcValue/4] >> 26;   /* get opcode */
   //   if(opc == 0)            /* check if R type instr */
   //   {
   //      funct = getFunct(mem[sim->pcValue/4]);
   //      decodeR(mem[sim->pcValue/4], opc, funct, i, sim, deBasket); /* decode R type instruction */
   //   }
   //   else
   //      decodeIJ(mem[sim->pcValue/4], opc, i, sim, deBasket);   /* Decode I or J type instr */
   //}

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

   /* Checks for Jump or Branch Type */
   int isJB(int opc)
   {
      if(opc == 0x02 || opc == 0x03 || opc == 0x04 || opc == 0x05)
         return 1;
      else
         return 0;
   }

   /* Checks for Store Type */
   int isStore(int opc)
   {
      if(opc == 0x28 || opc == 0x29 || opc == 0x2B)
         return 1;
      else
         return 0;
   }

   /* Checks for Load Type */
   int isLoad(int opc)
   {
      if(opc == 0x20 || opc == 0x24 || opc == 0x21 ||
            opc == 0x25 || opc == 0x0f || opc == 0x23 )
         return 1;
      else
         return 0;
   }


   /* Function DecodeR
    * ProcR processes R type instruction
    * Assumes multi cycle simulator, and two baskets 
    */
   void procR(MIPS_SIM* sim, DEB* deBasket, MWB* mwBasket) 
   {
      int funct = getFunct(deBasket->ir); 

      if (funct == 0xC)
      {
         return; 
      }
      else if(funct == 0x20)    /* add */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut = sim->regs[getrt(n)] + sim->regs[getrs(n)];

      }
      else if(funct == 0x21)    /* addu */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut = sim->regs[getrs(n)] + sim->regs[getrt(n)];
      }
      else if(funct == 0x22)  /* subtract */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut = sim->regs[getrs(n)] - sim->regs[getrt(n)];
      }
      else if(funct == 0x23) /* subtract unsigned */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut = sim->regs[getrs(n)] - sim->regs[getrt(n)];
      }
      else if(funct == 0x24) /* and */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut = sim->regs[getrs(n)] & sim->regs[getrt(n)];
      }
      else if(funct == 0x27) /* nor */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut  = ~(sim->regs[getrs(n)] | sim->regs[getrt(n)]);
      }
      else if(funct == 0x25) /* or */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut  = (sim->regs[getrs(n)] | sim->regs[getrt(n)]);
      }
      else if(funct == 0x26) /* xor */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut  = (sim->regs[getrs(n)] ^ sim->regs[getrt(n)]);
      }
      else if(funct == 0x00) /* sll */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut = sim->regs[getrt(n)] << getshift(n);
      }
      else if(funct == 0x02) /* srl */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut  = (sim->regs[getrt(n)] >> getshift(n));
      }

      else if(funct == 0x03) /* sra */                                        /// sra will need changed 
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
         printf(" Number of cycles: 4 \n");
      }
      else if(funct == 0x04) /* sllv */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut = sim->regs[getrt(n)] << sim->regs[getrs(n)];
      }
      else if(funct == 0x06) /* srlv */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         mwBasket->aluOut  = (sim->regs[getrt(n)] >> sim->regs[getrs(n)]);
      }
      else if(funct == 0x07) /* srav */                                          /// srav will need changed
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
      }
      else if(funct == 0x2A) /* slt */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         if(sim->regs[getrs(deBasket->ir)] < sim->regs[getrt(deBasket->)])
            mwBasket->aluOut = 1;
         else
            mwBasket->aluOut = 0;
      }
      else if(funct == 0x2B) /* sltu */
      {

         mwBasket->wBusy = 1;
         mwBasket->type = 0;
         mwBasket->ir= deBasket->ir;
         if(sim->regs[getrs(deBasket->ir)] < sim->regs[getrt(deBasket->ir)])
            mwBasket->aluOut = 1;
         else
            mwBasket->aluOut = 0;

      }
      else if(funct == 0x08) /* jump register */               // R type jumps?????????
      {
         printf(" jr %s \n", table[getrs(n)]);
         sim->pcValue = sim->regs[getrs(n)];
         printf(" Number of cycles: 3 \n");
      }
      else if(funct == 0x09) /* jalr */
      {
         sim->pcValue += 4; 
         printf(" jalr %s \n", table[getrs(n)]);
         sim->regs[31] = sim->pcValue; 
         sim->pcValue = sim->regs[getrs(n)];
         printf(" Number of cycles: 3 \n");
      }
      else
      {
         printf("Instruction@%08X : %08X is not a valid instruction! \n", i, n);
         return;
      }
   }

   /* Function procI 
    * Processes I type instructions 
    */
   void procI(MIPS_SIM* sim, DEB* deBasket, MWB* mwBasket)
   {
      int opc;
      opc = deBasket->ir >> 26;

      if(opc == 0x08)    /* addi */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 1;
         mwBasket->ir= deBasket->ir;
         deBasket->aluOut = sim->regs[getrs(deBasket->ir)] + getimm16(deBasket->ir);
      }
      else if(opc == 0x09)    /* addiu */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 1;
         mwBasket->ir= deBasket->ir;
         deBasket->aluOut = sim->regs[getrs(deBasket->irn)] + getimm16(deBasket->ir);
      }
      else if(opc == 0x0C)    /* andi */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 1;
         mwBasket->ir= deBasket->ir;
         deBasket->aluOut = sim->regs[getrs(n)] & (deBasket->ir& 0x0000FFFF);
      }
      else if(opc == 0x0D)    /* ori */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 1;
         mwBasket->ir= deBasket->ir;
         deBasket->aluOut = sim->regs[getrs(n)] | (deBasket->ir & 0x0000FFFF);
      }
      else if(opc == 0x0E)    /* xori */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 1;
         mwBasket->ir= deBasket->ir;
         deBasket->aluOut = sim->regs[getrs(n)] ^ (deBasket->ir & 0x0000FFFF);
      }
      else if(opc == 0x0A)    /* slti */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 1;
         mwBasket->ir= deBasket->ir;
         deBasket->aluOut = sim->regs[getrs(n)] < (deBasket->ir & 0x0000FFFF) ? 1 : 0;
      }
      else if(opc == 0x0B)    /* sltiu */
      {
         mwBasket->wBusy = 1;
         mwBasket->type = 1;
         mwBasket->ir= deBasket->ir;
         deBasket->aluOut = sim->regs[getrs(n)] < (deBasket->ir & 0x0000FFFF) ? 1 : 0;
      }
      else
      {
         printf("Instruction@%08X : %08X is not a valid instruction! \n \n", i, n);
         return;
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

   /*Function geteff returns effective address */
   int geteff(MIPS_SIM* sim, DEB* deBasket)
   {
      int n;
      n = getimm16(deBasket->ir); /* get immediate value (might be sign extended) */
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

