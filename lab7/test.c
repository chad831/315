

#include <stdio.h>
#define MAX 0
#define MIN 0

int main(int argc, char *argv[])
{

   int a = 0xABCDF000;

   printf("a: %08x \n", a);
   a >>= 12;
   printf("a after: %08x \n", a);

   return 0;

}

