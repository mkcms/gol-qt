#include <QKeyEvent>
#include <QMouseEvent>
#include <QGraphicsView>
#include "gridmousetool.h"

GridMouseTool::GridMouseTool(GridView *view, QObject *parent)
    : QObject(parent),
      m_view(view)
{
    view->view()->setMouseTracking(true);
    view->view()->viewport()->installEventFilter(this);
    view->view()->installEventFilter(this);
 }

bool GridMouseTool::eventFilter(QObject *object, QEvent *event)
{
    if (m_view && object == m_view->view()) {
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
