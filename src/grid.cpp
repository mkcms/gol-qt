#include "grid.h"

Grid::Grid(int rows, int cols, QObject *parent)
    : QObject(parent)
{
    appendRows(rows);
    appendCols(cols);
}

void Grid::appendRows(int n)
{
    setSize(rows() + n, cols());
}

void Grid::appendCols(int n)
{
    setSize(rows(), cols() + n);
}

void Grid::setSize(int rows, int cols)
{
    const int currentRows = this->rows(), currentCols = this->cols();

    if (rows > currentRows)
        m_grid.insert(m_grid.end(), rows - currentRows, QVector<Cell>(cols));
    if (rows < currentRows)
        m_grid.erase(m_grid.end() - (currentRows - rows), m_grid.end());
    for (auto &row : m_grid) {
        if (cols > row.size())
            row.insert(row.end(), cols - row.size(), Cell());
        else if (cols < row.size())
            row.erase(row.end() - (row.size() - cols), row.end());
    }

    emit sizeChanged(currentRows, currentCols, rows, cols);
}

void Grid::setCellStateAt(int x, int y, bool state)
{
    if (m_grid[x][y].m_state == state)
        return;

    m_grid[x][y].m_state = state;
    emit cellStateChanged(x, y, state);
}
