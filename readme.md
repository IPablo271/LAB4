# Compilar el codigo

Codigo secuencial de suma de vectores
```bash
gcc -o add vector_add.c
./add
```

Codigo paralelo de suma de vectores
```bash
mpicc -o mpi mpi_vector_add.c
mpiexec -n 4 ./mpi
```

Codigo del producto punto
```bash
mpicc -o mpi2 mpi_vector_punto.c
mpiexec -n 4 ./mpi2
```