/*
 * Chad Benson
 * Nghia Nguyen
 *
 * Lab 4 - Mips Decoder
 * Reads in Mips Binary File
 * Decodes and Displays Mips Instructions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mips_asm_header.h"

/* Function Declarations */
//unsigned char *read_file(char *fn);
unsigned char *read_file();

/* Main */
int main(int argc, char *argv[])
{
   unsigned char *bp;               // byte pointer               
   bp = read_file();                // Read in file from user and return buffer
   printf("Contents:\n%s", bp);
   

   return 0;
}

/* Functions */

/* Function read file prompts user for a file,
 * reads into and returns a buffer
 * Assumes file name is valid,
 * but function will print error if it is not
 */
unsigned char *read_file()
{
   char fn[50];            // array for input for file name
   unsigned char *buffer;           // buffer to store mips instructions
   FILE *fp;               // file pointer
   unsigned long flen;     // file length
   printf("Enter a mips binary file to decode: ");
   gets(fn);               // try to open file 
   fp = fopen(fn, "rb"); 
   if(!fp)
   {
      perror("Error opening file... \n");
      return;
   }
   fseek(fp, 0, SEEK_END);  // get file length and allocate space
   flen = ftell(fp);
   fseek(fp, 0, SEEK_SET);
   buffer = (unsigned char*) malloc(sizeof(flen +1));
   if(!buffer)
   {
      perror("Error allocating memory space! \n");
      fclose(fp);
      return buffer;
   }
   fread(buffer, flen, 1, fp);      // read file into buffer
   return buffer;
}
