#include <chrono>
#include "mpi.h"

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  size_t size = 1000000000;
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double *a;
  a = (double*)calloc(size, sizeof(double));

  MPI_Status status; 

  auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

  if(rank==0)
    MPI_Send(a, size, MPI_DOUBLE, 1,0,MPI_COMM_WORLD);
  else{
    MPI_Recv(a, size, MPI_DOUBLE, 0,0,MPI_COMM_WORLD, &status);

  }

    auto end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  std::cout << "TIME:" << end-start << std::endl;


}
