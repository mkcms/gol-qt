#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include <QObject>
#include <functional>
#include <QSet>
#include <QPoint>
#include <QHash>
#include <QVariant>
#include <QVector>

inline uint qHash(const QPoint& key)
{
    return qHash(QPair<int, int>(key.x(), key.y()));
}

namespace std {
    template <>
    struct hash<QPoint>
    {
        size_t operator()(const QPoint& point) const
        {
            return hash<int>()(point.x()) ^ hash<int>()(point.y());
        }
    };
}

class GridCellNeighbourIterator
{
public:
    static constexpr QPoint offsets[] =
    {
        {-1, 0}, {0, -1}, {1, 0},
        {1, 0}, {0, 1}, {0, 1},
        {-1, 0}, {-1, 0}
    };

    GridCellNeighbourIterator() = default;
    GridCellNeighbourIterator(QPoint cell, QPoint extent)
        : m_curCell(cell), m_extent(extent)
    {
        ++*this;
    }

    QPoint operator*() const { return m_curCell; }
    const QPoint* operator->() const { return &m_curCell; }
    GridCellNeighbourIterator& operator++()
    {
        ++m_curOffset;
        if (m_curOffset < 8) {
            m_curCell += offsets[m_curOffset];
            if (!valid())
                ++*this;
        }
        return *this;
    }

    GridCellNeighbourIterator operator++(int)
    {
        auto ret = *this;
        ++*this;
        return ret;
    }

    bool operator==(const GridCellNeighbourIterator& rhs) const
    {
        return rhs.m_curOffset == -1
            ? m_curOffset >= 8
            : m_curCell == rhs.m_curCell;
    }

    bool operator!=(const GridCellNeighbourIterator& rhs) const
    {
        return !(*this == rhs);
    }

private:
    bool valid() const
    {
        int x = m_curCell.x(), y = m_curCell.y();
        return x >= 0 && y >= 0 && x < m_extent.x() && y < m_extent.y();
    }

    QPoint m_curCell;
    QPoint m_extent;
    int m_curOffset = -1;
};

class Grid : public QObject
{
    Q_OBJECT
public:
    Grid(int rows, int cols, QObject *parent = nullptr);

public slots:
    void setRowCount(int rows) { setSize(rows, cols()); }
    void setColCount(int cols) { setSize(rows(), cols); }
    void setSize(int rows, int cols);
    void setCellStateAt(QPoint cell, bool state);
    void setCellDataAt(QPoint cell, const QVariant& data)
    {
        m_data[cell.x()].insert(cell.y(), data);
    }

signals:
    void columnAdded();
    void columnRemoved();
    void rowAdded();
    void rowRemoved();
    void cellStateChanged(QPoint cell, bool state);
    void sizeChanged(int newSizeX, int newSizeY);

public:
    Grid *clone() const
    {
        Grid *ret = new Grid(cols(), rows());
        ret->m_activeCells = m_activeCells;
        // data not copied.
        return ret;
    }
    void copyStateFrom(const Grid *grid)
    {
        setSize(grid->cols(), grid->rows());
        auto oldcells = m_activeCells;
        for (auto&& cell : oldcells)
            setCellStateAt(cell, false);
        for (auto&& cell : *grid)
            setCellStateAt(cell, true);
    }
    bool stateAt(QPoint cell) const { return m_activeCells.contains(cell); }
    QVariant dataAt(QPoint cell) const { return m_data[cell.x()][cell.y()]; }
    GridCellNeighbourIterator neighbourIterator(QPoint cell) const
    {
        return { cell, {cols(), rows()} };
    }
    int cols() const { return m_colCount; }
    int rows() const { return m_rowCount; }
    QSet<QPoint>::const_iterator begin() const { return m_activeCells.begin(); }
    QSet<QPoint>::const_iterator end() const { return m_activeCells.end(); }

private:
    QSet<QPoint> m_activeCells;
    // Mapping of column->map[row, data].
    QVector<QHash<int, QVariant>> m_data;
    int m_colCount = 0;
    int m_rowCount = 0;
};

#endif /* GRID_H_INCLUDED */
