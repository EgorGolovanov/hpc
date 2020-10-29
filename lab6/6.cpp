// lab.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <math.h>
#include <chrono>
#include <omp.h>

int* A;

bool isPrime(int number) {
	unsigned long i;
	if (number == 2)
		return true;
	if (number == 0 || number == 1 || number % 2 == 0)
		return false;
	for (i = 3; i * i <= number && number % i; i += 2)
		;
	return i * i > number;
}

int main()
{
	setlocale(LC_ALL, "russian");
	unsigned long a2, cnt;
	int i;
	double sum = 0.0;
	std::cout << "Введите длину массива: ";
	std::cin >> cnt;
	A = new int[cnt];
	for (int f = 0; f < cnt; f++) {
		A[f] = f + 1;
	}

	if (omp_get_max_threads() < 4)
	{
		printf("мало нитей");
		return(0);
	}
	else omp_set_num_threads(4);
#pragma omp parallel for schedule(static) private(i) shared(A) reduction(+:sum)
	for (i = 0; i < cnt; i++)
	{
#pragma omp critical
		if (isPrime(A[i])) {
			sum += A[i];
		}
	}
	printf("sum = %f\n", sum);
	return(0);
}
