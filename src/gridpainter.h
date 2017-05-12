#ifndef GRIDPAINTER_H_INCLUDED
#define GRIDPAINTER_H_INCLUDED

#include <QObject>
#include <QPoint>
#include <boost/optional.hpp>

class GridView;
class QKeyEvent;

class GridMouseTool : public QObject
{
    Q_OBJECT
public:
    GridMouseTool(GridView *view, QObject *parent = nullptr);

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

class GridPainter : public GridMouseTool
{
    Q_OBJECT
public:
    using GridMouseTool::GridMouseTool;

    virtual void mouseMoveEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void mousePressEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void mouseReleaseEvent(QEvent *event, boost::optional<QPoint> item) override;


private:
    boost::optional<QPoint> m_paintPoint;
    bool m_paintMode;
};

#endif /* GRIDPAINTER_H_INCLUDED */
