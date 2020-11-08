// lab7.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <math.h>
#include <chrono>
#include <omp.h>
#include <algorithm>
#include <vector>

/*
Вычисление определителя матрицы размерностью 3 на 3
*/
double minor_det(std::vector<std::vector<double>> matr)
{
	return matr[0][0] * matr[1][1] * matr[2][2] - matr[0][0] * matr[1][2] * matr[2][1]
		- matr[0][1] * matr[1][0] * matr[2][2] + matr[0][1] * matr[1][2] * matr[2][0]
		+ matr[0][2] * matr[1][0] * matr[2][1] - matr[0][2] * matr[1][1] * matr[2][0];
}

/*
Вычисление определителя минора
*/
double minor(std::vector<std::vector<double>> matrix, int i, int j)
{
	matrix.erase(matrix.begin() + i);
	for (auto& line : matrix)
	{
		line.erase(line.begin() + j);
	}
	double result = minor_det(matrix);
	return result;
}
/*
Вычисление определителя матрицы размерностью 4 на 4
*/
double det(std::vector<std::vector<double> > matrix, int n)
{
	double det = 0;
	int j = 1;

	for (int i = 1; i <= n; i++)
		det += std::pow(-1, i + j) * minor(matrix, i - 1, j - 1) * matrix[i - 1][j - 1];
	return det;
}

/*
Замена столбцов матрицы
*/
std::vector<std::vector<double>> swap_column(std::vector<std::vector<double>> private_matrix, std::vector<double> free_term_column, int i, int n)
{
	for (int cnt = 0; cnt < n; cnt++) {
		private_matrix[cnt][i] = free_term_column[cnt];
	}
	return private_matrix;
}
/*
Метод Крамера
*/
std::vector<double> cramer_solving(std::vector<std::vector<double> >& matrix, std::vector<double>& free_term_column, int n) {
	double mainDet = det(matrix, n);
	if (std::abs(mainDet) < 0.0001 || mainDet == 0.0)
		std::cerr << "Divide by zero exception!";

	std::vector<double> solution(n);
	int i = 0;
#pragma omp parallel for private(i) shared(matrix)
		for (i = 0; i < n; ++i) {			
			double localDet = det(swap_column(matrix, free_term_column, i, n), n);
#pragma omp critical
			solution[i] = localDet / mainDet;
		}
	return solution;
}

int main()
{

	if (omp_get_max_threads() < 4)
	{
		std::cout << "мало нитей";
		return(0);
	}
	else omp_set_num_threads(4);
	
	int n = 4;
	std::vector<std::vector<double> > matrix(n);
	
	for (int i = 0; i < n; ++i) {
		matrix[i].resize(n);
		for (int j = 0; j < n; ++j)
		{
			std::cout << "a[" << i + 1 << "][" << j + 1 << "] = ";
			std::cin >> matrix[i][j];
		}
	}
	
	std::vector<double> column(n);
	
	for (int j = 0; j < n; ++j)
	{
		std::cout << "b[" << j + 1 << "] = ";
		std::cin >> column[j];
	}

	int start_time = omp_get_wtime();
	
	std::vector<double> solution = cramer_solving(matrix, column, n);
	
	std::cout << "time(ms) = " << omp_get_wtime() - start_time << std::endl;
	for (int j = 0; j < n; ++j)
	{
		std::cout << "c[" << j + 1 << "] = " << solution[j] << std::endl;
	}
}
