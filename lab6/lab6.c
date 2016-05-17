/*
 * Chad Benson
 * Nghia Nguyen
 *
 * 315
 * lab6 - mips multi cycle simulator
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
   int bReg;

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
int geteff(int n, int i);
void procR(MIPS_SIM* sim, DEB* deBasket, MWB* mwBasket);
void procI(MIPS_SIM* sim, DEB* deBasket, MWB* mwBbasket);
/* Functs for running simulator */
void simDefault(MIPS_SIM* sim);
void clearfd(FDB* fdBasket);
void clearde(DEB* deBasket);
void clearem(EMB* emBasket);
void clearmw(MWB* mwBasket);
void basketDefault(FDB* fdBasket, DEB* deBasket, EMB* emBasket, MWB* mwBasket); 
int prompt();
void initProgram();
int getPC();
int getInstr(MIPS_SIM* sim, DEB* deBasket);
void printRegs(MIPS_SIM* sim);
/* Functs for 5 multi cycle stages */
void wb(MIPS_SIM* sim, MWB* mwBasket);  /* write back */  
void m(MIPS_SIM* sim, EMB* emBasket, MWB* mwBasket); /* memory write */
void exe(MIPS_SIM* sim, FDB* fdBasket, DEB* deBasket, EMB* emBasket, MWB* mwBasket); /* execute */
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

      wb(sim, mwBasket);  /* write back */  
      m(sim, emBasket, mwBasket); /* memory write */
      exe(sim, fdBasket, deBasket, emBasket, mwBasket); /* execute */
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

/* Funct wb, stage five, assumes the mwBasket */
void wb(MIPS_SIM* sim, MWB* mwBasket)  /* write back */  
{
   /* Check in Basket */
   if(mwBasket->mdr != 0 || mwBasket->aluOut != 0)
   {
      switch (mwBasket->type)
      {
         case 0: /* R type */
            sim->regs[getrd(mwBasket->ir)] = mwBasket->aluOut;
            break;

         case 1: /* I type */

            sim->regs[mwBasket->bReg] = mwBasket->aluOut;
            break;

         case 2: /* Load Word */

            if(mwBasket->mdr != 0)          
               sim->regs[mwBasket->bReg] = mwBasket->mdr;
            break;
      }
      clearmw(mwBasket);
   }    
   else
      return;

}
void m(MIPS_SIM* sim, EMB* emBasket, MWB* mwBasket) /* memory write */
{
   if(emBasket->aluOut != 0)
   {
      int opc = emBasket->ir >> 26;
      if(isStore(opc))  /* For Store Word Insturctions */
      {
         switch(opc)
         {
            case 0x28: /* store byte */

               mem[emBasket->aluOut] = emBasket->bReg & 0x000000FF;
               break;

            case 0x29: /* store half word */

               mem[emBasket->aluOut] = emBasket->bReg & 0x0000FFFF;
               break;

            case 0x2B: /* store word */

               mem[emBasket->aluOut] = emBasket->bReg;
               break;
         }
         clearem(emBasket);
      }
      else           /* For Load Word Instrustions */
      {
         if(mwBasket->wBusy)
            return;
         else
         {
            mwBasket->wBusy = 1;
            mwBasket->type = 2;
            mwBasket->mdr = mem[emBasket->aluOut]; /* write to mdr */
            clearem(emBasket);
         }
      }
   }
   else
      return;
}

