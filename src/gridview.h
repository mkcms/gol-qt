#ifndef GRIDVIEW_H_INCLUDED
#define GRIDVIEW_H_INCLUDED

#include <QObject>
#include <QGraphicsRectItem>
#include "grid.h"

Q_DECLARE_METATYPE(QGraphicsRectItem*);

class QGraphicsView;

class GridView : public QObject
{
    Q_OBJECT
public:
    static constexpr int RectSize = 10;
    GridView(Grid *grid, QGraphicsView *view, QObject *parent = nullptr);

private:
    void drawInitialGrid();

public:
    Grid *grid() { return m_grid; }
    QGraphicsView *view() { return m_view; }
    QPoint cellAtPos(const QPoint &point);

private slots:
    void addCell(int x, int y);
    void removeCell(int x, int y);
    void cellStateChanged(int x, int y, bool state);

private:
    Grid *m_grid;
    QGraphicsView *m_view;
};

#endif /* GRIDVIEW_H_INCLUDED */
