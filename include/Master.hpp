#ifndef __MASTER__
#define __MASTER__

#include <vector>
#include "Worker.hpp"

#define READY 1
#define JOB_DONE 2
#define DO_WORK 3
#define STOP 4

class Master
{
    private:

    std::vector<unsigned int> nbLineForEachWorker_;
    unsigned int nbWorker_;
    unsigned int totalLine_;
    unsigned int totalColumn_;

    public:

    Master(unsigned int nbWorker, unsigned int totalLine, unsigned int totalColumn);

    void SendInformationsToEachWorkers();
    void SendInitialPartToEachWorker(std::vector<std::vector<int>> states);
    void SendGoWorkToEachWorker();
    void SendStopWorking();
    
    std::vector<int> ReceiveCellsDatas(int workerID);
    int ReceiveWorkerStatusPassive(int sleepingTimeAsMicroseconds, int& sender);


};

#endif