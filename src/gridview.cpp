#include <QGraphicsRectItem>
#include <QtGlobal>
#include <QGraphicsView>
#include "gridview.h"
#include "grid.h"

GridView::GridView(Grid *grid, QGraphicsView *view, QObject *parent)
    : QObject(parent),
      m_grid(grid),
      m_view(view)
{
    connect(grid, SIGNAL(cellAdded(QPoint)), this, SLOT(addCell(QPoint)));
    connect(grid, SIGNAL(cellRemoved(QPoint)), this, SLOT(removeCell(QPoint)));
    connect(grid, SIGNAL(cellStateChanged(QPoint,bool)), this, SLOT(cellStateChanged(QPoint,bool)));

    view->setScene(new QGraphicsScene(view));
    drawInitialGrid();
}

void GridView::drawInitialGrid()
{
    for (int i = 0; i < m_grid->cols(); ++i)
        for (int j = 0; j < m_grid->rows(); ++j)
            addCell({i, j});
}

boost::optional<QPoint> GridView::cellAtPos(const QPoint &point)
{
    if (QGraphicsItem *atPos = m_view->itemAt(point)) {
        QPoint ret = atPos->scenePos().toPoint();

        ret /= RectSize;
        return ret;
    }

    return boost::none;
}

void GridView::addCell(QPoint cell)
{
    QGraphicsScene *scene = m_view->scene();
    QGraphicsRectItem *item = scene->addRect(0, 0, RectSize, RectSize);
    item->setPos(QPointF(cell * RectSize));

    m_grid->setCellDataAt(cell, QVariant::fromValue(item));
}

void GridView::removeCell(QPoint cell)
{
    QGraphicsRectItem *item = qvariant_cast<QGraphicsRectItem*>(m_grid->dataAt(cell));

    delete item;
}

void GridView::cellStateChanged(QPoint cell, bool state)
{
    QGraphicsRectItem *item = qvariant_cast<QGraphicsRectItem*>(m_grid->dataAt(cell));

    if (state)
        item->setBrush(Qt::black);
    else
        item->setBrush(Qt::white);
}
