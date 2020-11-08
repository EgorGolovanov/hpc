// lab1011.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <iostream>
#include <math.h>
#include <locale.h>
#include <conio.h>
#include <mpi.h> 
#include <tchar.h>
#include <cstdlib>

const int n = 500;

int get_chunk(int total, int commsize, int rank)
{
	int n = total;
	int q = n / commsize;
	if (n % commsize)
		q++;
	int r = commsize * q - n;
	/* Вычисляем кол-во строк на поток */
	int chunk = q;
	if (rank >= commsize - r)
		chunk = q - 1;
	return chunk;
}

void gauss() {
	int proc_rank, proc_num;

	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

	double t = MPI_Wtime();

	//получаем кол-во строк на поток
	int nrows = get_chunk(n, proc_num, proc_rank);
	std::cout << nrows << std::endl;

	int* rows = (int*)malloc(sizeof(*rows) * nrows);
	// Номера локальных строк
	// Матрица дополнена столбцом для вектора b
	double* a = (double*)malloc(sizeof(*a) * nrows * (n + 1));
	double* x = (double*)malloc(sizeof(*x) * n);
	double* tmp = (double*)malloc(sizeof(*tmp) * (n + 1));

	// Инициализация
	for (int i = 0; i < nrows; i++)
	{
		rows[i] = proc_rank + proc_num * i;
		srand(rows[i] * (n + 1));
		for (int j = 0; j < n; j++)
			a[i * (n + 1) + j] = rand() % 100 + 1;
		// b[i]
		a[i * (n + 1) + n] = rand() % 100 + 1;
	}

	// Прямой ход
	int row = 0;
	for (int i = 0; i < n - 1; i++)
	{
		// Исключаем x[i]
		if (i == rows[row])
		{
			// Рассылаем строку i, находящуюся в памяти текущего процесса
			MPI_Bcast(&a[row * (n + 1)], n + 1, MPI_DOUBLE, proc_rank, MPI_COMM_WORLD);
			for (int j = 0; j <= n; j++)
				tmp[j] = a[row * (n + 1) + j];
			row++;
		}
		else
		{
			MPI_Bcast(tmp, n + 1, MPI_DOUBLE, i % proc_num, MPI_COMM_WORLD);
		}
		// Вычитаем принятую строку из уравнений, хранящихся в текущем процессе
		for (int j = row; j < nrows; j++)
		{
			double scaling = a[j * (n + 1) + i] / tmp[i];
			for (int k = i; k < n + 1; k++)
				a[j * (n + 1) + k] -= scaling * tmp[k];
		}
	}
	// Инициализация неизвестных
	row = 0;
	for (int i = 0; i < n; i++)
	{
		x[i] = 0;
		if (i == rows[row])
		{
			x[i] = a[row * (n + 1) + n];
			row++;
		}
	}
	// Обратный ход 
	row = nrows - 1;
	for (int i = n - 1; i > 0; i--)
	{
		if (row >= 0)
		{
			if (i == rows[row])
			{
				x[i] /= a[row * (n + 1) + i];
				// Передаем найденное x[i]
				MPI_Bcast(&x[i], 1, MPI_DOUBLE, proc_rank, MPI_COMM_WORLD);
				row--;
			}
			else
				MPI_Bcast(&x[i], 1, MPI_DOUBLE, i % proc_num, MPI_COMM_WORLD);
		}
		else
			MPI_Bcast(&x[i], 1, MPI_DOUBLE, i % proc_num, MPI_COMM_WORLD);
		for (int j = 0; j <= row; j++)
			// Корректировка локальных x[i]
			x[rows[j]] -= a[j * (n + 1) + i] * x[i];
	}
	if (proc_rank == 0)
		x[0] /= a[row * (n + 1)];
	// Корректировка x[0]
	MPI_Bcast(x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	free(tmp);
	free(rows);
	free(a);
	t = MPI_Wtime() - t;

	if (proc_rank == 0)
	{
		printf("Parallel Gauss: n = %d, procs = %d, time (sec) = %.6f\n", n, proc_num, t);
	}

	free(x);
}


int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);
	gauss();
	MPI_Finalize();

	return 0;
}