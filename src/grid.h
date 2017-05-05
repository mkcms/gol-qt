#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include <QObject>
#include <functional>
#include <QSet>
#include <QPoint>
#include <QHash>
#include <QVariant>
#include <QVector>
#include <QTextStream>

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
    static constexpr int offsets[][2] =
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
            m_curCell.rx() += offsets[m_curOffset][0];
            m_curCell.ry() += offsets[m_curOffset][1];
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

    bool isValid() const { return m_rowCount > 0 && m_colCount > 0; }
public slots:
    void setRowCount(int rows) { setSize(rows, cols()); }
    void setColCount(int cols) { setSize(rows(), cols); }
    void setSize(int rows, int cols);
    void setCellStateAt(QPoint cell, bool state);
    void setCellDataAt(QPoint cell, const QVariant& data)
    {
        m_data[cell.x()].insert(cell.y(), data);
    }
    void clear();

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
        if (!grid->isValid())
            return;
        setSize(grid->rows(), grid->cols());
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

    friend QTextStream& operator<<(QTextStream& out, const Grid& grid)
    {
        if (grid.m_activeCells.isEmpty())
            return out << 1 << " " << 1 << "\n" << 0 << "\n";

        int maxX = 0, minX = INT_MAX, maxY = 0, minY = INT_MAX;
        for (const QPoint& pt : grid.m_activeCells) {
            if (pt.x() > maxX)
                maxX = pt.x();
            if (pt.x() < minX)
                minX = pt.x();
            if (pt.y() > maxY)
                maxY = pt.y();
            if (pt.y() < minY)
                minY = pt.y();
        }

        QSet<QPoint> activeCells;
        for (const QPoint& pt : grid.m_activeCells)
            activeCells += pt - QPoint{minX, minY};

        int cols = maxX - minX + 1, rows = maxY - minY + 1;
        out << cols << " " << rows << "\n";
        grid.writePoints(activeCells, out);
        return out << "\n";
    }

    friend QTextStream& operator>>(QTextStream& out, Grid& grid)
    {
        int cols, rows;
        out >> cols >> rows;
        if (out.status() != QTextStream::Ok
            || cols <= 0 || rows <= 0)
            return out;
        Grid g{rows, cols};
        g.readPoints(out);
        grid.copyStateFrom(&g);

        return out;
    }
private:
    template <typename Container>
    void writePoints(const Container& points, QTextStream& stream) const
    {
        stream << m_activeCells.size();
        for (const QPoint& pt : points)
            stream << "\n" << pt.x() << " " << pt.y();
        stream << "\n";
    }

    void readPoints(QTextStream& stream)
    {
        int n;
        stream >> n;
        while (n--) {
            if (stream.status() != QTextStream::Ok)
                break;
            int x, y;
            stream >> x >> y;
            m_activeCells += QPoint{x, y};
        }
        if (stream.status() != QTextStream::Ok)
            m_rowCount = -1;
    }

    QSet<QPoint> m_activeCells;
    // Mapping of column->map[row, data].
    QVector<QHash<int, QVariant>> m_data;
    int m_colCount = 0;
    int m_rowCount = 0;
};

Q_DECLARE_METATYPE(Grid*)

#endif /* GRID_H_INCLUDED */
