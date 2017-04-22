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
    connect(grid, SIGNAL(rowAdded()), this, SLOT(addRow()));
    connect(grid, SIGNAL(rowRemoved()), this, SLOT(removeRow()));
    connect(grid, SIGNAL(columnAdded()), this, SLOT(addColumn()));
    connect(grid, SIGNAL(columnRemoved()), this, SLOT(removeColumn()));
    connect(grid, SIGNAL(cellStateChanged(QPoint,bool)),
            this, SLOT(handleCellStateChange(QPoint,bool)));

    view->setScene(new QGraphicsScene(view));
    drawInitialGrid();
}

void GridView::drawInitialGrid()
{
    for (int i = 0; i < m_grid->cols(); ++i)
        for (int j = 0; j < m_grid->rows(); ++j)
            addCell({i, j});
}

void GridView::addRow()
{
    for (int i = 0; i < m_grid->cols(); ++i)
        addCell({i, m_grid->rows() - 1});
}

void GridView::removeRow()
{
    for (int i = 0; i < m_grid->cols(); ++i)
        removeCell({i, m_grid->rows()});
}

void GridView::addColumn()
{
    for (int i = 0; i < m_grid->rows(); ++i)
        addCell({m_grid->cols() - 1, i});
}

void GridView::removeColumn()
{
    for (int i = 0; i < m_grid->rows(); ++i)
        removeCell({m_grid->cols(), i});
}

void GridView::addCell(QPoint cell)
{
    auto *item = m_view->scene()->addRect(0, 0, RectSize, RectSize);
    item->setPos(cell * RectSize);
    m_grid->setCellDataAt(cell, QVariant::fromValue(item));
}

void GridView::removeCell(QPoint cell)
{
    auto *item = qvariant_cast<QGraphicsRectItem*>(m_grid->dataAt(cell));
    delete item;
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

void GridView::handleCellStateChange(QPoint cell, bool state)
{
    QGraphicsRectItem *item = qvariant_cast<QGraphicsRectItem*>(m_grid->dataAt(cell));

    if (state)
        item->setBrush(Qt::black);
    else
        item->setBrush(Qt::white);
}