void exe(MIPS_SIM* sim, FDB* fdBasket, DEB* deBasket, EMB* emBasket, MWB* mwBasket) 
{
   // four cases: J and Branch (0), R type (1), store word (2), load word (3), I type (4)

   if(deBasket->ir != 0)
   {

      int value;
      value = getInstr(sim, deBasket);
      switch (value)
      {
         int opc = deBasket->ir >> 26;
         // Execute Jump or Branch Type  *********************************************** 
         case 0:

         if(opc == 0x02) /* Jump */
         {  
            sim->pcValue = deBasket->ir;  /* set new PC value from IR */
            basketDefault(fdBasket, deBasket, emBasket, mwBasket);   /* clear baskets */
         }
         else if(opc == 0x03) /* Jump and Link */
         {

            sim->regs[31] = sim->pcValue; /* store ra */       // does this need to be pc + 4 ????
            sim->pcValue = deBasket->ir;  /* set new PC value from IR */
            basketDefault(fdBasket, deBasket, emBasket, mwBasket);   /* clear baskets */

         }
         else if(opc == 0x04) /* Branch on Equal */
         {
            if(deBasket->bReg == deBasket->aReg)   /* compares registers */
            {
               sim->pcValue = geteff(deBasket->ir, sim->pcValue); /* if equal set effective address */
               basketDefault(fdBasket, deBasket, emBasket, mwBasket);   /* clear baskets */
            }
               else return;
         }
         else  /* Branch not equal */
         {
           if(deBasket->bReg != deBasket->aReg)   /* compares registers */
           {
               sim->pcValue = geteff(deBasket->ir, sim->pcValue); /* if not equal set effective address */
               basketDefault(fdBasket, deBasket, emBasket, mwBasket);   /* clear baskets */
           }
            else return;
         }

         break;

         // Execute R Type Instuctrion **********************************************
         case 1:
         if(mwBasket->wBusy)
            return;
         else
         {
            /* update in / out baskets */
            mwBasket->wBusy = 1;
            mwBasket->type = 0;
            mwBasket->ir = deBasket->ir;
            procR(sim, deBasket, mwBasket);
            clearde(deBasket);
         }

         break;

         // Execute Store type 
         case 2:
         if(emBasket->mBusy)
            return;
         else
         {

            if(opc == 0x28)    /* sb */
            {
               emBasket->mBusy = 1;
               emBasket->ir = deBasket->ir;
               emBasket->bReg = deBasket->bReg;
               emBasket->aluOut = getimm8(deBasket->ir) + deBasket->aReg/4;

            }
            else if(opc == 0x29)    /* sh */
            {
               emBasket->mBusy = 1;
               emBasket->ir = deBasket->ir;
               emBasket->bReg = deBasket->bReg;
               emBasket->aluOut = getimm16(deBasket->ir) + deBasket->aReg/4;
            }
            else if(opc == 0x2B)    /* sw */
            {
               emBasket->mBusy = 1;
               emBasket->ir = deBasket->ir;
               emBasket->bReg = deBasket->bReg;
               emBasket->aluOut = getimm16(deBasket->ir) + deBasket->aReg/4;
            }
            clearde(deBasket);
            sim->numOfRWs ++;

         }
         break;

         // Execute Load type ***********************************
         case 3:

         if(opc == 0x0F)    /* lui */
         {
            if(mwBasket->wBusy)
               return;
            else
            {  
               mwBasket->wBusy = 1;
               mwBasket->type = 1;
               mwBasket->ir = deBasket->ir;
               mwBasket->bReg = deBasket->bReg;
               mwBasket->aluOut = deBasket->ir << 16;
               procI(sim, deBasket, mwBasket);
               clearde(deBasket);
            }
            sim->regs[deBasket->bReg] = deBasket->ir << 16;

         }

         emBasket->mBusy = 1;
         mwBasket->type = 2;
         emBasket->ir = deBasket->ir;

         if(opc == 0x20)    /* lb */         //////////////////////////////////////// double check your loads
         {
            emBasket->aluOut = deBasket->aReg + getimm8(deBasket->ir)/4;
         }

         else if(opc == 0x24)    /* lbu */
         {
            emBasket->aluOut = deBasket->aReg + getimm8(deBasket->ir)/4;
        }
         else if(opc == 0x21)    /* lh */
         {
            emBasket->aluOut = deBasket->aReg + getimm16(deBasket->ir)/4;
        }
         else if(opc == 0x21)    /* lhu */
         {
            emBasket->aluOut = mem[deBasket->aReg] + getimm16(deBasket->ir)/4;
            // sim->regs[getrt(n)] = mem[(sim->regs[getrs(n)] + (n & 0x0000FFFF))/4];
            // sim->regs[getrt(n)] &= 0x0000FFFF; *************************** when does this happend????
         }
         else if(opc == 0x23)    /* lw */
         {
            emBasket->aluOut = deBasket->aReg + getimm16(deBasket->ir)/4;
         }

         sim->numOfRWs ++;
         clearde(deBasket);

         break;
         // Process I Type Instuctrion ***************************************
         case 4:
         if(mwBasket->wBusy)
            return;
         else
         {  
            mwBasket->wBusy = 1;
            mwBasket->type = 1;
            mwBasket->ir = deBasket->ir;
            procI(sim, deBasket, mwBasket);
            clearde(deBasket);
         }

         break;
      }
   }
}

