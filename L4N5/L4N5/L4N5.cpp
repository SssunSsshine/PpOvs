#include "mpi.h"
#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

int main(int argc, char** argv)
{
    const int n = 5;
    const int m = n * 2 + n - 2;
    float a[n][n], b[m];
    int xpose, rank;
    //
    int array_of_displacements[n + 1], array_of_blocklengths[n + 1];

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i][j] = j + 10 * i;
        }
    }

    if (rank == 0) {
        cout << "A\n";
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << left
                    << setw(5)
                    << a[i][j]
                    << " ";
            }
            cout << "\n\n";
        }
    }

    //массив, содержащий число элементов базового типа в каждом блоке;
    array_of_blocklengths[0] = n - 1;
    //массив смещений каждого блока от начала размещения элемента нового типа, смещения измеряются числом элементов базового типа;
    array_of_displacements[0] = 0;
    for (int i = 1; i <= n; i++) {
        array_of_blocklengths[i] = 1;
        array_of_displacements[i] = array_of_displacements[i - 1] + n - 1;
    }
    array_of_blocklengths[n] = n;
    MPI_Type_indexed(n + 1, array_of_blocklengths, array_of_displacements, MPI_FLOAT, &xpose);
    MPI_Type_commit(&xpose);
    //преобразование матрицы
    MPI_Sendrecv(a, 1, xpose, rank, 0, b, m, MPI_FLOAT, rank, 0, MPI_COMM_WORLD, &status);
    if (rank == 0) {
        cout << "B\n";
        for (int i = 0; i < m; i++) {
            
            cout << left
                << setw(5)
                << b[i]
                << " ";
           
        }
    }
    MPI_Type_free(&xpose);
    MPI_Finalize();
}