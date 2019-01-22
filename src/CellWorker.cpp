#include "CellWorker.hpp"

CellWorker::CellWorker(unsigned int line, unsigned int column, int state):
line_(line),
column_(column),
state_(state)
{

}

void CellWorker::SetState(int state)
{
    state_ = state;
}

unsigned int CellWorker::GetLine() const
{
    return line_;
}

unsigned int CellWorker::GetColumn() const
{
    return column_;
}

int CellWorker::GetState() const
{
    return state_;
}

std::vector<Vector2u> CellWorker::GetAllNeighbor() const
{
    std::vector<Vector2u> neighbors;
    neighbors.push_back(GetUpperLeftNeighbor());
    neighbors.push_back(GetUpperMiddleNeighbor());
    neighbors.push_back(GetUpperRightNeighbor());
    neighbors.push_back(GetLeftNeighbor());
    neighbors.push_back(GetRightNeighbor());
    neighbors.push_back(GetLowerLeftNeighbor());
    neighbors.push_back(GetLowerMiddleNeighbor());
    neighbors.push_back(GetLowerRightNeighbor());
    return neighbors;
}

Vector2u CellWorker::GetUpperLeftNeighbor() const
{
    int newLine = line_ - 1;
    int newColumn = column_ - 1;

    return Vector2u(newLine,newColumn);
}

Vector2u CellWorker::GetUpperMiddleNeighbor() const
{
    int newLine = line_ - 1;
    int newColumn = column_;

    return Vector2u(newLine,newColumn);
}

Vector2u CellWorker::GetUpperRightNeighbor() const
{
    int newLine = line_ - 1;
    int newColumn = column_ + 1;

    return Vector2u(newLine,newColumn);
}

Vector2u CellWorker::GetLeftNeighbor() const
{
    int newLine = line_;
    int newColumn = column_ - 1;

    return Vector2u(newLine,newColumn);
}

Vector2u CellWorker::GetRightNeighbor() const
{
    int newLine = line_;
    int newColumn = column_ + 1;

    return Vector2u(newLine,newColumn);
}

Vector2u CellWorker::GetLowerLeftNeighbor() const
{
    int newLine = line_ + 1;
    int newColumn = column_ - 1;

    return Vector2u(newLine,newColumn);
}

Vector2u CellWorker::GetLowerMiddleNeighbor() const
{
    int newLine = line_ + 1;
    int newColumn = column_;

    return Vector2u(newLine,newColumn);
}

Vector2u CellWorker::GetLowerRightNeighbor() const
{
    int newLine = line_ + 1;
    int newColumn = column_ + 1;

    return Vector2u(newLine,newColumn);
}
