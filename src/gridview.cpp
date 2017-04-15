#include <QGraphicsRectItem>
#include <QtGlobal>
#include <QGraphicsView>
#include "gridview.h"

GridView::GridView(Grid *grid, QGraphicsView *view, QObject *parent)
    : QObject(parent),
      m_grid(grid),
      m_view(view)
{
    connect(grid, SIGNAL(cellAdded(int,int)), this, SLOT(addCell(int,int)));
    connect(grid, SIGNAL(cellRemoved(int,int)), this, SLOT(removeCell(int,int)));
    connect(grid, SIGNAL(cellStateChanged(int,int,bool)), this, SLOT(cellStateChanged(int,int,bool)));

    view->setScene(new QGraphicsScene(view));
    drawInitialGrid();
}

void GridView::drawInitialGrid()
{
    for (int i = 0; i < m_grid->cols(); ++i)
        for (int j = 0; j < m_grid->rows(); ++j)
            addCell(i, j);
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

void GridView::addCell(int x, int y)
{
    QGraphicsScene *scene = m_view->scene();
    QGraphicsRectItem *item = scene->addRect(0, 0, RectSize, RectSize);
    item->setPos(QPointF(x * RectSize, y * RectSize));

    m_grid->setCellDataAt(x, y, QVariant::fromValue(item));
}

void GridView::removeCell(int x, int y)
{
    QGraphicsRectItem *item = qvariant_cast<QGraphicsRectItem*>(m_grid->dataAt(x, y));

    delete item;
}

void GridView::cellStateChanged(int x, int y, bool state)
{
    QGraphicsRectItem *item = qvariant_cast<QGraphicsRectItem*>(m_grid->dataAt(x, y));

    if (state)
        item->setBrush(Qt::black);
    else
        item->setBrush(Qt::white);
}
