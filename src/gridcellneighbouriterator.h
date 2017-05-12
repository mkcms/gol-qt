#ifndef GRIDCELLNEIGHBOURITERATOR_H_INCLUDED
#define GRIDCELLNEIGHBOURITERATOR_H_INCLUDED

#include <QPoint>

class GridCellNeighbourIterator
{
public:
    GridCellNeighbourIterator() = default;
    GridCellNeighbourIterator(QPoint cell, QPoint extent);

    QPoint operator*() const { return m_curCell; }
    const QPoint* operator->() const { return &m_curCell; }

    GridCellNeighbourIterator& operator++();
    GridCellNeighbourIterator operator++(int);

    bool operator==(const GridCellNeighbourIterator& rhs) const;
    bool operator!=(const GridCellNeighbourIterator& rhs) const;
private:
    bool valid() const;

    QPoint m_curCell;
    QPoint m_extent;
    int m_curOffset = -1;
};

#endif /* GRIDCELLNEIGHBOURITERATOR_H_INCLUDED */
