#include <stdio.h>
#include <mpi.h>
#define to_right 201

int main(int argc, char *argv[])
{
    int my_rank, size, right, left;
    int rcv_buf, sum, i;
    int snd_buf;
    //  MPI_Status status;
    //  MPI_Request request;
    MPI_Win win;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    right = (my_rank + 1) % size;
    left = (my_rank - 1 + size) % size;
    /* ...this SPMD-style neighbor computation with modulo has the same meaning as: */
    /* right = my_rank + 1; if (right == size) right = 0; */
    /* left  = my_rank - 1; if (left == -1) left = size-1;*/

    MPI_Win_create(&rcv_buf, (MPI_Aint)sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    sum = 0;
    snd_buf = my_rank;

    for (i = 0; i < size; i++)
    {

        /* *** The following 3 lines shall be substituted by
         *** 1-sided communication and synchronization    */
        MPI_Win_fence(0, win);
        MPI_Put(&snd_buf, 1, MPI_INT, right, (MPI_Aint)0, 1, MPI_INT, win);
        MPI_Win_fence(0, win);

        // MPI_Accumulate(&rcv_buf, 1, MPI_INT, right, (MPI_Aint)0, 1, MPI_INT, MPI_SUM, win);
        // MPI_Issend(&snd_buf, 1, MPI_INT, right, to_right, MPI_COMM_WORLD, &request);
        // MPI_Recv(&rcv_buf, 1, MPI_INT, left, to_right, MPI_COMM_WORLD, &status);
        // MPI_Wait(&request, &status);
        snd_buf = rcv_buf;

        sum += rcv_buf;
    }

    printf("PE%i:\tSum = %i\n", my_rank, sum);
    MPI_Win_free(&win);
    MPI_Finalize();
}