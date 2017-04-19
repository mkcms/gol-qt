#ifndef GRIDVIEW_H_INCLUDED
#define GRIDVIEW_H_INCLUDED

#include <QObject>
#include <boost/optional.hpp>
#include <QGraphicsRectItem>

Q_DECLARE_METATYPE(QGraphicsRectItem*);

class QGraphicsView;
class Grid;

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
    boost::optional<QPoint> cellAtPos(const QPoint &point);

private slots:
    void addCell(QPoint cell);
    void removeCell(QPoint cell);
    void cellStateChanged(QPoint cell, bool state);

private:
    Grid *m_grid;
    QGraphicsView *m_view;
};

#endif /* GRIDVIEW_H_INCLUDED */
