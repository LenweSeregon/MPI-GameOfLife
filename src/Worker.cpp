#include <mpi.h>
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>

#include "Worker.hpp"

Worker::Worker(unsigned int nbWorker, int rank, unsigned int version):
nbWorker_(nbWorker),
rank_(rank),
version_(version)
{

}

unsigned int Worker::GetStartingLine() const
{
    return startingLine_;
}

unsigned int Worker::GetNbColumn() const
{
    return totalColumn_;
}

unsigned int Worker::GetNbLineToManage() const
{
    return nbLineToManaged_;
}

void Worker::InitNeighbors(int& upperNeighbor, int& lowerNeighbor)
{
    if(rank_ > 1) {
        upperNeighbor = rank_ - 1;
    }

    if(rank_ < nbWorker_) {
        lowerNeighbor = rank_ + 1;
    }
}

void Worker::ReceiveInformations()
{
    unsigned int informations[3];
    MPI_Recv(informations,3,MPI_INT,0, 666,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    nbLineToManaged_ = informations[0];
    totalLine_ = informations[1];
    totalColumn_ = informations[2];
    
    startingLine_ = (rank_ - 1) * (totalLine_ / nbWorker_);
    endingLine_ = startingLine_ + nbLineToManaged_;
}

void Worker::ReceiveInitialPart(std::vector<CellWorker>& cells)
{
    std::vector<int> datas;
    int gonnaReceive = nbLineToManaged_ * totalColumn_ * 3;
    datas.resize(gonnaReceive);

    MPI_Recv(&datas[0],gonnaReceive,MPI_INT,0,666, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for(unsigned int i = 0; i < datas.size() ; i+=3) {
        cells.push_back(CellWorker(datas[i],datas[i+1],datas[i+2]));
    }
}

void Worker::ReceivePartFromUpperNeighbor(std::vector<CellWorker>& cellsUpper)
{
    int upper = rank_ - 1;
    std::vector<int> datas;
    int gonnaReceive = totalColumn_ * 3;
    datas.resize(gonnaReceive);

    MPI_Recv(&datas[0],gonnaReceive, MPI_INT, upper, 666, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int it = 0;
    for(unsigned int i = 0; i < datas.size(); i+= 3) {
        cellsUpper[it++] = CellWorker(datas[i],datas[i+1],datas[i+2]); 
    }
}

void Worker::ReceivePartFromLowerNeighbor(std::vector<CellWorker>& cellsLower)
{
    int lower = rank_ + 1;
    std::vector<int> datas;
    int gonnaReceive = totalColumn_ * 3;
    datas.resize(gonnaReceive);

    MPI_Recv(&datas[0],gonnaReceive, MPI_INT, lower, 666, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int it = 0;
    for(unsigned int i = 0; i < datas.size(); i+= 3) {
        cellsLower[it++] = CellWorker(datas[i],datas[i+1],datas[i+2]); 
    }
}

int Worker::ReceivePassiveOrder(int sleepingTimeAsMicroseconds)
{
    MPI_Request request;
    int response;
    int flag = 0;
    MPI_Irecv(&response,1,MPI_INT, 0, 666, MPI_COMM_WORLD, &request);
    while(!flag) {
        MPI_Status status;
        MPI_Test(&request,&flag,&status);
        if(flag) {
            return response;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepingTimeAsMicroseconds));
    }

    return -1;
}

void Worker::SendWorkDone()
{
    int workDone = JOB_DONE;
    MPI_Send(&workDone, 1, MPI_INT, 0, 666, MPI_COMM_WORLD);
}

void Worker::SendCellsUpdated(const std::vector<CellWorker>& cells)
{
    std::vector<int> datas;
    for(auto& cell : cells) {
        datas.push_back(cell.GetLine());
        datas.push_back(cell.GetColumn());
        datas.push_back(cell.GetState());
    }

    MPI_Send(&datas[0],datas.size(), MPI_INT, 0, 666, MPI_COMM_WORLD);
}


void Worker::SendPartToUpperNeighbor(const std::vector<CellWorker>& cells)
{
    int rankUpper = rank_ - 1;
    std::vector<int> datas;
    for(unsigned int i = 0; i < totalColumn_; i++) {
        CellWorker cell = cells[i];
        datas.push_back(cell.GetLine());
        datas.push_back(cell.GetColumn());
        datas.push_back(cell.GetState());
    }

    MPI_Send(&datas[0], datas.size(), MPI_INT, rankUpper, 666, MPI_COMM_WORLD);
}

void Worker::SendPartToLowerNeighbor(const std::vector<CellWorker>& cells)
{
    int rankLower = rank_ + 1;
    std::vector<int> datas;
    for(unsigned int i = 0; i < totalColumn_; i++) {
        CellWorker cell = cells[((nbLineToManaged_-1) * totalColumn_) + i];
        datas.push_back(cell.GetLine());
        datas.push_back(cell.GetColumn());
        datas.push_back(cell.GetState());
    }

    MPI_Send(&datas[0], datas.size(), MPI_INT, rankLower, 666, MPI_COMM_WORLD);
}

bool Worker::PositionIsValid(int line, int column)
{
    return line >= 0 && (unsigned int)line < totalLine_ && column >= 0 && (unsigned int)column < totalColumn_;
}

int Worker::GetStateFromPosition(int line, int column,
                     const std::vector<CellWorker>& cells, 
                     const std::vector<CellWorker>& cellsUpper, 
                     const std::vector<CellWorker>& cellsLower) 
{
    
    if((unsigned int) line < startingLine_ ){ // upper 
        return cellsUpper[column].GetState();
    } else if((unsigned int) line >= endingLine_) { // Lower
        return cellsLower[column].GetState();
    } else { // Actual
        int linearPosition = (line-startingLine_) * totalColumn_ + column;
        return cells[linearPosition].GetState();
    }
}

std::vector<CellWorker> Worker::UpdateCells(const std::vector<CellWorker>& cells, 
                         const std::vector<CellWorker>& cellsUpper, 
                         const std::vector<CellWorker>& cellsLower)
{
    std::vector<CellWorker> updated;

    for(auto& cell : cells) {
        int nbAlive = 0;
        for(auto& position : cell.GetAllNeighbor()){
            if(PositionIsValid(position.x, position.y)) {
                int state = GetStateFromPosition(position.x, position.y, cells, cellsUpper, cellsLower);
                if(state == 1 || state == 2) {
                    nbAlive++;
                }
            }
        }

        /*  0 = MORT
            1 = STABLE
            2 = NAISSANCE
            3 = MOURANT
        */
        int currentState = cell.GetState();
        bool shouldDie = false;
        bool shouldStable = false;

        if(nbAlive < 2 || nbAlive > 3) {
            shouldDie = true;
        }
        else if(nbAlive == 2) {
            shouldStable = true;
        }

        /*  Version 1 = reupdate every cells even if they don't change
            Version 2 = update only cells that get change 
        */
        if(version_ == 1) {
            /*  Il doit mourir et il est mort / mourant, change => 0
                Il doit mourir et il est vivant / naissant , change => 3
            */
            if(shouldDie) {
                if(currentState == 1 || currentState == 2) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),3));
                } else {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),0));
                }
            } else if(shouldStable){
                /*  Il doit se stabiliser et il est mort / mourant, change => 0
                    Il doit se stabiliser et il est vivant / naissant, change => 1
                */
                if(currentState == 1 || currentState == 2) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),1));
                } else {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),0));
                }
            } else {
                /*  Il doit naitre et il est mort / mourant, change => 2
                    Il doit naitre et il est vivant / naissant , change => 1
                */
                if(currentState == 1 || currentState == 2) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),1));
                } else {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),2));
                }
            } 
        } else if(version_ == 2) {
            /*  Il doit mourir et il est mort, ne change pas
                Il doit mourir et il est mourant, change => 0
                Il doit mourir et il est vivant / naissant, change => 3
            */
            if(shouldDie) {
                if(currentState == 1 || currentState == 2) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),3));
                } else if(currentState == 3) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),0));
                } 
            } else if(shouldStable) {
                /*  Il doit se stabiliser et il est mort, ne change pas
                    Il doit se stabiliser et il est mourant, change => 0
                    Il doit se stabiliser et il est vivant , ne change pas
                    Il doit se stabiliser et il est naissant, change => 1
                */
                if(currentState == 2) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),1));
                } else if(currentState == 3) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),0));
                }
            } else {
                /*  Il doit naitre et il est mort / mourant, change => 2
                    Il doit naitre et il est naissant , change => 1
                    Il doit naitre et il est vivant , ne change pas
                */
                if(currentState == 3 || currentState == 0) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),2));
                } else if(currentState == 2) {
                    updated.push_back(CellWorker(cell.GetLine(),cell.GetColumn(),1));
                }
            }
        } else {
            std::cerr << "Version is undefined ! " << std::endl;
        }

    }
    return updated;
}
