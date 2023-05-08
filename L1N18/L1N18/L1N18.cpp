// L1N18.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
/*18.  аждый процесс обмениваетс€ сообщени€ми со всеми остальными и выводит номера тех процессов,
у которых есть хот€ бы одно значение, 
совпадающее с его собственными значени€ми*/

#include <iostream>
#include <mpi.h>
#include <iomanip>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <functional>
#include "L1N18.h"

using namespace std;


const int m = 5;
int TAG = 0;

bool is_intersects(int  message[m], int  buffer[m])
{
	bool is_ident = false;
	for (int i = 0; i < m && !is_ident; i++) {
		for (int j = 0; j < m && !is_ident; j++) {
			is_ident = message[i] == buffer[j];
		}
	}
	return is_ident;
}

void random_array(int* arr, int len)
{
	std::random_device rnd_device;
	std::mt19937 mersenne_engine(rnd_device());
	std::uniform_int_distribution<int> dist(0, 50);
	for (int i = 0; i < len; i++) {
		arr[i] = dist(mersenne_engine);
	}
}
void print_array(int  message[m])
{
	for (int i = 0;i < m;i++)
		cout << left
		<< setw(10)
		<< message[i] << ' ';
	cout << "\n";
}

int main(int argc, char* argv[])
{
	std::vector<int> vmain;
	int procnum;
	int message[m];
	int myrank;
	MPI_Status status;
	
	bool isfirst = true;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &procnum);

	
	random_array(message, m);
	cout << myrank << " process: ";
	print_array(message);
	
	for (int i = 0; i < procnum; i++) {
		if (i != myrank) {
			MPI_Send(&message, m, MPI_INT, i, TAG, MPI_COMM_WORLD);
		}
	}

	for (int i = 0; i < procnum; i++) {
		if (i != myrank) {
			int buffer[m];
			MPI_Recv(&buffer, m, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);

			if (is_intersects(message, buffer)) {
				cout << i << "\n";
			}
		}
	}
	cout << "\n";

	MPI_Finalize();
}






