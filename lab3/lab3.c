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
float pack_ieee(int s,int e,int f);

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

   fract1 += 0x40000000;
   if (sign1 != 0)
      fract1 = ~fract1 + 1; //fract1 = -fract1;

   fract2 = base2 << 9;
   fract2 = fract2 >> 2;
   fract2 = fract2 & 0x3fffffff;
   fract2 += 0x40000000;
   if (sign2 != 0)
      fract2 = ~fract2 + 1;

   if (exp1 > exp2)
      fract2 = fract2 >> (exp1-exp2);
   else if(exp2 > exp1)
      fract1 = fract1 >> (exp2-exp1);

   if ( sign1 == sign2)
   {
      fract1 >>= 1;
      fract2 >>= 1;
      exp1 += 1;
      exp2 += 1;
   }
   total_fract = fract1 + fract2;

   new_exp = exp1 >= exp2 ? exp1 : exp2;

   if (sign1 == sign2)
      new_sign = sign1;
   else if (total_fract < 0)
      new_sign = 0x80000000;
   else
      new_sign = 0;

   return pack_ieee(new_sign, new_exp, total_fract);
}

/* Function subtracts two float values together */
float single_float_subract(float a, float b)
{
   return single_float_add(a,-b);
}

/* Function multiplys two float values together */
float single_float_multiply(float a, float b)
{
   if(a == 0) return 0;
   if(b == 0) return 0;

   int base1 = (unsigned int) * (unsigned int*) &a;
   int base2 = (unsigned int) * (unsigned int*) &b;
   int sign1, sign2, exp1, exp2, fract1, fract2;
   int new_exp, new_sign;
   long temp, total_fract;
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
   fract1 += 0x40000000;

   fract2 = base2 << 9;
   fract2 = fract2 >> 2;
   fract2 = fract2 & 0x3fffffff;
   fract2 += 0x40000000;

   fract1 >>= 16;
   fract2 >>= 16;
   new_exp = exp1 + exp2 + 2;
   total_fract = fract1 * fract2;
   new_sign = sign1 != sign2 ? 0x80000000 : 0;

   return  pack_ieee(new_sign, new_exp, total_fract);
}

/* Function packs values into a floating point number */
float pack_ieee(int s,int e, int f)
{
   float num;
   int float_point = 0;

   if ( s != 0 )
      f = -f;

   if ( f == 0 )
      return 0.0;

   /* Normalize */
   float_point = float_point | s;

   if (f & 0x80000000)
   {
      f = f >> 1;
      f = f & 0x7fffffff;
      e++;
   }
   else
   {
      while((f & 0x40000000) == 0 && f != 0)
      {
         f = f << 1;
         e--;
      }
   }

   f = f << 2;
   f = f >> 9;
   f = f & 0x007fffff;
   float_point += f;
   e += BIAS;
   e = e << 23;
   float_point += e;
   num = * (float *) &float_point;
   return num;
}

int main()
{

   float g, h;
   printf("Enter first number: ");
   scanf("%f", &g);
   printf("Enter the second number: ");
   scanf("%f", &h);

   printf("Add result: ");
   printf("%f\n",single_float_add(g, h));

   printf("Subtract result: ");
   printf("%f\n",single_float_subract(g, h));

   printf("Multiply result: ");
   printf("%f\n", single_float_multiply(g,h));


   return 0;
}
