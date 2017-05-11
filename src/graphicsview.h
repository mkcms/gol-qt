#ifndef GRAPHICSVIEW_H_INCLUDED
#define GRAPHICSVIEW_H_INCLUDED

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = nullptr);

    qreal zoomFactor() const { return transform().m22(); }

public slots:
    void setZoomFactor(qreal factor);

signals:
    void zoomFactorChanged(qreal factor);

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
};


#endif /* GRAPHICSVIEW_H_INCLUDED */
