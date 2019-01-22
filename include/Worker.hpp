#ifndef __WORKER__
#define __WORKER__

#include "CellWorker.hpp"

#define READY 1
#define JOB_DONE 2
#define DO_WORK 3
#define STOP 4

class Worker
{
    private:

    unsigned int nbWorker_;
    unsigned int rank_;
    unsigned int version_;

    unsigned int nbLineToManaged_;
    unsigned int totalLine_;
    unsigned int totalColumn_;
    unsigned int startingLine_;
    unsigned int endingLine_;

    public:
    
    Worker(unsigned int nbWorker, int rank, unsigned int version);
    
    /* Methods according to settings variable to right values */
    void InitNeighbors(int& upperNeighbor, int& lowerNeighbor);

    /* Methods according to updating cells */
    int GetStateFromPosition(int line, int column,
                 const std::vector<CellWorker>& cells, 
                 const std::vector<CellWorker>& cellsUpper, 
                 const std::vector<CellWorker>& cellsLower);
    bool PositionIsValid(int line, int column);
    std::vector<CellWorker> UpdateCells(const std::vector<CellWorker>& cells, 
                                        const std::vector<CellWorker>& cellsUpper, 
                                        const std::vector<CellWorker>& cellsLower);

    /* Methods according to receiving informations */
    void ReceiveInformations();
    void ReceiveInitialPart(std::vector<CellWorker>& cells);
    void ReceivePartFromUpperNeighbor(std::vector<CellWorker>& cellsUpper);
    void ReceivePartFromLowerNeighbor(std::vector<CellWorker>& cellsLower);
    int ReceivePassiveOrder(int sleepingTimeAsMicroseconds);

    /* Methods according to sending informations */
    void SendWorkDone();
    void SendCellsUpdated(const std::vector<CellWorker>& cells);
    void SendPartToUpperNeighbor(const std::vector<CellWorker>& cells);
    void SendPartToLowerNeighbor(const std::vector<CellWorker>& cells);

    /* Methods getter */
    unsigned int GetNbColumn() const;
    unsigned int GetNbLineToManage() const;
    unsigned int GetStartingLine() const;
};

#endif 