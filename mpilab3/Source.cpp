#define _CRT_SECURE_NO_WARNINGS
#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <algorithm>
#include <math.h>

using namespace std;

void integral(const double a1, const double b1, const double a2,
	const double b2, const double h, double *res)
{
	double sum;       
	double midval;

	//вычисление интеграла
	sum = 0.0;

	for (double i = a1; i < b1; i += h)
	{
		for (double j = a2; j <b2; j += h)
		{
			midval = ((sin(i * j) * cos(i * j)) +
				(sin((i + h) * j) * cos((i + h) * j)) +
				(sin(i * (j + h)) * cos(i * (j + h))) +
				(sin((i + h) * (j + h)) * cos((i + h) * (j + h)))) / 4;
			sum += midval * h * h;
		}
	}
	*res = sum;
}

double f(double x)
{
	return 1;
}

double integral_ (const double a1, const double b1, const double h)
{
	double sum = 0;
	double hl = 0, hr = 0;

	for (double i = a1; i < b1; i += h)
	{
		hl = f(i);
		hr = f(i + h);
		sum += (hl + hr) / 2 * h;
	}
	return sum;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

int main(int argc, char *argv[])
{
	int procRank, procNum;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
	double starttime, endtime;

	double a1 = 0.0;    
	double b1 = 16.0;  
	double a2 = 0.0;   
	double b2 = 16.0;    
	double h = 0.1;   
	double step;
	double res = 0.0;	  
	double *ressivedres; 

	if (cmdOptionExists(argv, argv + argc, "-a1"))
	{
		char * wcount = getCmdOption(argv, argv + argc, "-a1");
		a1 = atof(wcount);
	}

	if (cmdOptionExists(argv, argv + argc, "-b1"))
	{
		char * wcount = getCmdOption(argv, argv + argc, "-b1");
		b1 = atof(wcount);
	}

	if (cmdOptionExists(argv, argv + argc, "-a2"))
	{
		char * wcount = getCmdOption(argv, argv + argc, "-a2");
		a2 = atof(wcount);
	}

	if (cmdOptionExists(argv, argv + argc, "-b2"))
	{
		char * wcount = getCmdOption(argv, argv + argc, "-b2");
		b2 = atof(wcount);
	}

	if (cmdOptionExists(argv, argv + argc, "-h"))
	{
		char * wcount = getCmdOption(argv, argv + argc, "-h");
		h = atof(wcount);
	}

	if (procRank == 0)
	{
		cout << "a1 = " << a1 << endl;
		cout << "b1 = " << b1 << endl;
		cout << "h = " << h << endl;

	}
	step = (b1 - a1) / procNum;
						  //Время
	if (procRank == 0)
		starttime = MPI_Wtime();

	double *X;
	X = new double[2 * procNum];


	//if (procRank == 0)
	//{
	//	double tmp = (b1 - a1) / (double)procNum;

	//	tmp = round(tmp * 1000) / 1000;
	//	X[2] = a1;
	//	X[3] = a1 + tmp;

	//	for (int i = 4; i < procNum * 2; i += 2)
	//	{
	//		X[i] = X[i - 1];
	//		X[i + 1] = X[i] + tmp;
	//	}
	//	X[0] = X[procNum * 2 - 1];
	//	X[1] = b1;
	//}


	double X_loc[2];
	MPI_Scatter(X, 2, MPI_DOUBLE,
		X_loc, 2, MPI_DOUBLE,
		0, MPI_COMM_WORLD);


	//integral(a1, b1, X_loc[0], X_loc[1], h, &res);
	res = integral_(procRank * step, procRank * step + step, h);

	//Собираем результат
	if (procRank == 0) {
		ressivedres = new double[procNum];
	}

	MPI_Gather(&res, 1, MPI_DOUBLE,
		ressivedres, 1, MPI_DOUBLE,
		0, MPI_COMM_WORLD);

	if (procRank == 0)
	{
		endtime = MPI_Wtime();
		for (int i = 1; i < procNum; i++)
			res += ressivedres[i];
		res -= h;
		std::cout << "res: " << res << " Parallel time for " << procNum << ": " << endtime - starttime << "\n";
	}

	MPI_Finalize();

	return 0;
}
