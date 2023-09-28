#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void Allocate_vectors(double** local_x_pp, double** local_y_pp, double** local_z_pp, int local_n, MPI_Comm comm);
void Print_partial_vector(double local_b[], int local_n, int n, char title[], int my_rank, MPI_Comm comm, int num_elements);
void Parallel_vector_sum(double local_x[], double local_y[], double local_z[], int local_n);
double Dot_product(double local_x[], double local_y[], int local_n);
void Scalar_vector_product(double local_x[], double scalar, int local_n);
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

   n = 100000; // Cambia el tamaño del vector según tus necesidades
   local_n = n / comm_sz; // Tamaño local de cada proceso

   Allocate_vectors(&local_x, &local_y, &local_z, local_n, comm);

   Randomly_fill_vector(local_x, local_n);
   Randomly_fill_vector(local_y, local_n);

   start_time = MPI_Wtime(); // Inicia la medición del tiempo

   // Calcula el producto punto de local_x y local_y
   double dot_product = Dot_product(local_x, local_y, local_n);

   // Calcula el producto de un escalar por local_x y local_y
   double scalar = 2.0; // Puedes cambiar el escalar según tus necesidades
   Scalar_vector_product(local_x, scalar, local_n);
   Scalar_vector_product(local_y, scalar, local_n);

   // Calcula la suma de local_x y local_y y almacena el resultado en local_z
   Parallel_vector_sum(local_x, local_y, local_z, local_n);

   end_time = MPI_Wtime(); // Finaliza la medición del tiempo

   // Imprime el resultado del producto punto
   if (my_rank == 0) {
       printf("Producto punto: %f\n", dot_product);
   }

   // Imprime los primeros 10 elementos del vector resultante local_z
   Print_partial_vector(local_z, local_n, n, "Primeros 10 de local_z", my_rank, comm, 10);

   // Imprime los últimos 10 elementos del vector resultante local_z
   Print_partial_vector(local_z + n - 10, local_n, n, "Ultimos 10 de local_z", my_rank, comm, 10);

   // Imprime el tiempo de ejecución
   if (my_rank == 0) {
       printf("Tiempo de ejecución: %f segundos\n", end_time - start_time);
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
    // Inicializa la semilla para generar números aleatorios
    srand(time(NULL));

    for (int i = 0; i < local_n; i++) {
        local_a[i] = (double)rand() / RAND_MAX; // Números aleatorios entre 0 y 1
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

    double global_dot_product;
    MPI_Allreduce(&local_dot_product, &global_dot_product, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    return global_dot_product;
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
