#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include <QObject>
#include <QSet>
#include <QPoint>
#include <QHash>
#include <QVariant>
#include <QVector>

inline uint qHash(const QPoint& key)
{
    return qHash(QPair<int, int>(key.x(), key.y()));
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
    void appendRows(int n);
    void eraseRows(int n);
    void appendCols(int n);
    void eraseCols(int n);
    void setRowCount(int rows) { setSize(rows, cols()); }
    void setColCount(int cols) { setSize(rows(), cols); }
    void setSize(int rows, int cols);
    void setCellStateAt(int x, int y, bool state);
    void setCellDataAt(int x, int y, const QVariant& data)
    {
        m_data.insert(QPoint(x, y), data);
    }
    void toggleCellAt(int x, int y) { setCellStateAt(x, y, !stateAt(x, y)); }

signals:
    void cellAdded(int x, int y);
    void cellRemoved(int x, int y);
    void cellStateChanged(int x, int y, bool state);
    void sizeChanged(int oldSizeX, int oldSizeY, int newSizeX, int newSizeY);

public:
    Grid *clone() const
    {
        Grid *ret = new Grid(cols(), rows());
        ret->m_grid = m_grid;
        ret->m_activeCells = m_activeCells;
        // data not copied.
        return ret;
    }
    bool stateAt(int x, int y) const { return m_grid[x][y]; }
    QVariant dataAt(int x, int y) const { return m_data.value(QPoint(x, y)); }
    GridCellNeighbourIterator neighbourIterator(int x, int y) const
    {
        return { {x, y}, {cols(), rows()} };
    }
    int cols() const { return m_grid.size(); }
    int rows() const { return m_grid.empty() ? 0 : m_grid[0].size(); }
    const QSet<QPoint>& activeCells() const { return m_activeCells; }
    QSet<QPoint>::const_iterator begin() const { return m_activeCells.begin(); }
    QSet<QPoint>::const_iterator end() const { return m_activeCells.end(); }

private:
    QVector<QVector<bool>> m_grid;
    QHash<QPoint, QVariant> m_data;
    QSet<QPoint> m_activeCells;
};

#endif /* GRID_H_INCLUDED */
