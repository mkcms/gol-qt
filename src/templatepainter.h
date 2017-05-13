#ifndef TEMPLATEPAINTER_H_INCLUDED
#define TEMPLATEPAINTER_H_INCLUDED

#include <QPointer>
#include "grid.h"
#include "cellpainter.h"

class GridTemplatePainter : public GridMouseTool
{
    Q_OBJECT
public:
    GridTemplatePainter(GridView *view, Grid *template_, QObject *parent = nullptr);
    virtual ~GridTemplatePainter();

    bool isDone() const { return m_done; }

signals:
    void done();

protected:
    virtual void mouseMoveEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void mousePressEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void mouseReleaseEvent(QEvent *event, boost::optional<QPoint> item) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    enum class TemplateInsertMode { ShowPreview, HidePreview, Insert };

    void maybeHidePreview(const QPoint& cell, bool force = false);
    void drawTemplate(const QPoint& insertionPoint, TemplateInsertMode mode);

    void finish() { m_done = true; emit done(); }

    QPointer<Grid> m_template;
    QPoint m_insertionPoint;
    boost::optional<TemplateInsertMode> m_lastInsertMode;
    bool m_done = false;
};


#endif /* TEMPLATEPAINTER_H_INCLUDED */
