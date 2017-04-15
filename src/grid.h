#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include <QObject>
#include <QPoint>
#include <QHash>
#include <QVariant>
#include <QVector>

inline uint qHash(const QPoint& key)
{
    return qHash(QPair<int, int>(key.x(), key.y()));
}

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
    bool stateAt(int x, int y) const { return m_grid[x][y]; }
    QVariant dataAt(int x, int y) const { return m_data.value(QPoint(x, y)); }
    int cols() const { return m_grid.size(); }
    int rows() const { return m_grid.empty() ? 0 : m_grid[0].size(); }

private:
    QVector<QVector<bool>> m_grid;
    QHash<QPoint, QVariant> m_data;
};

#endif /* GRID_H_INCLUDED */
