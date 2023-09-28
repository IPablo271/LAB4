/* File:     vector_add.c
 *
 * Purpose:  Implement vector addition
 *
 * Compile:  gcc -g -Wall -o vector_add vector_add.c
 * Run:      ./vector_add
 *
 * Input:    The order of the vectors, n, and the vectors x and y
 * Output:   The sum vector z = x+y
 *
 * Note:
 *    If the program detects an error (order of vector <= 0 or malloc
 * failure), it prints a message and terminates
 *
 * IPP:      Section 3.4.6 (p. 109)
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

void Read_n(int* n_p);
void Allocate_vectors(double** x_pp, double** y_pp, double** z_pp, int n);
void Print_partial_vector(double b[], int n, char title[], int num_elements);
void Read_vector(double a[], int n, char vec_name[]);
void Print_vector(double b[], int n, char title[]);
void Vector_sum(double x[], double y[], double z[], int n);

/*---------------------------------------------------------------------*/
int main(void) {
   int n = 100000; // Cambia el tamaño del vector según tus necesidades
   double *x, *y, *z;
   srand(time(NULL)); // Inicializa la semilla del generador de números aleatorios
   clock_t start_time, end_time;

   Allocate_vectors(&x, &y, &z, n);

   // Llena los vectores x e y con números aleatorios
   for (int i = 0; i < n; i++) {
       x[i] = (double)rand() / RAND_MAX; // Números aleatorios entre 0 y 1
       y[i] = (double)rand() / RAND_MAX;
   }

   // Imprime los primeros y últimos 10 elementos de los vectores x e y
   Print_partial_vector(x, n, "Primeros 10 de x", 10);
   Print_partial_vector(y, n, "Primeros 10 de y", 10);
   Print_partial_vector(x + n - 10, n, "Ultimos 10 de de x", 10);
   Print_partial_vector(y + n - 10, n, "Ultimos 10 de de y", 10);

   start_time = clock(); // Inicia la medición del tiempo

   Vector_sum(x, y, z, n);

   end_time = clock(); // Finaliza la medición del tiempo

   // Imprime los primeros y últimos 10 elementos del vector resultante z
   Print_partial_vector(z, n, "Primeros 10 de z", 10);
   Print_partial_vector(z + n - 10, n, "Ultimos 10 de de z", 10);

   // Calcula el tiempo de ejecución en segundos
   double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
   printf("Tiempo de ejecución: %f segundos\n", execution_time);

   free(x);
   free(y);
   free(z);

   return 0;
} /* main */

/*---------------------------------------------------------------------
 * Function:  Read_n
 * Purpose:   Get the order of the vectors from stdin
 * Out arg:   n_p:  the order of the vectors
 *
 * Errors:    If n <= 0, the program terminates
 */
void Read_n(int* n_p /* out */) {
   printf("What's the order of the vectors?\n");
   scanf("%d", n_p);
   if (*n_p <= 0) {
      fprintf(stderr, "Order should be positive\n");
      exit(-1);
   }
}  /* Read_n */

/*---------------------------------------------------------------------
 * Function:  Allocate_vectors
 * Purpose:   Allocate storage for the vectors
 * In arg:    n:  the order of the vectors
 * Out args:  x_pp, y_pp, z_pp:  pointers to storage for the vectors
 *
 * Errors:    If one of the mallocs fails, the program terminates
 */
void Allocate_vectors(double** x_pp, double** y_pp, double** z_pp, int n) {
    *x_pp = malloc(n * sizeof(double));
    *y_pp = malloc(n * sizeof(double));
    *z_pp = malloc(n * sizeof(double));
    if (*x_pp == NULL || *y_pp == NULL || *z_pp == NULL) {
        fprintf(stderr, "Can't allocate vectors\n");
        exit(-1);
    }
}  /* Allocate_vectors */

/*---------------------------------------------------------------------
 * Function:  Read_vector
 * Purpose:   Read a vector from stdin
 * In args:   n:  order of the vector
 *            vec_name:  name of vector (e.g., x)
 * Out arg:   a:  the vector to be read in
 */
void Read_vector(
      double  a[]         /* out */, 
      int     n           /* in  */, 
      char    vec_name[]  /* in  */) {
   int i;
   printf("Enter the vector %s\n", vec_name);
   for (i = 0; i < n; i++)
      scanf("%lf", &a[i]);
}  /* Read_vector */  

/*---------------------------------------------------------------------
 * Function:  Print_vector
 * Purpose:   Print the contents of a vector
 * In args:   b:  the vector to be printed
 *            n:  the order of the vector
 *            title:  title for print out
 */
void Print_vector(
      double  b[]     /* in */, 
      int     n       /* in */, 
      char    title[] /* in */) {
   int i;
   printf("%s\n", title);
   for (i = 0; i < n; i++)
      printf("%f ", b[i]);
   printf("\n");
}  /* Print_vector */

/*---------------------------------------------------------------------
 * Function:  Vector_sum
 * Purpose:   Add two vectors
 * In args:   x:  the first vector to be added
 *            y:  the second vector to be added
 *            n:  the order of the vectors
 * Out arg:   z:  the sum vector
 */
void Vector_sum(
      double  x[]  /* in  */, 
      double  y[]  /* in  */, 
      double  z[]  /* out */, 
      int     n    /* in  */) {
   int i;

   for (i = 0; i < n; i++)
      z[i] = x[i] + y[i];
}  /* Vector_sum */


void Print_partial_vector(double b[], int n, char title[], int num_elements) {
    printf("%s\n", title);
    for (int i = 0; i < num_elements; i++) {
        printf("%f ", b[i]);
    }
    printf("\n");
}