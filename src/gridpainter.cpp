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
    if (cell && m_mousePressed)
        view()->grid()->setCellStateAt(*cell, m_paintMode);
}

void GridPainter::mousePressEvent(QEvent *event, boost::optional<QPoint> cell)
{
    QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
    if (mevent->buttons() == Qt::LeftButton && cell) {
        m_mousePressed = true;
        m_paintMode = !view()->grid()->stateAt(*cell);
        view()->grid()->setCellStateAt(*cell, m_paintMode);
    }
}

void GridPainter::mouseReleaseEvent(QEvent *event, boost::optional<QPoint> cell)
{
    m_mousePressed = false;
}
