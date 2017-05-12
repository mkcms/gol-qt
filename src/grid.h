#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include <functional>
#include <QObject>
#include <QSet>
#include <QPoint>
#include <QHash>
#include <QVariant>
#include <QVector>
#include <QTextStream>
#include <QtGlobal>
#include <QSize>
#include "gridcellneighbouriterator.h"

uint qHash(const QPoint& key);

namespace std {
    template <>
    struct hash<QPoint>
    {
        size_t operator()(const QPoint& point) const;
    };
}

class Grid : public QObject
{
    Q_OBJECT
public:
    Grid(const QSize& size, QObject *parent = nullptr);
    virtual ~Grid() { invalidate(); }

public slots:
    void setRowCount(int rows) { setSize(rows, cols()); }
    void setColCount(int cols) { setSize(rows(), cols); }
    void setSize(int rows, int cols) { setSize({cols, rows}); }
    void setSize(const QSize& size);
    void setCellStateAt(const QPoint& cell, bool state);
    void setCellDataAt(const QPoint& cell, const QVariant& data);
    void clear();

signals:
    void columnAdded();
    void columnRemoved();
    void rowAdded();
    void rowRemoved();
    void cellStateChanged(const QPoint& cell, bool state);
    void sizeChanged(const QSize& newSize);

public:
    bool isValid() const { return m_size.isValid() && rows() * cols() != 0; }
    bool stateAt(const QPoint& cell) const { return m_activeCells.contains(cell); }
    QVariant dataAt(const QPoint& cell) const { return m_data[cell.x()][cell.y()]; }
    int cols() const { return m_size.width(); }
    int rows() const { return m_size.height(); }
    QSet<QPoint>::const_iterator begin() const { return m_activeCells.begin(); }
    QSet<QPoint>::const_iterator end() const { return m_activeCells.end(); }
    GridCellNeighbourIterator neighbourIterator(const QPoint& cell) const
    {
        return { cell, {cols(), rows()} };
    }

    Grid *clone() const;
    void copyStateFrom(const Grid *grid);

    friend QTextStream& operator<<(QTextStream& out, const Grid& grid);
    friend QTextStream& operator>>(QTextStream& out, Grid& grid);

private:
    void invalidate();
    void readPoints(QTextStream& stream);

    QSet<QPoint> m_activeCells;
    QVector<QHash<int, QVariant>> m_data;
    QSize m_size;
};

Q_DECLARE_METATYPE(Grid*)

#endif /* GRID_H_INCLUDED */
