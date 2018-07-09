

/* mountain.c - Generate the memory mountain. */
/* $begin mountainmain */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <zconf.h>
#include <math.h>
#include <gsl/gsl_blas.h>
#include "fcyc2.h" /* measurement routines */
#include "clock.h" /* routines to access the cycle counter */

#define MINBYTES (1 << 11)  /* Working set size ranges from 2 KB */
#define MAXBYTES (1 << 25)  /* ... up to 64 MB */
#define MAXSTRIDE 64        /* Strides range from 1 to 64 elems */
//#define MAXELEMS MAXBYTES/sizeof(double)
#define MAXELEMS 4097*4097
#define ROW 4097
#define COLUMN 4097

/* $begin mountainfuns */
double data[MAXELEMS];      /* The global array we'll be traversing */
//float data[MAXELEMS];      /* The global array we'll be traversing */

void addTwoMatrix(double* A, double* B, double* result, int row_A, int col_A, int row_B, int col_B);
void subtractTwoMatrix(double* A, double* B, double* result, int row_A, int col_A, int row_B, int col_B);
void multiply(int m1, int m2, int mat1[][m2],
              int n1, int n2, int mat2[][n2], int res[m1][n2]);
double rand_double_from(double min, double max);
double rand_float_from(double min, double max);
void store_double_data_to_csv(char* filename, int row, int col);
void read_double_data_from_csv(char* filename);
void store_float_data_to_csv(char* filename, int row, int col);
void read_float_data_from_csv(char* filename);
/* $mountainfuns */
void init_data(double *data, long n);
void test(int elems, int stride);
double run(int size, int stride, double Mhz);

/* $begin mountainmain */
int main()
{
    int size;        /* Working set size (in bytes) */
    int stride;      /* Stride (in array elements) */
    double Mhz;      /* Clock frequency */

//    store_double_data_to_csv("double_data.csv", ROW, COLUMN); /* Store double data into a csv file */
//    store_float_data_to_csv("float_data.csv", ROW, COLUMN); /* Store float data into a csv file */

//    read_double_data_from_csv("double_data.csv"); /* Load data from a csv file */
//    FILE *file = fopen("double_result.csv", "w+");
//    FILE *f = fopen("graph_double_data.csv", "w+");

    read_float_data_from_csv("float_data.csv"); /* Load data from a csv file */
    FILE *file = fopen("float_result.csv", "w+");
    FILE *f = fopen("graph_float_data.csv", "w+");



//    init_data(data, MAXELEMS); /* Initialize each element in data */
    Mhz = mhz(0);              /* Estimate the clock frequency */

            /* Testing for matrix computation */
//    double A[] = {1, 2, 3, 4, 6, 6};
//    double B[] = {1, 1, 1, 1, 2, 2};
//    double sum[6];
//
//    addTwoMatrix(A, B, sum, 3, 2, 3, 2);
//
//    for (int i = 0; i < 6; i++) {
//        printf("%lf\n", sum[i]);
//    }
//
//    printf("\n");
//
//    subtractTwoMatrix(A, B, sum, 3, 2, 3, 2);
//
//    for (int i = 0; i < 6; i++) {
//        printf("%lf\n", sum[i]);
//    }

//    int mat1[][2] = { { 2, 4 }, { 3, 4 } };
//    int mat2[][2] = { { 1, 2 }, { 1, 3 } };
//    int m1 = 2, m2 = 2, n1 = 2, n2 = 2;
//    int res[m1][n2];
//    multiply(m1, m2, mat1, n1, n2, mat2, res);
//
//    for (int i = 0; i < m1; i++) {
//        for (int j = 0; j < m2; j++) {
//            printf(res[i][j]);
//        }
//        printf("\n");
//    }

    if (file == NULL)
    {
        printf("Open file error!");
        exit(1);
    }

    /* Shown in the text */
    fprintf(file, "Clock frequency is approx. %.1f MHz\n", Mhz);
    fprintf(file, "Memory mountain (MB/sec)\n");
    fprintf(file, "Max Elements: %ld\n", MAXELEMS);

    fprintf(file, "\t");
    for (stride = 1; stride <= MAXSTRIDE; stride++)
        fprintf(file, "s%d\t", stride);
    fprintf(file, "\n");

    for (size = MAXBYTES; size >= MINBYTES; size >>= 1) {
        if (size > (1 << 20))
            fprintf(file, "%dm\t", size / (1 << 20));
        else
            fprintf(file, "%dk\t", size / 1024);

        for (stride = 1; stride <= MAXSTRIDE; stride++) {
            double value = run(size, stride, Mhz);
            fprintf(file, "%.1f\t", value);
            fprintf(f, "%.1f\t", value);

        }
        fprintf(file, "\n");
        fprintf(f, "\n");
    }

    fclose(file);
    fclose(f);

    exit(0);
}
/* Matrix add */
void addTwoMatrix(double* A, double* B, double* result, int row_A, int col_A, int row_B, int col_B)
{
    if (row_A != row_B || col_A != col_B) {
        printf("Either column or row between Matrix A and Matrix B is not the same.");
        exit(1);
    }

    for (int i = 0; i < row_A; i++) {
        for (int j = 0; j < col_A; j++) {
            result[i*col_A + j] = A[i*col_A + j] + B[i*col_A + j];
        }
    }
}

