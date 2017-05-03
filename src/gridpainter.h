#ifndef GRIDPAINTER_H_INCLUDED
#define GRIDPAINTER_H_INCLUDED

#include <QObject>
#include <QPoint>
#include <boost/optional.hpp>

class GridView;
class QKeyEvent;

class GridEventFilter : public QObject
{
    Q_OBJECT
public:
    GridEventFilter(GridView *view, QObject *parent = nullptr);

    virtual bool eventFilter(QObject *object, QEvent *event) override;
    GridView *view() { return m_view; }

protected:
    virtual void mouseMoveEvent(QEvent *event, boost::optional<QPoint> item) { }
    virtual void mousePressEvent(QEvent *event, boost::optional<QPoint> item) { }
    virtual void mouseReleaseEvent(QEvent *event, boost::optional<QPoint> item) { }
    virtual void keyPressEvent(QKeyEvent *event) { }

private:
    GridView *m_view;
};

class GridPainter : public GridEventFilter
{
    Q_OBJECT
public:
    using GridEventFilter::GridEventFilter;

    virtual void mouseMoveEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void mousePressEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void mouseReleaseEvent(QEvent *event, boost::optional<QPoint> item) override;


private:
    bool m_mousePressed = false;
    bool m_paintMode;
};

#endif /* GRIDPAINTER_H_INCLUDED */
