#include <QKeyEvent>
#include "templatepainter.h"
#include "gridview.h"

GridTemplatePainter::GridTemplatePainter(GridView *view, Grid *template_, QObject *parent)
    : GridMouseTool(view, parent),
      m_template(template_)
{
    auto deinitialize = [this] { m_lastInsertMode = boost::none; };
    connect(view->grid(), &Grid::destroyed, deinitialize);
    connect(view, &GridView::destroyed, deinitialize);
}

GridTemplatePainter::~GridTemplatePainter()
{
    if (!isDone()) {
        maybeHidePreview(m_insertionPoint, true);
        finish();
    }
}

void GridTemplatePainter::mouseMoveEvent(QEvent *event, boost::optional<QPoint> item)
{
    if (isDone() || !item)
        return;

    maybeHidePreview(*item);
    drawTemplate(*item, TemplateInsertMode::ShowPreview);
}

void GridTemplatePainter::mousePressEvent(QEvent *event, boost::optional<QPoint> item)
{
    QMouseEvent *mevent = static_cast<QMouseEvent*>(event);
    if (isDone() || !item || mevent->buttons() != Qt::LeftButton)
        return;

    maybeHidePreview(*item);
    drawTemplate(*item, TemplateInsertMode::Insert);
    finish();
}

void GridTemplatePainter::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        if (!isDone()) {
            maybeHidePreview(m_insertionPoint, true);
            finish();
        }
}

void GridTemplatePainter::mouseReleaseEvent(QEvent *event, boost::optional<QPoint> item)
{
    if (m_lastInsertMode && *m_lastInsertMode == TemplateInsertMode::Insert)
        emit done();
}

void GridTemplatePainter::maybeHidePreview(const QPoint& cell, bool force)
{
    if (m_lastInsertMode && *m_lastInsertMode == TemplateInsertMode::ShowPreview) {
        if (m_insertionPoint == cell && !force)
            return;
        drawTemplate(m_insertionPoint, TemplateInsertMode::HidePreview);
    }
}

void GridTemplatePainter::drawTemplate(const QPoint& insertionPoint, TemplateInsertMode mode)
{
    auto cellIsValid = [this] (const QPoint& cell) {
        return cell.x() >= 0 && cell.y() >= 0
        && cell.x() < view()->grid()->cols()
        && cell.y() < view()->grid()->rows();
    };

    for (int i = 0; i < m_template->cols(); ++i) {
        for (int j = 0; j < m_template->rows(); ++j) {
            QPoint cell{i, j};
            bool templateState = m_template->stateAt(cell);

            cell += insertionPoint;
            if (!cellIsValid(cell))
                continue;

            if (mode == TemplateInsertMode::ShowPreview)
                view()->setVisibleCellState(cell, templateState);
            if (mode == TemplateInsertMode::HidePreview)
                view()->setVisibleCellState(cell, view()->grid()->stateAt(cell));
            if (mode == TemplateInsertMode::Insert)
                view()->grid()->setCellStateAt(cell, templateState);
        }
    }

    m_insertionPoint = insertionPoint;
    m_lastInsertMode = mode;
}
