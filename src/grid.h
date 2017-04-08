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

    private:
        friend class Grid;

        mutable QVariant m_data;
        bool m_state = false;
    };

    Grid(int rows, int cols, QObject *parent = nullptr);

public slots:
    void appendRows(int n);
    void appendCols(int n);
    void setSize(int rows, int cols);

    void setCellStateAt(int x, int y, bool state);

signals:
    void sizeChanged(int oldSizeX, int oldSizeY, int newSizeX, int newSizeY);
    void cellStateChanged(int x, int y, bool state);

public:
    const Cell& cellAt(int x, int y) const { return m_grid[x][y]; }
    int rows() const { return m_grid.size(); }
    int cols() const { return m_grid[0].size(); }

private:
    QVector<QVector<Cell>> m_grid;
};

#endif /* GRID_H_INCLUDED */
