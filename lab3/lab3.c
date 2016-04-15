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
#define BIAS 127
/* Function Declarations: */
float single_float_add(float a, float b);
float single_float_subract(float a, float b);
float single_float_multiply(float a, float b);
float pack_ieee(int s,int e,int f); // float???
float compare(int a, int b);


/* Functions */
/* Function adds two float values together */

float single_float_add(float a, float b)
{
   if(a == 0) return b;
   if(b == 0) return a;

    int base1 = (unsigned int) * (unsigned int*) &a;
    int base2 = (unsigned int) * (unsigned int*) &b;
    int sign1, sign2, exp1, exp2, fract1, fract2;
    int new_exp, total_fract, new_sign;
   /* store sign bits */
   sign1 = base1 & 0x80000000;
   sign2 = base2 & 0x80000000;

   /* set exponents equal */
   exp1 = base1 & 0x7f800000;
   exp2 = base2 & 0x7f800000;
   exp1 = exp1 >> 23;
   exp2 = exp2 >> 23;
   exp1 -= BIAS;
   exp2 -= BIAS;
   fract1 = base1 << 9;
   fract1 = fract1 >> 2;
   fract1 = fract1 & 0x3fffffff;
    printf("fract1: %.8x\n", fract1);

   fract1 += 0x40000000;
   if (sign1 != 0)
      fract1 = ~fract1 + 1;
   fract2 = base2 << 9;
   fract2 = fract2 >> 2;
   fract2 = fract2 & 0x3fffffff;
   printf("fract2: %.8x\n", fract2);
   fract2 += 0x40000000;
   if (sign2 != 0)
      fract2 = ~fract2 + 1;
  // temp = compare(exp1, exp2);
   if (exp1 > exp2)
      fract2 = fract2 >> (exp1-exp2);
   else if(exp2 > exp1)
      fract1 = fract1 >> (exp2-exp1);

   total_fract = fract1 + fract2;
   //total_fract = total_fract &0x4ffffff;
   new_exp = exp1 >= exp2 ? exp1 : exp2;

   if(sign1 == sign2)
      new_sign = sign1;
   else
   {
      if (fract1 + fract2 < 0)
         new_sign = sign1 < sign2 ? sign1 : sign2;
      else
         new_sign = 0;
   }
   printf("b1 float: %f \n", a);
   printf("b2 float: %f \n", b);
   printf("b1 int: %.8x \n", base1);
   printf("b2 int: %.8x \n", base2);
   printf("exp1: %d\n", exp1);
   printf("exp2: %d\n", exp2);
   printf("fract1: %.8x\n", fract1);
   printf("fract2: %.8x\n", fract2);
   printf("Total fract: %.8x\n", total_fract);
   printf("New exp: %d\n", new_exp);
   printf("Sign 1: %.8x\n", sign1);
   printf("Sign 2: %.8x\n", sign2);
   printf("New Sign: %.8x\n", new_sign);


   return pack_ieee(new_sign, new_exp, total_fract);
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
float pack_ieee(int s,int e, int f) // float????
{
   float num;
   int float_point = 0;
   printf("\n\ninside\n");

   float_point = float_point | s;
   if  (s != 0)
      f = ~f + 1;
   if (f & 0x80000000)
   {
      f = f >> 1;
      f = f & 0x7fffffff;
      e++;
   }
   else
   {
      while((f & 0x40000000) == 0)
      {
         f = f << 1;
         e--;
      }
   }

   f = f << 2;
   f = f >> 9;
   f = f & 0x007fffff;
   float_point += f;
   printf("new exp: %u\n",e);
   e += BIAS;
   e = e << 23;
   printf("NewEx: %.8x\n", e);
   float_point += e;
   printf("new fract: %.8x\n",f);
   //printf("new exp: %.8x\n",e);
   printf("float_point: %.8x\n", float_point);
   num = * (float *) &float_point;
   printf("%f\n", num);
   return num;
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
   g = 1.25;
   h = 2.75;
   k = (unsigned int) * (unsigned int *) &g;
   //j = (unsigned int) g; ???????

   single_float_add(g, h);

//   printf("k: %f \n", i);


   //printf("k: %08X \n", k);
 //  printf("k: %d \n", k);
 //  printf("k: %08X \n", k);

   return 0;
}
