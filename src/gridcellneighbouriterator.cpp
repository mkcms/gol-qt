#include "gridcellneighbouriterator.h"

namespace {
    constexpr int NeighbourOffsets[][2] =
    {
        {-1, 0}, {0, -1}, {1, 0},
        {1, 0}, {0, 1}, {0, 1},
        {-1, 0}, {-1, 0}
    };
}

GridCellNeighbourIterator::GridCellNeighbourIterator(QPoint cell, QPoint extent)
    : m_curCell(cell),
      m_extent(extent)
{
    ++*this;
}

GridCellNeighbourIterator& GridCellNeighbourIterator::operator++()
{
    ++m_curOffset;
    if (m_curOffset < 8) {
        m_curCell.rx() += NeighbourOffsets[m_curOffset][0];
        m_curCell.ry() += NeighbourOffsets[m_curOffset][1];
        if (!valid())
            ++*this;
    }
    return *this;
}

GridCellNeighbourIterator GridCellNeighbourIterator::operator++(int)
{
    auto ret = *this;
    ++*this;
    return ret;
}

bool GridCellNeighbourIterator::operator==(const GridCellNeighbourIterator& rhs) const
{
    return rhs.m_curOffset == -1
        ? m_curOffset >= 8
        : m_curCell == rhs.m_curCell;
}

bool GridCellNeighbourIterator::operator!=(const GridCellNeighbourIterator& rhs) const
{
    return !(*this == rhs);
}

bool GridCellNeighbourIterator::valid() const
{
    int x = m_curCell.x(), y = m_curCell.y();
    return x >= 0 && y >= 0 && x < m_extent.x() && y < m_extent.y();
}
