#include <QWheelEvent>
#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{

}

void GraphicsView::setZoomFactor(qreal factor)
{
    if (factor < 0.1 || factor > 2.05)
        return;

    QTransform transform;
    transform.scale(factor, factor);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setTransform(transform);
    emit zoomFactorChanged(zoomFactor());
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    qreal newScale, curScale = zoomFactor();

    if (event->delta() > 0)
        newScale = curScale + 0.1;
    else
        newScale = curScale - 0.1;

    setZoomFactor(newScale);
}
