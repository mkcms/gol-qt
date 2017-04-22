#include "grid.h"
#include <QtGlobal>

constexpr QPoint GridCellNeighbourIterator::offsets[];

Grid::Grid(int rows, int cols, QObject *parent)
    : QObject(parent)
{
    setSize(rows, cols);
}

void Grid::setSize(int rows, int cols)
{
    Q_ASSERT(rows > 0 && cols > 0);
    bool different = rows != m_rowCount || cols != m_colCount;

    if (cols > m_colCount)
        m_data.resize(cols);

    while (cols > m_colCount) {
        m_colCount++;
        emit columnAdded();
    }

    while (cols < m_colCount) {
        m_colCount--;
        emit columnRemoved();
        for (int i = 0; i < m_rowCount; ++i) {
            m_activeCells -= {m_colCount, i};
            m_data[m_colCount].remove(i);
        }
    }

    m_data.resize(cols);

    while (rows > m_rowCount) {
        m_rowCount++;
        emit rowAdded();
    }

    while (rows < m_rowCount) {
        m_rowCount--;
        emit rowRemoved();
        for (int i = 0; i < m_colCount; ++i) {
            m_activeCells -= {i, m_rowCount};
            m_data[i].remove(m_rowCount);
        }
    }

    if (different)
        emit sizeChanged(cols, rows);
}

void Grid::setCellStateAt(QPoint cell, bool state)
{
    if (stateAt(cell) == state)
        return;

    if (state)
        m_activeCells += cell;
    else
        m_activeCells -= cell;

    emit cellStateChanged(cell, state);
}
