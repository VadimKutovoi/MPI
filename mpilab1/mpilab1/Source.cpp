#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_matr(double* matr,int row_c, int column_c) {
	for (int i = 0; i < row_c * column_c; i++) {
		matr[i] = rand() % 10;
	}
}

void print_matr(double* matr, int row_c, int column_c) {
	for (int i = 1; i < row_c * column_c + 1; i++) {
		std::cout << matr[i - 1] << " ";
		if (i%column_c == 0) {
			std::cout << std::endl;
		}
	}
}


void main(int argc, char *argv[])
{
	//parallel program
	MPI_Init(&argc, &argv);

	int rank, size, rows = 5, columns = 5, srows = 1;
	double *matrix = NULL, *recv_row = NULL, *sum = NULL, *lsum = NULL;
	double times;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		if (argc == 3) {
			rows = atoi(argv[1]);
			columns = atoi(argv[2]);
		}
		else {
			rows = columns = size;
		}

		std::cout << "Generating matrix.." << std::endl;

		matrix = new double[columns * rows];
		sum = new double[rows];

		generate_matr(matrix, rows, columns);
		print_matr(matrix, rows, columns);

		srows = rows / size;

		times = MPI_Wtime();
	}

	recv_row = new double[columns];
	
	MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&srows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(matrix, columns * srows, MPI_DOUBLE, recv_row, columns * srows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	lsum = new double[srows];
	
	for (int i = 0; i < srows; i++) lsum[i] = 0;
	
	int i = 0;
	std::cout << rank << " process recieved " << srows << " rows" << std::endl;
	for (int j = 0; j < columns * srows; j++) {
		lsum[i] += recv_row[j];
		if ((j + 1) % columns == 0) i++;
	}
	std::cout << rank << " - rank, sum = " << lsum[0] << std::endl;
	

	MPI_Gather(lsum, srows, MPI_DOUBLE, sum, srows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		std::cout << "Time = " << MPI_Wtime() - times << std::endl;
		for (int i = 0; i < rows; i++) {
			std::cout << "sum[" << i << "] = " << sum[i] << std::endl;
		}
	}

	//sequential program

	if (rank == 0) {
		for (int j = 0; j < columns; j++) sum[j] = 0;

		std::cout << std::endl;
		//sequential program

		times = MPI_Wtime();

		i = 0;
		for (int j = 0; j < columns * rows; j++) {
			sum[i] += matrix[j];
			if ((j + 1) % columns == 0) i++;
		}
	
		std::cout << "Time = " << MPI_Wtime() - times << std::endl;

		for (int i = 0; i < rows; i++) {
			std::cout << "sum2[" << i << "] = " << sum[i] << std::endl;
		}

		delete[] matrix, sum;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	delete[] lsum, recv_row;

	MPI_Finalize();
}