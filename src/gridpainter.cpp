#include <QGraphicsView>
#include <QEvent>
#include <QMouseEvent>
#include "gridpainter.h"
#include "grid.h"
#include "gridview.h"

GridPainter::GridPainter(GridView *view, QObject *parent)
    : QObject(parent),
      m_view(view)
{
    view->view()->setMouseTracking(true);
    view->view()->viewport()->installEventFilter(this);
}

bool GridPainter::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseMove && m_mousePressed) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (auto cell = m_view->cellAtPos(mouseEvent->pos()))
            m_view->grid()->setCellStateAt(cell->x(), cell->y(), m_paintMode);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (auto cell = m_view->cellAtPos(mouseEvent->pos())) {
            m_mousePressed = true;
            m_paintMode = !m_view->grid()->stateAt(cell->x(), cell->y());
            m_view->grid()->setCellStateAt(cell->x(), cell->y(), m_paintMode);
        }
    }

    if (event->type() == QEvent::MouseButtonRelease)
        m_mousePressed = false;

    return QObject::eventFilter(object, event);
}
