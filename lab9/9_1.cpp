// 9_1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <math.h>
#include <mpi.h>
#include <algorithm>
#include <ctime>

int main(int argc, char* argv[])
{
	int proc_rank, proc_num, vote;
	long long N;
	double sum = 0;
	int* arr = NULL;
	MPI_Status st;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

	double start, end;

	arr = new int[proc_num];
	for (int cnt = 0; cnt < proc_num; cnt++)
		arr[cnt] = 0;

	srand(time(NULL) + proc_rank);
	start = MPI_Wtime();

	vote = (rand() % proc_num);

	//  председатель 
	if (proc_rank == 0)
	{
		int votes;

		for (int i = 1; i < proc_num; ++i)
		{
			MPI_Recv(&votes, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
			arr[votes]++;
		}
		arr[vote]++;
		end = MPI_Wtime();
		std::cout << "time to sum = " << end - start << "s\n";
		// вывод распределения голосов
		for (int g = 0; g < proc_num; g++) {
			std::cout << "arr[" << g << "] = " << arr[g] << std::endl;
		}
		std::cout << "Winner is: " << std::distance(arr, std::max_element(arr, arr + proc_num)) << std::endl;
	}// магистры
	else
	{
		for (int i = 1; i < proc_num; ++i)
		{
			if (i != proc_rank)
				MPI_Send(&vote, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	delete[] arr;
	return 0;
}
