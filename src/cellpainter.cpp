#include <QGraphicsView>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include "cellpainter.h"
#include "grid.h"
#include "gridview.h"

void CellPainter::mouseMoveEvent(QEvent *event, boost::optional<QPoint> cell)
{
    if (m_paintPoint) {
        if (!cell) {
            QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
            cell = nearestValidCell(mevent->pos());
        }

        plotLine(*m_paintPoint, *cell);
        m_paintPoint = cell;
    }
}

void CellPainter::mousePressEvent(QEvent *event, boost::optional<QPoint> cell)
{
    QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
    if (mevent->buttons() == Qt::LeftButton && cell) {
        m_paintPoint = cell;
        m_paintMode = !view()->grid()->stateAt(*cell);
        plot(*cell);
    }
}

void CellPainter::mouseReleaseEvent(QEvent *event, boost::optional<QPoint> cell)
{
    m_paintPoint.reset();
}

QPoint CellPainter::nearestValidCell(const QPoint& mousePosition) const
{
    Grid *grid = view()->grid();
    QPoint p = view()->view()->mapToScene(mousePosition).toPoint();

    if (p.x() < 0)
        p.rx() = 0;
    if (p.x() / GridView::RectSize >= grid->cols())
        p.rx() = (grid->cols() - 1) * GridView::RectSize;
    if (p.y() < 0)
        p.ry() = 0;
    if (p.y() / GridView::RectSize >= grid->rows())
        p.ry() = (grid->rows() - 1) * GridView::RectSize;

    return *view()->cellAtPos(view()->view()->mapFromScene(p));
}

void CellPainter::plotLine(const QPoint& from, const QPoint& to)
{
    QPoint delta = to - from;
    int deltax = delta.x(), deltay = delta.y(),
        x = from.x(), y = from.y(),
        incx = x < to.x() ? 1 : -1,
        incy = y < to.y() ? 1 : -1;

    if (deltax != 0) {
        double deltaerr = qAbs(double(deltay) / deltax);
        double error = deltaerr - 0.5;

        while (x != to.x()) {
            plot({x, y});
            error += deltaerr;
            if (error >= 0.5) {
                y += incy;
                error -= 1.0;
            }
            x += incx;
        }
    }

    while (y != to.y()) {
        plot({x, y});
        y += incy;
    }
}

void CellPainter::plot(const QPoint& cell)
{
    view()->grid()->setCellStateAt(cell, m_paintMode);
}
