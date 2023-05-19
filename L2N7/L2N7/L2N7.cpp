#include  <mpi.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>
#include "L2N7.h"

void input_value(double& value)
{
	std::string str;
	std::cin >> str;
	std::istringstream istr(str);
	if (!(istr >> value)) {
		value = -3;
	}
}

//ввод информации
void input_data(double& x1, double& xn, double& eps, int& n, int procnum)
{
	
	do {
		std::cout << "Input first value: ";
		input_value(x1);
	} while (x1 >= 1 || x1 <= -1);

	std::cout << "\n";
	do {
		std::cout << "Input last value: ";
		input_value(xn);
	} while (xn >= 1 || xn <= -1 || xn < x1);
	std::cout << "\n";

	do {
		std::cout << "Input epsilon: ";
		input_value(eps);
	} while (eps >= 1 || eps <= 0);
	std::cout << "\n";

	do {
		std::cout << "Input n: ";
		std::string str;
		std::cin >> str;
		std::istringstream istr(str);
		if (!(istr >> n)) {
			n = -3;
		}

	} while (n <= 0 || n < procnum);
	std::cout << "\n";
}

//вывод в консоль значения n и epsilon
void print_n_eps(int n, double eps)
{
	std::cout << std::left
		<< std::setw(10)
		<< "|" + std::to_string(n)
		<< std::left
		<< std::setw(10)
		<< "|" + std::to_string(eps);
}

//вывод в консоль строки содержащей значения точек и n и epsilon
void print_x_value(int n, double eps, double* globaldata)
{
	std::cout
		<< std::left
		<< std::setw(20)
		<< "|X Values";
	print_n_eps(n, eps);
	for (int i = 0; i < n; i++)
		std::cout
		<< std::left
		<< std::setw(10)
		<< "|" + std::to_string(globaldata[i]);
	std::cout << std::endl;
}

//вывод в консоль строки содержащей результат, вычесленный с точностью eps
void print_eps_result(int n, double eps, double* resglobal)
{
	std::cout
		<< std::left
		<< std::setw(20)
		<< "|epsilon results";
	print_n_eps(n, eps);
	for (int i = 0; i < n; i++)
		std::cout
		<< std::left
		<< std::setw(10)
		<< "|" + std::to_string(resglobal[i]);
	std::cout << std::endl;
}

//вывод в консоль результирующих значений, вычесленных по формуле заданной в условии задачи 
void print_result_values(int n, double eps, double* res)
{
	std::cout
		<< std::left
		<< std::setw(20)
		<< "|Results";
	print_n_eps(n, eps);
	for (int i = 0; i < n; i++)
		std::cout
		<< std::left
		<< std::setw(10)
		<< "|" + std::to_string(res[i]);
	std::cout << std::endl;
}

//вывод в консоль заголовков столбцов таблицы
void print_header(int n)
{
	std::cout
		<< std::left
		<< std::setw(20)
		<< "|"
		<< std::left
		<< std::setw(10)
		<< "|n"
		<< std::left
		<< std::setw(10)
		<< "|epsilon";
	for (int i = 1; i <= n; i++) {
		std::cout
			<< std::left
			<< std::setw(10)
			<< "|X" + std::to_string(i);
	}
	std::cout << std::endl;
}

void bord(int n)
{
	for (int i = 0; i < 20 + (20 - n) * n; i++)
	{
		std::cout << "_";
	}
	std::cout << std::endl;
}

void print_table(int n, double eps, double* globaldata, double* resglobal, double* res)
{
	bord(n);
	print_header(n);
	bord(n);

	print_x_value(n, eps, globaldata);
	bord(n);

	print_eps_result(n, eps, resglobal);
	bord(n);

	print_result_values(n, eps, res);
	bord(n);
}

//вычисление ряда тэйлора для числа x с точностью eps
double epsilon_taylor(double x, double eps)
{
	double cur = 1;
	double res = 1;
	int i = 1;
	while (abs(cur) >= eps) {
		double num = x * (2 * i - 1);
		double de = 2 * i;
		cur *= (-1) * num / de;
		res += cur;
		i++;
	}
	return res;
}

int main(int argc, char* argv[])
{
	int procnum;
	int myrank;
	double x1;
	double xn;
	int n;
	double eps;
	double* globaldata = NULL;
	double* localdata = NULL;
	double* reslocal = NULL;
	double* resglobal = NULL;
	double* res = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &procnum);

	if (myrank == 0) {
		input_data(x1, xn, eps, n, procnum);

		globaldata = new double[n];
		res = new double[n];
		resglobal = new double[n];

		double step = (xn - x1) / n;
		double tmp = x1;
		for (int i = 0; i < n; i++) {
			globaldata[i] = tmp;
			res[i] = 1 / sqrt(1 + tmp);
			tmp = tmp + step;
		}
	}

	//широковещание, посылаем всем процессам эпсилон и n
	MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//вычисляем сколько данных посылается каждому из процессов
	double k = ceil((double)n / (double)procnum);
	localdata = new double[k];
	reslocal = new double[k];

	//раздача данных всем процессам (отправляем иксы)
	MPI_Scatter(globaldata, k, MPI_DOUBLE, localdata, k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for (int j = 0; j < k; j++) {
		reslocal[j] = epsilon_taylor(localdata[j], eps);
	}

	//собираем данные результирующие
	MPI_Gather(reslocal, k, MPI_DOUBLE, resglobal, k, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	if (myrank == 0) {
		print_table(n, eps, globaldata, resglobal, res);
	}

	MPI_Finalize();
	return 0;
}
