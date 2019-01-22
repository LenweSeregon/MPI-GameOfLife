#include <mpi.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "Master.hpp"

Master::Master(unsigned int nbWorker, unsigned int totalLine, unsigned int totalColumn):
nbWorker_(nbWorker),
totalLine_(totalLine),
totalColumn_(totalColumn)
{
    int nbLinePerWorker = (float)totalLine / (float)nbWorker;

    for(unsigned int i = 0; i < nbWorker_; i++) {
        if(i == nbWorker_ -1) {
            nbLineForEachWorker_.push_back(totalLine - ((nbWorker-1) * nbLinePerWorker));
        } else {
            nbLineForEachWorker_.push_back(nbLinePerWorker);
        }
    }
}

void Master::SendInformationsToEachWorkers()
{
    /* Informations are an array of 4 element representing : [nbLineManaged,totalLine,TotalColumn] */
    for(unsigned int i = 0; i < nbWorker_; i++) {
        unsigned int informations[3] = {nbLineForEachWorker_[i],totalLine_,totalColumn_};
        MPI_Send(informations, 3, MPI_INT, i+1, 666, MPI_COMM_WORLD);       
    }
}

void Master::SendInitialPartToEachWorker(std::vector<std::vector<int>> states)
{
    unsigned int lineFocus = 0;
    for(unsigned int k = 0; k < nbWorker_; k++) {
        std::vector<int> datas;
        unsigned int nbLine = nbLineForEachWorker_[k];
        unsigned int startLine = lineFocus;

        for(unsigned int i = startLine; i < startLine + nbLine; i++) {
            for(unsigned int j = 0; j < totalColumn_; j++) {
                datas.push_back(i);
                datas.push_back(j);
                datas.push_back(states[i][j]);
            }
        }

        lineFocus += nbLine;
        MPI_Send(&datas[0],datas.size(),MPI_INT,k+1,666,MPI_COMM_WORLD);
    }
}

void Master::SendGoWorkToEachWorker()
{
    for(unsigned int i = 0; i < nbWorker_; i++) {
        int goWork = DO_WORK;
        MPI_Send(&goWork, 1, MPI_INT, i+1, 666, MPI_COMM_WORLD);
    }
}

void  Master::SendStopWorking()
{
    for(unsigned int i = 0; i < nbWorker_; i++) {
        int stop = STOP;
        MPI_Send(&stop, 1, MPI_INT, i+1, 666, MPI_COMM_WORLD);
    }
}

std::vector<int> Master::ReceiveCellsDatas(int workerID)
{
    /* On envoie une probe pour savoir combien on va recevoir */
    std::vector<int> datas;
    MPI_Status status;
    MPI_Probe(workerID, 666, MPI_COMM_WORLD, &status);

    int nbReceive;
    MPI_Get_count(&status,MPI_INT,&nbReceive);
    datas.resize(nbReceive);

    MPI_Recv(&datas[0],nbReceive,MPI_INT,workerID,666,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return datas;
}

int Master::ReceiveWorkerStatusPassive(int sleepingTimeAsMicroseconds, int& sender)
{
    MPI_Request request;
    int response;
    int flag = 0;
    MPI_Irecv(&response,1,MPI_INT, MPI_ANY_SOURCE, 666, MPI_COMM_WORLD, &request);
    while(!flag) {
        MPI_Status status;
        MPI_Test(&request,&flag,&status);
        if(flag) {
            sender = status.MPI_SOURCE;
            return response;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepingTimeAsMicroseconds));
    }
    sender = -1;
    return -1;
}