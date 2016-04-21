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



/* Function Declarations */

/* Function isIType */
int isIType(int n);





main()
{
   // Vars
   FILE *fd;               /* points to a file */
   int n;                  /* for reading instructions */
   int memp;
   int i;
   int opc, funct, regs;
   char filename[20];      /* for file name */ 
   unsigned char *bp;      /* byte pointer */
   unsigned int *wp;       /* word pointer */
   unsigned int wordval;    /* instruction value */
  // printf("Enter a mips file to decode: ");
  // scanf("%s", filename);
   /* format the MIPS Binary header */

   //fd = fopen(filename, "rb");
   fd = fopen("testcase1.mb", "rb");
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

   /* ok, now dump out the instructions loaded: */

//   for (i = 0; i<memp; i+=4)	/* i contains byte offset addresses */
//   {
//      printf("Instruction@%08X : %08X\n", i, mem[i/4]);
//   }
   printf("\n");

 


/*
 * INstructions for testcase1
 * 1. 0000 1100 0001 0000 0000 0000 0000 1100
 * 2. 0011 0100 0000 0010 0000 0000 0000 1010
 * 3. 0000 0000 0000 0000 0000 0000 0000 1010
 * 4. 0000 0010 0001 0001 0100 0000 0010 0110
 * 5. 0011 0001 0000 0101 0000 0000 0000 0010
 * 6  0001 0000 1000 1000 1111 1111 1111 1110
 * 7. 0000 1000 0001 0000 0000 0000 0001 0000
 * 8. 0000 0011 1110 0000 0000 0000 0000 1000 
 *
 * if ( instrIsValid )
 *    decode ( instru )
 *
 * else printf Invalid INstr
 *
 *
  */

/* todo: ******************* */
   
    for (i = 0; i<memp; i+=4)	/* i contains byte offset addresses */
   {
         opc = mem[i/4] >> 27;   /* get opcode */
         if(opc == 0)
         {
            //decodeR();
            printf("is r type \n");
         } else if(opc == 2 || opc == 3)  
         {
            printf("is j type \n");
         }
         printf("b4 shift %08x    after shift %d  \n", mem[i/4], n);
      
  //    printf("Instruction@%08X : %08X\n Value: %d \n", i, mem[i/4], mem[i/4]);
   }
   printf("\n");




   exit(0);
}




int isIType(int n)
{
   int i;
   for(i = 0; i<100; i++)
   {
      if(n == 8)     /* addi */
         return 8;
      if(n == 9)     /* addiu */
         return 9;
      if(n == 12)     /* andi */
         return 12;
      if(n == 4)     /* beq */
         return 4;
      if(n == 5)     /* bne*/
         return 5;
      if(n == 36)     /* load byte unsigned */
         return 36;
      if(n == 37)     /* load halfword unsigned*/
         return 37;
      if(n == 8)     /* addi */
         return 8;
      if(n == 8)     /* addi */
         return 8;
      if(n == 8)     /* addi */
         return 8;










   }

      return 0; 
}






