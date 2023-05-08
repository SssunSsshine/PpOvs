/*  Каждый процесс заполняет свой массив размером n случайными числами. 
    Для решения задачи использовать операции приведения с собственной функцией для решения задачи. 
    Результат – вектор размером n, каждый элемент которого получен по правилу определенной в задаче функции.
    6. Найти сумму неравных заданному значению.*/

#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <random>
#include <iomanip>
#include <functional>
#include <string>
#include <sstream>

using namespace std;
void sum(int* invec, int* inoutvec, int* len, MPI_Datatype* dtype)
{
    int i;
    for (i = 0; i < *len - 1; i++)
    {
        if (invec[i] != invec[*len - 1] && inoutvec[i] != invec[*len - 1]) {
            inoutvec[i] = inoutvec[i] + invec[i];
        }
        else if (invec[i] != invec[*len - 1] && inoutvec[i] == invec[*len - 1]) {
            inoutvec[i] = invec[i];
        }
        else if (invec[i] == invec[*len - 1] && inoutvec[i] == invec[*len - 1]) {
            inoutvec[i] = 0;
        }

    }
}

void random_array(int* arr, int len)
{
    std::random_device rnd_device;
    std::mt19937 mersenne_engine(rnd_device());
    std::uniform_int_distribution<int> dist(0, 5);
    for (int i = 0; i < len; i++) {
        arr[i] = dist(mersenne_engine);
    }
}

int main(int argc, char** argv)
{
    const int n = 5;
    int rank, size, i;
    int data[n+1];
    int result[n+1];
    MPI_Op op;
    int x[1];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) {
        std::string str;
        bool is_nan;
        do {
            std::cout << "Input value: ";
            std::cin >> str;
            std::istringstream istr(str);
            is_nan = !(istr >> x[0]);
        } while (is_nan);
        
    }
    MPI_Bcast(x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    random_array(data, n);
    data[n] = x[0];

    for (int i = 0;i < n;i++)
    {
        result[i] = 0;
    }
    result[n] = x[0];

    cout << left
        << setw(20)
        << to_string(rank) + " process: ";
    for (i = 0;i < n;i++)
        cout << left
        << setw(10)
        << data[i] << ' ';
    cout << "\n\n";

    MPI_Op_create((MPI_User_function*)sum, 0, &op);
    MPI_Reduce(&data, &result, n + 1, MPI_INT, op, 0, MPI_COMM_WORLD);

    MPI_Op_free(&op);
    if (rank == 0) {
        cout << left
            << setw(20)
            << "res ";
        for (i = 0;i < n;i++)
            cout << left
            << setw(10)
            << result[i] << ' ';
        cout << "\n\n";
    }

    MPI_Finalize();
}