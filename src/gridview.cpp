#include <QGraphicsRectItem>
#include <QGraphicsView>
#include "gridview.h"

GridView::GridView(Grid *grid, QGraphicsView *view, QObject *parent)
    : QObject(parent),
      m_grid(grid),
      m_view(view)
{
    connect(grid, SIGNAL(cellAdded(int,int)), this, SLOT(addCell(int,int)));
    connect(grid, SIGNAL(cellRemoved(int,int)), this, SLOT(removeCell(int,int)));

    view->setScene(new QGraphicsScene(view));
    drawInitialGrid();
}

void GridView::drawInitialGrid()
{
    for (int i = 0; i < m_grid->cols(); ++i)
        for (int j = 0; j < m_grid->rows(); ++j)
            addCell(i, j);
}

void GridView::addCell(int x, int y)
{
    QGraphicsScene *scene = m_view->scene();
    QGraphicsRectItem *item = scene->addRect(x * RectSize, y * RectSize, RectSize, RectSize);

    m_grid->cellAt(x, y).setData(QVariant::fromValue(item));
}

void GridView::removeCell(int x, int y)
{
    const Grid::Cell &cell = m_grid->cellAt(x, y);
    QGraphicsRectItem *item = qvariant_cast<QGraphicsRectItem*>(cell.data());

    delete item;
}
