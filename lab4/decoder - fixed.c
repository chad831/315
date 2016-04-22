/*
 * Chad Benson
 * Nghia Nguyen
 *
 * 315
 * lab4 - mips decoder
 * starter code provided
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mips_asm_header.h"
typedef unsigned int MIPS, *MIPS_PTR;
MB_HDR mb_hdr;		/* Header area */
MIPS mem[4096];	/* Memory space */
char* table[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4",
                     "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9",
                     "$k0", "$k1", "$gp","$sp", "$fp", "$ra"};

/* Function Declarations */
int checkIJ(int n);
int getFunct(int n);
int getrs(int n);
int getrt(int n);
int getrd(int n);
int getshift(int n);
int getimm8(int n);
int getimm16(int n);
int geteff(int n, int i);
void decodeR(int n, int opc, int funct, int i);
void decodeIJ(int n, int opc,  int i);
/*************************************** MAIN ******************************/
int main()
{
   // Vars
   FILE *fd;               /* points to a file */
   int n;                  /* for reading instructions */
   int memp;
   int i;
   int opc, type, funct;
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


   /************************* Main Program Loop ************************************/

   for (i = 0; i<memp; i+=4)	/* i contains byte offset addresses */
   {
      printf("Instruction@%08X : %08X\n", i, mem[i/4]); /* print instruction */
      opc = mem[i/4] >> 26;   /* get opcode */
      if(opc == 0)            /* check if R type instr */
      {
         funct = getFunct(mem[i/4]);
         decodeR(mem[i/4], opc, funct, i); /* decode R type instruction */
      }
      else if(opc != 0)
      {
         type = checkIJ(opc);  /* check if J or I type instr */
         if(type == 0)
         {
            /* Instruction was not found */
            printf("Instruction@%08X : %08X is not a valid instruction! \n", i, mem[i/4]);
         }
         else
         {
            decodeIJ(mem[i/4], opc, i);   /* Decode I or J type instr */
         }
      }
      else        {
         /* This case should not happen */
         printf("An error occured in decoding instruction! \n");
      }
   }
   return 0;
}
/* Function checkIJ
 * Returns a function type if valid, for R type instructions
 * Otherwise returns zero
 * Assumes an integer equal to opcode
 */
