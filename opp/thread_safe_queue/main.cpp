#include "Task.hpp"
#include "MyQueue.hpp"
#include <iostream>
#include <mpi/mpi.h>
#include <mpi/mpi_proto.h>
#include <vector>
// #include <mpi.h>
// #define LINE std::cout << "here: " << __LINE__ << '\n'

const int STOP_RECIEVING = -200;
const int NO_TASKS_TO_SEND = -100;
bool allPackTasksFinished = false;
auto queue = MyQueue<Task>();

void RecvThreadStart(const int numberOfTasksInAPack, const int size, const int rank);
void SendThreadStart(const int size, const int rank, const int numberOfTasksInAPack);
void DoTasks();
void CreateTaskPack(MyQueue<Task> &queue, const int n, const int rank);
void StopComms(const int rank, const int size);
void SlowTaskExec(const int rank, const int numberOfTasksInAPack);

int main(int argc, char *argv[])
{
  const int numberOfPacks = 5;
  const int numberOfTasksInAPack = 1000;
  int provided;
  int rank;
  int size;
  int packIndex = 0;
  double start;
  double finish;

  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  if (provided != MPI_THREAD_MULTIPLE)
  {
    MPI_Finalize();
    std::cerr << "Couldn't get a thread-comlient mpi.\n";
    return 0;
  }

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  start = MPI_Wtime();

  std::thread recvThread(RecvThreadStart, numberOfTasksInAPack, size, rank);
  std::thread sendThread;

  while (packIndex < numberOfPacks)
  {
    if (rank == 0)
    {
      std::cout << "Running pack " << packIndex + 1 << '\n';
    }
    MPI_Barrier(MPI_COMM_WORLD);
    sendThread = std::thread(SendThreadStart, size, rank, numberOfTasksInAPack);
    // sendThread = std::thread(SlowTaskExec, rank, numberOfTasksInAPack);
    sendThread.join();
    MPI_Barrier(MPI_COMM_WORLD);
    ++packIndex;
  }
  // std::cout << "here :) \n";
  MPI_Barrier(MPI_COMM_WORLD);
  StopComms(rank, size); // the loop is over (sendThread is dead), but the reciever still exists and looks for a send
  MPI_Barrier(MPI_COMM_WORLD);
  finish = MPI_Wtime();
  recvThread.join();
  if (rank == 0)
  {
    std::cout << "----------------------------------------------\n";
    std::cout << "time: " << finish - start << '\n';
    std::cout << "----------------------------------------------\n";
  }

  MPI_Finalize();

  return 0;
}

void StopComms(const int rank, const int size)
{
  if (rank == 0)
  {
    allPackTasksFinished = true;
    int finishMessage = STOP_RECIEVING;
    for (int i = 0; i < size; i++)
    {
      if (rank == i)
      {
        continue;
      }
      MPI_Send(&finishMessage, 1, MPI_INT, i, 1111, MPI_COMM_WORLD);
    }
  }
}

void CreateTaskPack(MyQueue<Task> &queue, const int n, const int rank)
{
  for (int i = 0; i < n; i++)
  {
    queue.Emplace(rank * (rand() % 10 + 1));
  }
}

void RecvThreadStart(const int numberOfTasksInAPack, const int size, const int rank)
{
  MPI_Barrier(MPI_COMM_WORLD);
  int requestingRank = 0;
  std::vector<int> tmp;
  while (!allPackTasksFinished)
  {
    MPI_Request request;
    int flag = 0;
    MPI_Status status;
    MPI_Irecv(&requestingRank, 1, MPI_INT, MPI_ANY_SOURCE, 1111, MPI_COMM_WORLD,
              &request);
    while (!flag && !allPackTasksFinished)
    {
      MPI_Test(&request, &flag, &status);
    }
    if (requestingRank == STOP_RECIEVING)
    {
      allPackTasksFinished = true;
      std::cout << "Recieve thread stopped on rank " << rank << '\n';
      return;
    }
    if (allPackTasksFinished) // causes "tag_match.c:62   UCX  WARN  unexpected tag-receive descriptor 0x55e1efcf0a40 was not matched" if not used
    {
      std::cout << "Recieve thread stopped on rank " << rank << '\n';
      allPackTasksFinished = false;
      return;
    }
    int numberOfTasksToSend = 0; // between actions in the loop some tasks might be executed
    if (queue.Size() > numberOfTasksInAPack / size)
    {
      tmp.resize(numberOfTasksInAPack / size);
      for (int i = 0; i < numberOfTasksInAPack / size; i++)
      {
        std::optional<Task> task = queue.Pop();
        if (task == std::nullopt)
        {
          break;
        }
        tmp[i] = task.value().timeInMilliseconds;
        ++numberOfTasksToSend;
      }
    }
    if (numberOfTasksToSend == 0)
    {
      numberOfTasksToSend = NO_TASKS_TO_SEND;
    }
    std::cout << "Sending tasks to rank " << requestingRank << " from rank " << rank << '\n';
    MPI_Send(&numberOfTasksToSend, 1, MPI_INT, requestingRank, rank, MPI_COMM_WORLD);
    if (numberOfTasksToSend != NO_TASKS_TO_SEND)
    {
      MPI_Send(tmp.data(), numberOfTasksToSend, MPI_INT, requestingRank, rank, MPI_COMM_WORLD);
    }
    std::cout << "Tasks sent to proc " << requestingRank << '\n';
    numberOfTasksToSend = 0;
    tmp.clear();
  }
}

void SlowTaskExec(const int rank, const int numberOfTasksInAPack)
{
  MPI_Barrier(MPI_COMM_WORLD);
  CreateTaskPack(queue, numberOfTasksInAPack, rank);
  DoTasks();
  std::cout << "Doing tasks slowly on rank " << rank << '\n';
}

void SendThreadStart(const int size, const int rank, const int numberOfTasksInAPack)
{
  // MPI_Barrier(MPI_COMM_WORLD);
  int recievedNumberOfTasks = 0;
  CreateTaskPack(queue, numberOfTasksInAPack, rank);
  DoTasks();
  for (int i = 0; i < size; ++i)
  {
    MPI_Request request;
    MPI_Status status;
    if (rank == i)
    {
      continue;
    }
    // std::cout << "line: " << __LINE__ << '\n';
    MPI_Send(&rank, 1, MPI_INT, i, 1111, MPI_COMM_WORLD);
    MPI_Recv(&recievedNumberOfTasks, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
    if (recievedNumberOfTasks != NO_TASKS_TO_SEND)
    {
      // int temp = recievedNumberOfTasks;
      std::vector<int> tmp(recievedNumberOfTasks);
      MPI_Recv(tmp.data(), recievedNumberOfTasks, MPI_INT, i, i, MPI_COMM_WORLD, &status);
      for (int j = 0; j < recievedNumberOfTasks; j++)
      {
        queue.Emplace(tmp[j]);
      }
      DoTasks();
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
}

void DoTasks()
{
  while (!queue.IsEmpty())
  {
    std::optional<Task> task;
    task = queue.Pop();
    task->Execute();
  }
}