/* Matrix subtract */
void subtractTwoMatrix(double* A, double* B, double* result, int row_A, int col_A, int row_B, int col_B)
{
    if (row_A != row_B || col_A != col_B) {
        printf("Either column or row between Matrix A and Matrix B is not the same.");
        exit(1);
    }

    for (int i = 0; i < row_A; i++) {
        for (int j = 0; j < col_A; j++) {
            result[i*col_A + j] = A[i*col_A + j] - B[i*col_A + j];
        }
    }
}

/* multiply */
void multiply(int m1, int m2, int mat1[][m2],
              int n1, int n2, int mat2[][n2], int res[m1][n2])
{
    int x, i, j;
    for (i = 0; i < m1; i++) {
        for (j = 0; j < n2; j++) {
            res[i][j] = 0;
            for (x = 0; x < m2; x++) {
                *(*(res + i) + j) += *(*(mat1 + i) + x) *
                                     *(*(mat2 + x) + j);
            }
        }
    }
    for (i = 0; i < m1; i++) {
        for (j = 0; j < n2; j++) {
            printf("%d ", *(*(res + i) + j));
        }
        printf("\n");
    }
}

/* generate a random double number from min to max */
double rand_double_from(double min, double max)
{
    static int num = -1;
    if((num = (num < 0)))
        srand(time(NULL)+getpid());

    return min + (double)rand() / ((double)RAND_MAX / (max - min));
}

/* generate a random floating number from min to max */
double rand_float_from(double min, double max)
{
    static int num = -1;
    if((num = (num < 0)))
        srand(time(NULL)+getpid());

    return min + (double)rand() / ((double)RAND_MAX / (max - min));
}

void store_double_data_to_csv(char* filename, int row, int col)
{
	FILE *file = fopen(filename, "w+");

	if (file == NULL)
	{
		printf("Open file error!");
		exit(1);
	}

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			fprintf(file, "%lf\t", rand_double_from(1.0, 100.0));
		}
		fprintf(file, "\n");
	}

    fclose(file);
}

void store_float_data_to_csv(char* filename, int row, int col)
{
    FILE *file = fopen(filename, "w+");

    if (file == NULL)
    {
        printf("Open file error!");
        exit(1);
    }

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            fprintf(file, "%f\t", rand_float_from(1.0, 100.0));
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void read_double_data_from_csv(char* filename)
{
    FILE *file = fopen(filename, "r");

    int i = 0;
    int c;

    if (file == NULL)
    {
        printf("Open file error!");
        exit(1);
    }

    do{
        fscanf(file,"%lf",&data[i]); //Storing the number into the array
        i++;
    }while(c=getc(file)!=EOF);

    // close file
    fclose(file);
}

void read_float_data_from_csv(char* filename)
{
    FILE *file = fopen(filename, "r");

    int i = 0;
    int c;

    if (file == NULL)
    {
        printf("Open file error!");
        exit(1);
    }

    do{
        fscanf(file,"%f",&data[i]); //Storing the number into the array
        i++;
    }while(c=getc(file)!=EOF);

    // close file
    fclose(file);
}
/* $end mountainmain */

/* init_data - initializes the array */
void init_data(double *data, long n)
{
    int i;
    
    for (i = 0; i < n; i++)
        data[i] = i;
}

/* $begin mountainfuns */
/*
 * test - Iterate over first "elems" elements of array "data"
 *        with stride of "stride".
 */
void test(int elems, int stride) /* The test function */
{
    int i;
    double result = 0.0;
    volatile double sink;
    
    for (i = 0; i < elems; i += stride) {
        result += data[i];
    }
    sink = result; /* So compiler doesn't optimize away the loop */
}

/*
 * run - Run test(elems, stride) and return read throughput (MB/s).
 *       "size" is in bytes, "stride" is in array elements, and
 *       Mhz is CPU clock frequency in Mhz.
 */
double run(int size, int stride, double Mhz)
{
    double cycles;
    int elems = size / sizeof(double);
    
    test(elems, stride);                     /* warm up the cache */       //line:mem:warmup
    cycles = fcyc2(test, elems, stride, 0);  /* call test(elems,stride) */ //line:mem:fcyc
    return (size / stride) / (cycles / Mhz); /* convert cycles to MB/s */  //line:mem:bwcompute
}
/* $end mountainfuns */