void d(MIPS_SIM* sim, FDB* fdBasket, DEB* deBasket) /* instruction decode */
{

   if(fdBasket->ir == 0) return; /* No Op - Special case */
   if(deBasket->eBusy) /* If next stage is busy return */
      return;
   else
   {
      deBasket->ir = fdBasket->ir; /* pass ir */
      deBasket->aReg = sim->regs[getrs(fdBasket->ir)]; /* load source A */
      deBasket->bReg = sim->regs[getrt(fdBasket->ir)]; /* load source B */
      deBasket->aluOut = geteff(fdBasket->ir, sim->pcValue);    /* get effective address */   
      deBasket->eBusy = 1;

      clearfd(fdBasket);
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
      fdBasket->dBusy = 1;

   } 

}
/* Function getPC - returns pc value */
int getPC(MIPS_SIM* sim)
{
   return sim->pcValue;
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

/* Function procR
 * ProcR processes R type instruction
 * Assumes multi cycle simulator, and two baskets 
 */
void procR(MIPS_SIM* sim, DEB* deBasket, MWB* mwBasket) 
{
   int funct = getFunct(deBasket->ir); 

   mwBasket->wBusy = 1;
   mwBasket->type = 0;
   mwBasket->ir= deBasket->ir;

   if (funct == 0xC)
   {
      return; 
   }
   else if(funct == 0x20)    /* add */
   {
      mwBasket->aluOut = deBasket->aReg + deBasket->bReg;
   }
   else if(funct == 0x21)    /* addu */
   {
      mwBasket->aluOut = deBasket->aReg + deBasket->bReg;
   }
   else if(funct == 0x22)  /* subtract */
   {
      mwBasket->aluOut = deBasket->aReg - deBasket->bReg;

   }
   else if(funct == 0x23) /* subtract unsigned */
   {
      mwBasket->aluOut = deBasket->aReg - deBasket->bReg;
   }
   else if(funct == 0x24) /* and */
   {
      mwBasket->aluOut =  deBasket->aReg & deBasket->bReg;
   }
   else if(funct == 0x27) /* nor */
   {
      mwBasket->aluOut  = ~(deBasket->aReg | deBasket->bReg);
   }
   else if(funct == 0x25) /* or */
   {
      mwBasket->aluOut  =  deBasket->aReg | deBasket->bReg;
   }
   else if(funct == 0x26) /* xor */
   {
      mwBasket->aluOut  =  deBasket->aReg ^ deBasket->bReg;
   }
   else if(funct == 0x00) /* sll */
   {
      mwBasket->aluOut =  deBasket->bReg << getshift(deBasket->ir);
   }
   else if(funct == 0x02) /* srl */
   {
      mwBasket->aluOut  =  deBasket->bReg >> getshift(deBasket->ir);
   }
   else if(funct == 0x03) /* sra */                                        
   {
            int temp = 0x80000000, i=0;
            mwBasket->aluOut = deBasket->bReg;
            if ((deBasket->bReg & temp) != 0)
            {
               while (i < getshift(deBasket->ir))
               {
                  mwBasket->aluOut >>= 1;
                  mwBasket->aluOut += temp;
                  i++;
               }
            }
            else
               mwBasket->aluOut >>= getshift(deBasket->ir);
   }
   else if(funct == 0x04) /* sllv */
   {
      mwBasket->aluOut = deBasket->bReg << deBasket->aReg; 
   }
   else if(funct == 0x06) /* srlv */
   {
      mwBasket->aluOut  = deBasket->bReg >> deBasket->aReg; 
   }
   else if(funct == 0x07) /* srav */                                          
   {
            int temp = 0x80000000, i=0;
            mwBasket->aluOut = deBasket->bReg;
            if ((deBasket->bReg & temp) != 0)
            {
               while (i < deBasket->aReg)
               {
                  mwBasket->aluOut >>= 1;
                  mwBasket->aluOut += temp;
                  i++;
               }
            }
            else
               mwBasket->aluOut >>= deBasket->aReg;
   }
   else if(funct == 0x2A) /* slt */
   {
      if(sim->regs[deBasket->aReg] < sim->regs[deBasket->bReg])
         mwBasket->aluOut = 1;
      else
         mwBasket->aluOut = 0;
   }
   else if(funct == 0x2B) /* sltu */
   {

      if(sim->regs[deBasket->aReg] < sim->regs[deBasket->bReg])
         mwBasket->aluOut = 1;
      else
         mwBasket->aluOut = 0;

   }
   else if(funct == 0x08) /* jump register */               /////////////////////////////////////// R type jumps?????????
   {
      //sim->pcValue = sim->regs[getrs(n)];
      //      sim->pcValue = sim->regs[getrs(n)];
   }
   else if(funct == 0x09) /* jalr */
   {
      sim->pcValue += 4; 
      sim->regs[31] = sim->pcValue; 
      //     sim->pcValue = sim->regs[getrs(n)];
   }
   else
   {
      printf("Instruction@%08X : %08X is not a valid instruction! \n", sim->pcValue, deBasket->ir);
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
      mwBasket->aluOut = deBasket->aReg + getimm16(deBasket->ir);
   }
   else if(opc == 0x09)    /* addiu */
   {
      mwBasket->wBusy = 1;
      mwBasket->type = 1;
      mwBasket->ir= deBasket->ir;
      mwBasket->aluOut = deBasket->aReg + getimm16(deBasket->ir);
   }
   else if(opc == 0x0C)    /* andi */
   {
      mwBasket->wBusy = 1;
      mwBasket->type = 1;
      mwBasket->ir= deBasket->ir;
      mwBasket->aluOut = deBasket->aReg & (deBasket->ir& 0x0000FFFF);
   }
   else if(opc == 0x0D)    /* ori */
   {
      mwBasket->wBusy = 1;
      mwBasket->type = 1;
      mwBasket->ir= deBasket->ir;
      mwBasket->aluOut = deBasket->aReg | (deBasket->ir & 0x0000FFFF);
   }
   else if(opc == 0x0E)    /* xori */
   {
      mwBasket->wBusy = 1;
      mwBasket->type = 1;
      mwBasket->ir= deBasket->ir;
      mwBasket->aluOut = deBasket->aReg ^ (deBasket->ir & 0x0000FFFF);
   }
   else if(opc == 0x0A)    /* slti */
   {
      mwBasket->wBusy = 1;
      mwBasket->type = 1;
      mwBasket->ir= deBasket->ir;
      mwBasket->aluOut = deBasket->aReg < (deBasket->ir & 0x0000FFFF) ? 1 : 0;
   }
   else if(opc == 0x0B)    /* sltiu */
   {
      mwBasket->wBusy = 1;
      mwBasket->type = 1;
      mwBasket->ir= deBasket->ir;
      mwBasket->aluOut = deBasket->aReg < (deBasket->ir & 0x0000FFFF) ? 1 : 0;
   }
   else
   {
      printf("Instruction@%08X : %08X is not a valid instruction! \n \n", sim->pcValue, deBasket->ir);
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
int geteff(int n, int i)
{
   int t;
   t = getimm16(n); /* get immediate value (might be sign extended) */
   t <<= 2;       /* word align the value */
   return t + i;  /* add PC to Branch Address */
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

/* Funct clears FD basket */
void clearfd(FDB* fdBasket)
{
   fdBasket->ir = 0;
   fdBasket->dBusy = 0;

}
/* Funct clears DE basket */
void clearde(DEB* deBasket)
{
   deBasket->eBusy = 0;
   deBasket->ir = 0;
   deBasket->aReg = 0;
   deBasket->bReg = 0;
   deBasket->aluOut = 0;

}
/* Funct clears EM basket */
void clearem(EMB* emBasket)
{
   emBasket->mBusy = 0;
   emBasket->ir = 0;
   emBasket->bReg = 0;
   emBasket->aluOut = 0;

}
/* Funct clears MW basket */
void clearmw(MWB* mwBasket)
{
   mwBasket->wBusy = 0;
   mwBasket->ir = 0;
   mwBasket->type = 0;
   mwBasket->mdr = 0;
   mwBasket->aluOut = 0;
   mwBasket->bReg = 0;

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

