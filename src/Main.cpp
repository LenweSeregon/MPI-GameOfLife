#include <SFML/Graphics.hpp>
#include <mpi.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>

#include "Button.hpp"
#include "Renderer.hpp"
#include "Worker.hpp"
#include "Master.hpp"
#include "Constantes.hpp"
#include "CellWorker.hpp"

void InitializeStartingBoard(std::vector<std::vector<int>>& states, unsigned int line, unsigned int column) {
    
    for(unsigned int i = 0; i < line; i++) {
        states.push_back(std::vector<int>());
        for(unsigned int j = 0; j < column; j++) {
            states[i].push_back(0);
        }
    }
}

int main(int argc, char** argv)
{
    int rank;
    int size;
    int nbWorker;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    nbWorker = size - 1;

    if(rank == 0) {

        unsigned int sizeCell = 2;
        unsigned int totalLine = 350;
        unsigned int totalColumn = 350;
        
        std::vector<std::vector<int>> states;
        InitializeStartingBoard(states,totalLine,totalColumn);
        
        Renderer renderer(1,sf::Color::White, sizeCell, totalLine, totalColumn);

        /* Changing scenario */
        // Ship
        renderer.GenerateShip(70,70,states);
        
        // U generator
        renderer.GenerateU(100,100,states);
        renderer.GenerateU(200,100,states);
        renderer.GenerateU(300,100,states);

        renderer.GenerateU(100,300,states);
        renderer.GenerateU(200,300,states);
        renderer.GenerateU(300,300,states);

        /* End changing scenario */

        Master master(nbWorker,totalLine,totalColumn);
        master.SendInformationsToEachWorkers();
        master.SendInitialPartToEachWorker(states);

        bool isPaused = true;
        while (renderer.GetWindow().isOpen())
        {
            renderer.ManageEvents(isPaused);      

            renderer.GetWindow().clear(sf::Color::White);
            renderer.Draw();
            renderer.GetWindow().display();

            if(!isPaused) {
                master.SendGoWorkToEachWorker();

                int completed = nbWorker;
                while(completed != 0) {
                    int sender;
                    int response = master.ReceiveWorkerStatusPassive(5,sender);
                    if(response == JOB_DONE) {
                        std::vector<int> cellsUpdated = master.ReceiveCellsDatas(sender);
                        for(unsigned int i = 0; i < cellsUpdated.size(); i+=3) {
                            int line = cellsUpdated[i];
                            int column = cellsUpdated[i+1];
                            int state = cellsUpdated[i+2];
                            renderer.UpdateColor(line,column,state);
                        }
                        completed--;
                    } else {
                        std::cout << "Message from : " << sender << " which is not register." << std::endl;
                    }
                }
            }

            /* Timer of rendering loop, to slow down a bit */
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
        
        master.SendStopWorking();

    } else {
        std::vector<CellWorker> cells;
        std::vector<CellWorker> cellsUpper;
        std::vector<CellWorker> cellsLower;

        unsigned int version = 2;
        int neighborUpper = -1; int neighborLower = -1;

        Worker worker(nbWorker, rank, version);
        worker.ReceiveInformations();
        cellsUpper.reserve(worker.GetNbColumn());
        cellsLower.reserve(worker.GetNbColumn());
        worker.ReceiveInitialPart(cells);
        worker.InitNeighbors(neighborUpper, neighborLower);

        bool goOn = true;
        while(goOn) {
            int response = worker.ReceivePassiveOrder(10);
            if(response == DO_WORK) {
                /*  On sait que l'on doit travailler, maintenant on va
                    envoyer les parties manquantes aux autres...
                    La stratégie est la suivante :
                    On attend l'envoie du voisin du haut, puis on envoie au voisin de haut,
                    Puis on envoie au voisin du bas, puis on attend l'envoi du voisin du bas,
                    Pour savoir qui va envoyer en premier pour lancer le processus d'échange,
                    On démarre juste l'envoi avec le rank = 1
                */

                if(neighborUpper != -1) {
                    worker.ReceivePartFromUpperNeighbor(cellsUpper);
                    worker.SendPartToUpperNeighbor(cells);
                }

                if(neighborLower != -1) {
                    worker.SendPartToLowerNeighbor(cells);
                    worker.ReceivePartFromLowerNeighbor(cellsLower);
                }

                std::vector<CellWorker> updated = worker.UpdateCells(cells, cellsUpper, cellsLower);
                for(unsigned int i = 0; i < updated.size(); i++) {
                    int start = worker.GetStartingLine();
                    int totalColumn = worker.GetNbColumn();
                    int line = updated[i].GetLine();
                    int column = updated[i].GetColumn();

                    int linearPosition = (line-start) * totalColumn + column;

                    cells[linearPosition].SetState(updated[i].GetState());
                }

                worker.SendWorkDone();
                worker.SendCellsUpdated(updated);
            } else if(response == STOP) {
                goOn = false;
            } else {
                 std::cout << "Message from : " << 0 << " which is not register => " << response <<std::endl;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
