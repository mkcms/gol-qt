#ifndef GRAPHICSVIEW_H_INCLUDED
#define GRAPHICSVIEW_H_INCLUDED

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = nullptr);
};


#endif /* GRAPHICSVIEW_H_INCLUDED */
