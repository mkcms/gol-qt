#ifndef GRIDPAINTER_H_INCLUDED
#define GRIDPAINTER_H_INCLUDED

#include "gridmousetool.h"

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
