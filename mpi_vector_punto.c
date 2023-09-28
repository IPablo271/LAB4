#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void Allocate_vectors(double** local_x_pp, double** local_y_pp, double** local_z_pp, int local_n, MPI_Comm comm);
void Print_partial_vector(double local_b[], int local_n, int n, char title[], int my_rank, MPI_Comm comm, int num_elements);
void Parallel_vector_sum(double local_x[], double local_y[], double local_z[], int local_n);
double Dot_product(double local_x[], double local_y[], int local_n);
void Scalar_vector_product(double local_x[], double scalar, int local_n);
void Randomly_fill_vector(double local_a[], int local_n); 
void Check_for_error(int local_ok, char fname[], char message[], MPI_Comm comm);


int main(void) {
   int n, local_n;
   int comm_sz, my_rank;
   double *local_x, *local_y, *local_z;
   MPI_Comm comm;
   double start_time, end_time;

   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &my_rank);

   srand(time(NULL));

   n = 4; 
   double scalar = 10.0;
   local_n = n;

   Allocate_vectors(&local_x, &local_y, &local_z, local_n, comm);

   Randomly_fill_vector(local_x, local_n);
   Randomly_fill_vector(local_y, local_n);

   start_time = MPI_Wtime(); // Inicia la medición del tiempo

   if (my_rank == 0) {
       printf("Elementos originales de local_x : ");
       for (int i = 0; i < n; i++) {
           printf("%f ", local_x[i]);
       }
       printf("\n");

       // Imprime los Elementos originales de local_y
       printf("Elementos originales de local_y : ");
       for (int i = 0; i < n; i++) {
           printf("%f ", local_y[i]);
       }
       printf("\n");
   }

   // Calcula el producto punto de local_x y local_y
   double dot_product = Dot_product(local_x, local_y, n);

   // Calcula el producto de un escalar por local_x y local_y
   Scalar_vector_product(local_x, scalar, local_n);
   Scalar_vector_product(local_y, scalar, local_n);

   end_time = MPI_Wtime(); // Finaliza la medición del tiempo

   // Imprime los resultados de la multiplicación escalar
   if (my_rank == 0) {

       printf("\nProducto punto: %f\n", dot_product);

       printf("\nResultado de la multiplicación escalar:\n");

       // Imprime los primeros 10 elementos de local_x
       printf("Elementos modificados de local_x: ");
       for (int i = 0; i < n; i++) {
           printf("%f ", local_x[i]);
       }
       printf("\n");

       // Imprime los Elementos de local_y
       printf("Elementos modificados de local_y: ");
       for (int i = 0; i < n; i++) {
           printf("%f ", local_y[i]);
       }
       printf("\n");

       // Imprime el tiempo de ejecución
       printf("\nTiempo de ejecución: %f segundos\n", end_time - start_time);
   }

   free(local_x);
   free(local_y);
   free(local_z);

   MPI_Finalize();

   return 0;
}


void Allocate_vectors(double** local_x_pp, double** local_y_pp, double** local_z_pp, int local_n, MPI_Comm comm) {
    int local_ok = 1;
    char* fname = "Allocate_vectors";

    *local_x_pp = malloc(local_n * sizeof(double));
    *local_y_pp = malloc(local_n * sizeof(double));
    *local_z_pp = malloc(local_n * sizeof(double));

    if (*local_x_pp == NULL || *local_y_pp == NULL || *local_z_pp == NULL) local_ok = 0;
    Check_for_error(local_ok, fname, "Can't allocate local vector(s)", comm);
}

void Randomly_fill_vector(double local_a[], int local_n) {
    for (int i = 0; i < local_n; i++) {
        do {
            local_a[i] = ((double)rand() / ((double)RAND_MAX + 1)) + 0.1; // Números aleatorios entre 0.1 y 1.0
        } while (local_a[i] == 0);
    }
}



void Print_partial_vector(double local_b[], int local_n, int n, char title[], int my_rank, MPI_Comm comm, int num_elements) {
    if (my_rank == 0) {
        printf("%s:\n", title);
        int start = 0;
        int end = n;

        // Imprime los primeros 10 elementos
        for (int i = start; i < start + num_elements; i++) {
            if (i < end) {
                printf("%f ", local_b[i]);
            }
        }
        printf("\n");
    }
}

void Parallel_vector_sum(double local_x[], double local_y[], double local_z[], int local_n) {
    for (int local_i = 0; local_i < local_n; local_i++)
        local_z[local_i] = local_x[local_i] + local_y[local_i];
}

double Dot_product(double local_x[], double local_y[], int local_n) {
    double local_dot_product = 0.0;
    for (int local_i = 0; local_i < local_n; local_i++) {
        local_dot_product += local_x[local_i] * local_y[local_i];
    }

    // printf("Local dot product: %f\n", local_dot_product);

    double global_dot_product;
    MPI_Allreduce(&local_dot_product, &global_dot_product, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    return local_dot_product;
}


void Scalar_vector_product(double local_x[], double scalar, int local_n) {
    for (int local_i = 0; local_i < local_n; local_i++) {
        local_x[local_i] *= scalar;
    }
}

void Check_for_error(int local_ok, char fname[], char message[], MPI_Comm comm) {
    int ok;

    MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
    if (ok == 0) {
        int my_rank;
        MPI_Comm_rank(comm, &my_rank);
        if (my_rank == 0) {
            fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, message);
            fflush(stderr);
        }
        MPI_Finalize();
        exit(-1);
    }
}
