#ifndef CELLPAINTER_H_INCLUDED
#define CELLPAINTER_H_INCLUDED

#include "gridmousetool.h"

class CellPainter : public GridMouseTool
{
    Q_OBJECT
public:
    using GridMouseTool::GridMouseTool;

    virtual void mouseMoveEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void mousePressEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void mouseReleaseEvent(QEvent *event, boost::optional<QPoint> item) override;

private:
    QPoint nearestValidCell(const QPoint& mousePosition) const;
    void plotLine(const QPoint& from, const QPoint& to);
    void plot(const QPoint& cell);

    boost::optional<QPoint> m_paintPoint;
    bool m_paintMode;
};

#endif /* CELLPAINTER_H_INCLUDED */
