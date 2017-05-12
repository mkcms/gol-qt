#ifndef GRIDMOUSETOOL_H_INCLUDED
#define GRIDMOUSETOOL_H_INCLUDED

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
    GridView *view() const { return m_view; }

protected:
    virtual void mouseMoveEvent(QEvent *event, boost::optional<QPoint> item) { }
    virtual void mousePressEvent(QEvent *event, boost::optional<QPoint> item) { }
    virtual void mouseReleaseEvent(QEvent *event, boost::optional<QPoint> item) { }
    virtual void keyPressEvent(QKeyEvent *event) { }

private:
    GridView *m_view;
};

#endif /* GRIDMOUSETOOL_H_INCLUDED */
