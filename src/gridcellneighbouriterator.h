#ifndef GRIDCELLNEIGHBOURITERATOR_H_INCLUDED
#define GRIDCELLNEIGHBOURITERATOR_H_INCLUDED

#include <QPoint>
#include <QSize>

class GridCellNeighbourIterator
{
public:
    GridCellNeighbourIterator() = default;
    GridCellNeighbourIterator(const QPoint& cell, const QSize& gridSize);

    const QPoint& operator*() const { return m_curCell; }
    const QPoint* operator->() const { return &m_curCell; }

    GridCellNeighbourIterator& operator++();
    GridCellNeighbourIterator operator++(int);

    bool operator==(const GridCellNeighbourIterator& rhs) const;
    bool operator!=(const GridCellNeighbourIterator& rhs) const;
private:
    bool valid() const;

    QPoint m_curCell;
    QSize m_gridSize;
    int m_curOffset = -1;
};

#endif /* GRIDCELLNEIGHBOURITERATOR_H_INCLUDED */
