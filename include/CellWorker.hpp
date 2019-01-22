#ifndef __CELL_WORKER__
#define __CELL_WORKER__

#include <vector>

class Vector2u {
    
    public:
    int x;
    int y;

    Vector2u(int x, int y) {
        this->x = x;
        this->y = y;
    }
};

class CellWorker
{

    private:

    unsigned int line_;
    unsigned int column_;
    int state_;

    public:

    CellWorker(unsigned int line, unsigned int column, int state);

    unsigned int GetLine() const;
    unsigned int GetColumn() const;
    int GetState() const;

    void SetState(int state);

    std::vector<Vector2u> GetAllNeighbor() const;
    Vector2u GetUpperLeftNeighbor() const;
    Vector2u GetUpperMiddleNeighbor() const;
    Vector2u GetUpperRightNeighbor() const;
    Vector2u GetLeftNeighbor() const;
    Vector2u GetRightNeighbor() const;
    Vector2u GetLowerLeftNeighbor() const;
    Vector2u GetLowerMiddleNeighbor() const;
    Vector2u GetLowerRightNeighbor() const;
    

};

#endif