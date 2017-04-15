#ifndef GRIDPAINTER_H_INCLUDED
#define GRIDPAINTER_H_INCLUDED

#include <QObject>

class GridView;

class GridPainter : public QObject
{
    Q_OBJECT
public:
    GridPainter(GridView *view, QObject *parent = nullptr);

    virtual bool eventFilter(QObject *object, QEvent *event) override;

private:
    GridView *m_view;
    bool m_mousePressed = false;
    bool m_paintMode;
};

#endif /* GRIDPAINTER_H_INCLUDED */
