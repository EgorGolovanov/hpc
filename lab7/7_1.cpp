#include <stdio.h>
#include <iostream>
#include <omp.h>

const int N = 10;

int main(void) {
	int i;
	double x[N], y[N];
	x[0] = 0; //деление на ноль в массиве y

#pragma omp parallel 
	{
#pragma omp for nowait
		for (i = 1; i < N; i++) {
			x[i] = x[i - 1] * x[i - 1]; //проблема гонок, x[i] (использование x[i-1])
			y[i] = x[i] / y[i - 1]; //проблема гонок, y[i] (использование y[i-1])
		}
	}
	y[0] = x[N - 1];

	for (i = 1; i < N; i++) {
		std::cout << x[i] << y[i] << std::endl;
	}

	return 0;

}
