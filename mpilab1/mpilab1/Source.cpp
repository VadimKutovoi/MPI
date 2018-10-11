#include "mpi.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[])
{
	int rank, size, rows = 3, columns = 3;
	int **matrix, *recv_row, *sum;
	MPI_Status status;

	matrix = new int*[rows];
	sum = new int[rows];

	for (int i = 0; i < rows; i++) {
		matrix[i] = new int[columns];
		for (int j = 0; j < columns; j++)
			matrix[i][j] = i + j;
	}

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (int i = 0; i < rows; i++) {
			if (i%size != 0)
				MPI_Send(matrix[i], columns, MPI_INT, i%size, i, MPI_COMM_WORLD);
		}
	}
	
	if(rank != 0) {
		recv_row = new int[columns];

		MPI_Recv(recv_row, columns, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
		printf("Process %i received row from process 0\n", rank);

		int row_sum = 0;

		for (int i = 0; i < columns; i++)
			row_sum += recv_row[i];
		
		/*MPI_Send(&row_sum, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);*/
		printf("Process %i send sum = %i ", rank, row_sum);
	}

	MPI_Finalize();

	for (int i = 0; i < rows; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
}