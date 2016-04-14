/*
 *
 * Nghia Nguyen and Chad Benson
 * Lab 3 - Floating Point Arithmetic
 * Functions 3, 4, 5, 6 in C
 *
 */

#include <stdio.h>
#define  TRUE 1
#define  FALSE 0

/* Function Declarations: */
float single_float_add(float a, float b);
float single_float_subract(float a, float b);
float single_float_multiply(float a, float b);
float pack_ieee(int s, int e, float f); // float???
float compare(int a, int b);


/* Functions */
/* Function adds two float values together */

float single_float_add(float a, float b)
{
   if(a == 0) return b;
   if(b == 0) return a;

   int base1 = (unsigned int) * (unsigned int*) &a;
   int base2 = (unsigned int) * (unsigned int*) &b;
   int sign1, sign2, exp1, exp2, fract1, fract2, temp;
   
   /* store sign bits */
   sign1 = base1 & 0x80000000;
   sign2 = base2 & 0x80000000;

   /* set exponents equal */
   exp1 = base1 & 0x7f800000;
   exp2 = base2 & 0x7f800000;
   temp = compare(exp1, exp2); 



   
   printf("b1 float: %f \n", a);
   printf("b2 float: %f \n", b);
   printf("b1 int: %d \n", base1);
   printf("b2 int: %d \n", base2);


   return -1.3;
}

/* Function subtracts two float values together */
float single_float_subract(float a, float b)
{
   return -1.4;
}

/* Function multiplys two float values together */
float single_float_multiply(float a, float b)
{
   return -1.5;
}

/* Function packs values into a floating point number */
float pack_ieee(int s, int e, float f) // float????
{
   return -1.6;
}

/* returns the smaller of two floats */
float compare(int a, int b)
{
   return a <= b ? a:b;  
}



int main()
{ 

   float g, h, i;
   unsigned int k, j;
   g = 2.25;
   h = 1.75;
   k = (unsigned int) * (unsigned int *) &g;
   //j = (unsigned int) g; ???????
   
   single_float_add(g, h);

//   printf("k: %f \n", i);


   //printf("k: %08X \n", k);
 //  printf("k: %d \n", k);
 //  printf("k: %08X \n", k);

   return 0;
}
