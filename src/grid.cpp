#include "grid.h"
#include <QtGlobal>

constexpr QPoint GridCellNeighbourIterator::offsets[];

Grid::Grid(int rows, int cols, QObject *parent)
    : QObject(parent)
{
    appendCols(cols);
    appendRows(rows);
}

void Grid::appendRows(int n)
{
    Q_ASSERT(n > 0);
    const int oldCount = rows();

    for (auto& col : m_grid) {
        col.resize(col.size() + n);
        for (int i = col.size() - n; i < col.size(); ++i)
            emit cellAdded({ static_cast<int>(&col - &*m_grid.begin()), i});
    }

    emit sizeChanged(cols(), oldCount, cols(), rows());
}

void Grid::eraseRows(int n)
{
    Q_ASSERT(n < rows() && n > 0);
    const int oldCount = rows();

    while (n--) {
        for (auto& col : m_grid) {
            QPoint pos(&col - &*m_grid.begin(), col.size() - 1);
            emit cellRemoved({pos.x(), pos.y()});
            m_data.remove(pos);
            col.erase(col.end() - 1);
        }
    }

    emit sizeChanged(cols(), oldCount, cols(), rows());
}

void Grid::appendCols(int n)
{
    Q_ASSERT(n > 0);
    const int oldCount = cols();

    while (n--) {
        m_grid.append(QVector<bool>(rows()));
        for (int i = 0; i < rows(); ++i)
            emit cellAdded({m_grid.size() - 1, i});
    }

    emit sizeChanged(oldCount, rows(), cols(), rows());
}

void Grid::eraseCols(int n)
{
    Q_ASSERT(n < cols() && n > 0);
    const int oldCount = cols();

    while (n--) {
        for (auto& row : m_grid.back()) {
            QPoint pos(m_grid.size() - 1, &row - &*m_grid.back().begin());
            emit cellRemoved({pos.x(), pos.y()});
            m_data.remove(pos);
        }
        m_grid.erase(m_grid.end() - 1);
    }

    emit sizeChanged(oldCount, rows(), cols(), rows());
}

void Grid::setSize(int rows, int cols)
{
    if (rows > this->rows())
        appendRows(rows - this->rows());
    else if (rows < this->rows())
        eraseRows(this->rows() - rows);

    if (cols > this->cols())
        appendCols(cols - this->cols());
    else if (cols < this->cols())
        eraseCols(this->cols() - cols);
}

void Grid::setCellStateAt(QPoint cell, bool state)
{
    if (stateAt(cell) == state)
        return;

    if (state)
        m_activeCells += cell;
    else
        m_activeCells -= cell;

    m_grid[cell.x()][cell.y()] = state;
    emit cellStateChanged(cell, state);
}
