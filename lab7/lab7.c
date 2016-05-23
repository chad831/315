
/*
 * Nghia Nguyen
 * Chad Benson
 *
 * 315 - Lab7 Cache Simulator
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define AMAX 10			/* Maximum (square) array size */
#define CACHESIM 1		/* Set to 1 if simulating Cache */

/*	memory management, code density, Cache emulation - statistics generation */
/*	Generated for CSC 315 Lab 5 */

/* Cache Entry */
typedef struct 
{
   int valid;
   int tag;  
   int data;

} Entry;

/* Cache Line */
typedef struct 
{
   Entry group[4];
   int count;

} Line;

/* Cache */
typedef struct
{
   Line *cache;
   int size;

} Cache;

/* Func valiCache - checks command line arguments */
int validCache(char *a[])
{
   int size, assoc;
   size = atoi(a[1]); /* cache size */
   assoc = atoi(a[2]); /* associativity */
   return ((size == 256 || size == 16) && (assoc == 1 || assoc == 2 || assoc == 4)); 
}

/* Func getIndex */
int getIndex(int n)
{
   n >>= 2; /* Move index to lower bits */
   n &= 0x000000FF; /* clear out tag */
   return n;
}

/* Func getTag */
int getTag(int n)
{
   n >>= 12; /* move tag to lower 32 bits */
   n &= 0x000FFFFF; /* remove sign ext */
   return n;
}

/* Func close program */
void closeP()
{
   printf("\nCommand Line Usage: runProgam <cachSize> <associativity> \n"
         "Valid values for cache size: 16 or 256 \n"
         "Valid values for associativity: 1, 2, or 4 \n\n");
}

/* This function gets called with each "read" reference to memory */

mem_read(int *mp)
{

   //   printf("Memory read from location %p\n", mp);  
   //   printf("Memory* read from location %d\n\n", *mp);  

}


/* This function gets called with each "write" reference to memory */

mem_write(int *mp)
{

   //   printf("Memory write to location %p\n", mp);
   //   printf("Memory* write to location %d\n\n", *mp);  

}

/* Statically define the arrays a, b, and mult, where mult will become the cross product of a and b, i.e., a x b. */

static int a[AMAX][AMAX], b[AMAX][AMAX], mult[AMAX][AMAX];

void matmul( r1, c1, c2 )
{
   int i,j,k;
   int *mp1, *mp2, *mp3;

   /* Initializing elements of matrix mult to 0.*/
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
      {
         mult[i][j]=0;
      }

   /* Multiplying matrix a and b and storing in array mult. */

   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
         for(k=0; k<c1; ++k)
         {

#if CACHESIM		/* "Hooks" to measure memory references - enabled if CACHESIM  */

            mp1 = &mult[i][j];
            mp2 = &a[i][k];
            mp3 = &b[k][j];   
            mem_read(mp1);
            mem_read(mp2);
            mem_read(mp3);
            mem_write(mp1); 
#endif
            mult[i][j]+=a[i][k]*b[k][j];

         }
}

/******************************   Main   ***************************/
int main(int argc, char *argv[])
{
   if(argc != 3) /* check for command line arguments */
   {
      closeP();
      return 0;
   }
   if(validCache(argv)) /* check command line values */
   {
      int size, assoc;
      size = atoi(argv[1]); /* cache size */
      assoc = atoi(argv[2]); /* associativity */

      Cache *cach = calloc(size, sizeof(cach)); /* init cache */

      

         int r1, c1, r2, c2, i, j, k;
      int *mp1, *mp2, *mp3;

      printf("Size of pointer is: %d\n\n", (int)sizeof(mp1));   // did he want a cast here or ????????????????????????????????????????????????

      printf("Enter rows and column for first matrix: ");
      scanf("%d%d", &r1, &c1);
      printf("Enter rows and column for second matrix: ");
      scanf("%d%d",&r2, &c2);

      /* If column of first matrix in not equal to row of second matrix, asking user to enter the size of matrix again. */
      while (c1 != r2)
      {
         printf("Error! column of first matrix not equal to row of second.\n");
         printf("Enter rows and column for first matrix: ");
         scanf("%d%d", &r1, &c1);
         printf("Enter rows and column for second matrix: ");
         scanf("%d%d",&r2, &c2);
      }

      /* Storing elements of first matrix. */

      printf("\nEnter elements of matrix 1:\n");
      for(i=0; i<r1; ++i)
         for(j=0; j<c1; ++j)
         {
            //        printf("Enter elements a%d%d: ",i+1,j+1);
            //        scanf("%d",&a[i][j]);
            a[i][j] = i+j; // build sample data

         }

      /* Storing elements of second matrix. */
      printf("\nEnter elements of matrix 2:\n");
      for(i=0; i<r2; ++i)
         for(j=0; j<c2; ++j)
         {
            //        printf("Enter elements b%d%d: ",i+1,j+1);
            //        scanf("%d",&b[i][j]);

            b[i][j] = 10 + i + j;
         }

      matmul(r1, c1, c2);  	/* Invoke matrix multiply function */	

      /* Displaying the multiplication of two matrix. */
      printf("\nOutput Matrix:\n");
      for(i=0; i<r1; ++i)
         for(j=0; j<c2; ++j)
         {
            printf("%d  ",mult[i][j]);
            if(j==c2-1)
               printf("\n\n");
         }   
      return 0;
   }
   else
   {
      closeP(); /* command line values invalid */
      return 0;
   }
}