int checkIJ(int n)
{
   if(n == 2)     /* j (jump - J type*/
      return 2;
   if(n == 3)     /* jal (jumap and link - J type */
      return 3;
   if(n == 8)     /* addi */
      return 8;
   if(n == 9)     /* addiu */
      return 9;
   if(n == 12)     /* andi */
      return 12;
   if(n == 13)     /* ori */
      return 13;
   if(n == 14)     /* xori */
      return 14;
   if(n == 12)     /* andi */
      return 12;
   if(n == 4)     /* beq */
      return 4;
   if(n == 5)     /* bne */
      return 5;
   if(n == 10)     /* slti */
      return 10;
   if(n == 11)     /* sltiu */
      return 11;
   if(n == 15)     /* lui */
      return 15;
   if(n == 32)     /* load byte*/
      return 32;
   if(n == 33)     /* load half word */
      return 33 ;
   if(n == 35)     /* load word */
      return 35;
   if(n == 36)     /* load byte unsigned */
      return 36;
   if(n == 37)     /* load halfword unsigned*/
      return 37;
   if(n == 40)     /* store byte */
      return 40;
   if(n == 41)     /* store half word */
      return 41;
   if(n == 43)     /* store word */
      return 43;
   return 0;      /* was not I or J type */
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
void decodeR(int n, int opc, int funct, int i)
{
   if(n == 0x000000C)   /* syscall case */
   {
      printf("Special Instruction: Syscall \n \n");
      return;
   }
   else if(funct == 0x20)    /* add */
   {
      printf("Instruction: add, Type: R, Opc: 00000, Funct: 0x20 \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x21)    /* addu */
   {
      printf("Instruction: add unsigned, Type: R, Opc: 00000, Funct: 0x21 \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x22)  /* subtract */
   {
      printf("Instruction: subtract, Type: R, Opc: 00000, Funct: 0x22 \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x23) /* subtract unsigned */
   {
      printf("Instruction: subtract unsigned, Type: R, Opc: 00000, Funct: 0x23 \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x24) /* and */
   {
      printf("Instruction: and, Type: R, Opc: 00000, Funct: 0x24 \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x27) /* nor */
   {
      printf("Instruction: nor, Type: R, Opc: 00000, Funct: 0x27 \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x25) /* or */
   {
      printf("Instruction: or, Type: R, Opc: 00000, Funct: 0x25 \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x26) /* xor */
   {
      printf("Instruction: xor, Type: R, Opc: 00000, Funct: 0x26 \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x00) /* sll */
   {
      printf("Instruction: sll (shift left logical), Type: R, Opc: 00000, Funct: 0x00 \n"
            "reg reg rt: %s, reg rd: %s \n"
            "Shift amount: %d  \n",
            table[getrt(n)], table[getrd(n)], getshift(n));
      return;
   }
   else if(funct == 0x02) /* srl */
   {
      printf("Instruction: srl (shift right logical), Type: R, Opc: 00000, Funct: 0x02 \n"
            "reg rt: %s, reg rd: %s \n"
            "Shift amount: %d  \n",
            table[getrt(n)], table[getrd(n)], getshift(n));
      return;
   }
   else if(funct == 0x03) /* sra */
   {
      printf("Instruction: sra (shift right arithmetic), Type: R, Opc: 00000, Funct: 0x03 \n"
            "reg rt: %s, reg rd: %s \n"
            "Shift amount: %d  \n",
            table[getrt(n)], table[getrd(n)], getshift(n));
      return;
   }
   else if(funct == 0x04) /* sllv */
   {
      printf("Instruction: sra (shift left logical variable), Type: R, Opc: 00000, Funct: 0x04 \n"
            "reg rs (holds shift amount): %s, reg rt (the register to be shifted): %s, reg rd: %s \n"
            "Shift amount: %d  \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)], 7);     /* how do you get the shift amount?? */
      return;
   }
   else if(funct == 0x06) /* srlv */
   {
      printf("Instruction: srlv (shift right logical variable), Type: R, Opc: 00000, Funct: 0x06 \n"
            "reg rs (holds shift amount): %s, reg rt (the register to be shifted): %s, reg rd: %s \n"
            "Shift amount: %d  \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)], 7);     /* how do you get the shift amount??? */
      return;
   }
   else if(funct == 0x07) /* srav */
   {
      printf("Instruction: srlv (shift right arithmetic variable), Type: R, Opc: 00000, Funct: 0x07 \n"
            "reg rs (holds shift amount): %s, reg rt (the register to be shifted): %s, reg rd: %s \n"
            "Shift amount: %d \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)], 7);     /* how do you get the shift amount??? */
      return;
   }
   else if(funct == 0x2A) /* slt */
   {
      printf("Instruction: slt (set less than), Type: R, Opc: 00000, Funct: 0x2A \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x2B) /* sltu */
   {
      printf("Instruction: slt (set less than unsigned), Opc: 00000, Type: R, Funct: 0x2B \n"
            "reg rs: %s, reg rt: %s, reg rd: %s \n",
            table[getrs(n)], table[getrt(n)], table[getrd(n)]);
      return;
   }
   else if(funct == 0x08) /* jump */
   {
      printf("Instruction: jr (jump register), Type: R, Opc: 00000, Funct: 0x08 \n"
            "Set PC equal to reg rs: %s \n",
            table[getrs(n)]);
      return;
   }
   else if(funct == 0x09) /* jalr */
   {
      printf("Instruction: jalr (jump and link register), Type: R, Opc: 00000, Funct: 0x08 \n"
            "Set PC equal to reg rs: %s and set $ra to PC + 4; \n",
            table[getrs(n)]);
      return;
   }
   else
   {
      printf("Instruction@%08X : %08X is not a valid instruction! \n", i, n);
   }
}
/* Function DecodeIJ
 * Decodes and prints I or J type if valid
 */
void decodeIJ(int n, int opc, int i)
{
   if(opc == 0x02 || opc == 0x03)    /* j or jal  */
   {
      int temp = n;
      n &= 0xF0000000; /* save PC */
      temp &= 0x0FFFFFFF; /* obtain 26 address*/
      temp <<= 2;             /* multiply by 4 */
      n += temp;              /* effective address */
      if(opc == 0x02)
      {
         printf("Instruction: jump, Type: J, Opc: 02 \n"
               "reg rt: %s, reg rs: %s, effective address %08X \n \n",
               table[getrt(n)], table[getrs(n)], n);
         return;
      }
      else
      {
         printf("Instruction: jump and link, Type: J, Opc: 0x03 \n"
               "reg rt: %s, reg rs: %s, effective address %08X \n \n",
               table[getrt(n)], table[getrs(n)], n);
         return;
      }
   }
   else if(opc == 0x08)    /* addi */
   {
      printf("Instruction: addi, Type: I, Opc: 0x08 \n"
            "reg rt: %s, reg rs: %s, sign ext immediate value %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      return;
   }
   else if(opc == 0x09)    /* addi */
   {
      printf("Instruction: addiu, Type: I, Opc: 0x09 \n"
            "reg rt: %s, reg rs: %s, sign ext immediate value %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      return;
   }
   else if(opc == 0x0C)    /* andi */
   {
      printf("Instruction: andi, Type: I, Opc: 0x0C \n"
            "reg rt: %s, reg rs: %s, zero ext immediate value %08X \n \n",
            table[getrt(n)], table[getrs(n)], n & 0X0000FFFF);
      return;
   }
   else if(opc == 0x0D)    /* ori */
   {
      printf("Instruction: ori, Type: I, Opc: 0x0D \n"
            "reg rt: %s, reg rs: %s, zero ext immediate value %08X \n \n",
            table[getrt(n)], table[getrs(n)], n & 0X0000FFFF);
      return;
   }
   else if(opc == 0x0E)    /* xori */
   {
      printf("Instruction: xori, Type: I, Opc: 0x0E \n"
            "reg rt: %s, reg rs: %s, zero ext immediate value %08X \n \n",
            table[getrt(n)], table[getrs(n)], n & 0X0000FFFF);
      return;
   }
   else if(opc == 0x0A)    /* slti */
   {
      printf("Instruction: slti, Type: I, Opc: 0x0A \n"
            "reg rt: %s, reg rs: %s, sign ext immediate value %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      return;
   }
   else if(opc == 0x0B)    /* sltiu */
   {
      printf("Instruction: sltiu, Type: I, Opc: 0x0B \n"
            "reg rt: %s, reg rs: %s, sign ext immediate value %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      return;
   }
   else if(opc == 0x04)    /* beq */
   {
      printf("Instruction: beq, Type: I, Opc: 0x04 \n"
            "reg rt: %s, reg rs: %s, Effective address: %08X \n \n",
            table[getrt(n)], table[getrs(n)], geteff(n, i));
      return;
   }
   else if(opc == 0x05)    /* bne */
   {
      printf("Instruction: bne, Type: I, Opc: 0x05 \n"
            "reg rt: %s, reg rs: %s, Effective address: %08X \n \n",
            table[getrt(n)], table[getrs(n)], geteff(n, i));
      return;
   }
   else if(opc == 0x20)    /* lb */
   {
      printf("Instruction: lb, Type: I, Opc: 0x20 \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm8(n));
      return;
   }
   else if(opc == 0x24)    /* lbu */
   {
      printf("Instruction: lbu, Type: I, Opc: 0x24 \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrt(n)], table[getrs(n)], n & 0x000000FF);
      return;
   }
   else if(opc == 0x21)    /* lh */
   {
      printf("Instruction: lh, Type: I, Opc: 0x21 \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      return;
   }
   else if(opc == 0x21)    /* lhu */
   {
      printf("Instruction: lhu, Type: I, Opc: 0x21 \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrt(n)], table[getrs(n)], n & 0x0000FFFF);
      return;
   }
   else if(opc == 0x0F)    /* lui */
   {
      printf("Instruction: lui, Type: I, Opc: 0x21 \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrt(n)], table[getrs(n)], n << 16);
      return;
   }
   else if(opc == 0x23)    /* lw */
   {
      printf("Instruction: lw, Type: I, Opc: 0x23 \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrs(n)], table[getrt(n)], getimm16(n));
      return;
   }
   else if(opc == 0x28)    /* sb */
   {
      printf("Instruction: lw, Type: I, Opc: 0x28 \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm8(n));
      return;
   }
   else if(opc == 0x29)    /* sh */
   {
      printf("Instruction: lw, Type: I, Opc: 0x29 \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      return;
   }
   else if(opc == 0x2B)    /* sh */
   {
      printf("Instruction: lw, Type: I, Opc: 0x2B \n"
            "reg rt: %s, reg rs: %s, %08X \n \n",
            table[getrt(n)], table[getrs(n)], getimm16(n));
      return;
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
int geteff(int n, int i)
{
   i += 4;  /* advance PC */
   n = getimm16(n); /* get immediate value (might be sign extended) */
   n <<= 2;       /* word align the value */
   return n + i;  /* add PC to Branch Address */
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







