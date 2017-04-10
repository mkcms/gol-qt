#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include <QObject>
#include <QVariant>
#include <QVector>

class Grid : public QObject
{
    Q_OBJECT
public:
    class Cell
    {
    public:
        QVariant& data() const { return m_data; }
        void setData(const QVariant &data) const { m_data = data; };

    private:
        friend class Grid;

        mutable QVariant m_data;
        bool m_state = false;
    };

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

signals:
    void cellAdded(int x, int y);
    void cellRemoved(int x, int y);
    void cellStateChanged(int x, int y, bool state);
    void sizeChanged(int oldSizeX, int oldSizeY, int newSizeX, int newSizeY);

public:
    Cell& cellAt(int x, int y) { return m_grid[x][y]; }
    const Cell& cellAt(int x, int y) const { return m_grid[x][y]; }
    int cols() const { return m_grid.size(); }
    int rows() const { return m_grid.empty() ? 0 : m_grid[0].size(); }

private:
    QVector<QVector<Cell>> m_grid;
};

#endif /* GRID_H_INCLUDED */
