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

	int rank, size, rows = 5, columns = 5;
	double *matrix = NULL, *recv_row = NULL, *sum = NULL;
	double times;
	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		std::cout << "Comm size = " << size << std::endl;
		
		std::cout << "Rows = ";
		std::cin >> rows; 
		
		std::cout << "Columns = ";
		std::cin >> columns;

		std::cout << "Generating matrix.." << std::endl;

		matrix = new double[columns * rows];

		generate_matr(matrix, rows, columns);
		print_matr(matrix, rows, columns);

		times = MPI_Wtime();
	}

	
	
	//MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&matrix, columns * rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//изменить матрицу на массив, инт на дабл, передалать сенд.
	if (rank == 0) {
		for (int i = 0; i < rows; i++) {
			if (i%size != 0)
				MPI_Send(matrix, columns, MPI_INT, i%size, i, MPI_COMM_WORLD);
		}
	}
	if(rank != 0) {
		recv_row = new double[columns];
		for (int i = rank; i < rows; i += size) {
			int row_sum = 0;

			MPI_Recv(recv_row, columns, MPI_INT, 0, i, MPI_COMM_WORLD, &status);
			//std::cout << "Process " << rank << " recieved row" << std::endl;

			for (int j = 0; j < columns; j++)
				row_sum += recv_row[j];

			MPI_Send(&row_sum, 1, MPI_INT, 0, i, MPI_COMM_WORLD); //Gather
			//std::cout << "Process " << rank << " sended row" << std::endl;
		}
		delete recv_row;
	}
	else {
		for (int i = rank; i < rows; i += size)
		{
			for (int j = 0; j < columns; j++)
			{
				sum[i] += matrix[i];
			}
		}
	}

	if (rank == 0) {
		for (int i = 1; i < rows; i++) {
			if (i%size != 0) {
				MPI_Recv(&sum[i], 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &status);				
			}
		}

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

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < columns; j++)
			{
				sum[i] += matrix[i][j];
			}
	
		std::cout << "Time = " << MPI_Wtime() - times << std::endl;

		for (int i = 0; i < rows; i++) {
			std::cout << "sum2[" << i << "] = " << sum[i] << std::endl;
		}


		for (int i = 0; i < rows; i++) {
			delete[] matrix[i];
		}
		delete[] matrix, sum;
	}

	MPI_Finalize();
}