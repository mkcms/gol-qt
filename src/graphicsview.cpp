#include <QWheelEvent>
#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{

}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    QTransform transform;
    qreal curScale = this->transform().m22();
    qreal newScale;

    if (event->delta() > 0)
        newScale = curScale + 0.1;
    else
        newScale = curScale - 0.1;

    if (newScale < 0.2 || newScale > 2.0)
        return;

    transform.scale(newScale, newScale);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setTransform(transform);
    emit zoomFactorChanged(zoomFactor());
}
