#include <QGraphicsView>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include "gridpainter.h"
#include "grid.h"
#include "gridview.h"

GridEventFilter::GridEventFilter(GridView *view, QObject *parent)
    : QObject(parent),
      m_view(view)
{
    view->view()->setMouseTracking(true);
    view->view()->viewport()->installEventFilter(this);
    view->view()->installEventFilter(this);
 }

bool GridEventFilter::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_view->view()) {
        if (event->type() == QEvent::KeyPress)
            keyPressEvent(static_cast<QKeyEvent*>(event));
        return QObject::eventFilter(object, event);
    }

    auto getCellAtPos = [this, event] {
        return m_view->cellAtPos(static_cast<QMouseEvent*>(event)->pos());
    };

    switch (event->type()) {
    case QEvent::MouseMove: mouseMoveEvent(event, getCellAtPos()); break;
    case QEvent::MouseButtonPress: mousePressEvent(event, getCellAtPos()); break;
    case QEvent::MouseButtonRelease: mouseReleaseEvent(event, getCellAtPos()); break;
    default: break;
    }

    return QObject::eventFilter(object, event);
}

void GridPainter::mouseMoveEvent(QEvent *event, boost::optional<QPoint> cell)
{
    if (m_paintPoint) {
        QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
        Grid *grid = view()->grid();
        if (!cell) {
            QPoint p = view()->view()->mapToScene(mevent->pos()).toPoint();

            if (p.x() < 0)
                p.rx() = 0;
            if (p.x() / GridView::RectSize >= grid->cols())
                p.rx() = (grid->cols() - 1) * GridView::RectSize;
            if (p.y() < 0)
                p.ry() = 0;
            if (p.y() / GridView::RectSize >= grid->rows())
                p.ry() = (grid->rows() - 1) * GridView::RectSize;

            cell = view()->cellAtPos(view()->view()->mapFromScene(p));
        }

        auto plot = [this] (const QPoint& pos) {
            view()->grid()->setCellStateAt(pos, m_paintMode);
        };

        QPoint delta = *cell - *m_paintPoint;
        int deltax = delta.x(), deltay = delta.y();

        int x = m_paintPoint->x(), y = m_paintPoint->y(),
            incx = x < cell->x() ? 1 : -1,
            incy = y < cell->y() ? 1 : -1;

        if (deltax != 0) {
            double deltaerr = qAbs(double(deltay) / deltax);
            double error = deltaerr - 0.5;

            while (x != cell->x()) {
                plot({x, y});
                error += deltaerr;
                if (error >= 0.5) {
                    y += incy;
                    error -= 1.0;
                }
                x += incx;
            }
        }

        while (y != cell->y()) {
            plot({x, y});
            y += incy;
        }

        m_paintPoint = cell;
    }
}

void GridPainter::mousePressEvent(QEvent *event, boost::optional<QPoint> cell)
{
    QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
    if (mevent->buttons() == Qt::LeftButton && cell) {
        m_paintPoint = cell;
        m_paintMode = !view()->grid()->stateAt(*cell);
        view()->grid()->setCellStateAt(*cell, m_paintMode);
    }
}

void GridPainter::mouseReleaseEvent(QEvent *event, boost::optional<QPoint> cell)
{
    m_paintPoint.reset();
}
