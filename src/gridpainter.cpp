#include <QGraphicsView>
#include <QEvent>
#include <QMouseEvent>
#include "gridpainter.h"

GridPainter::GridPainter(GridView *view, QObject *parent)
    : QObject(parent),
      m_view(view)
{
    view->view()->setMouseTracking(true);
    view->view()->viewport()->installEventFilter(this);
}

bool GridPainter::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint cell = m_view->cellAtPos(mouseEvent->pos());
        if (cell.x() != -1 && m_mousePressed)
            m_view->grid()->setCellStateAt(cell.x(), cell.y(), m_paintMode);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint cell = m_view->cellAtPos(mouseEvent->pos());
        if (cell.x() != -1) {
            m_mousePressed = true;
            m_paintMode = !m_view->grid()->cellAt(cell.x(), cell.y()).state();
            m_view->grid()->setCellStateAt(cell.x(), cell.y(), m_paintMode);
        }
    }

    if (event->type() == QEvent::MouseButtonRelease)
        m_mousePressed = false;

    return false;
}
