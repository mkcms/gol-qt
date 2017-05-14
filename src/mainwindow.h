#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include "ui_mainwindow.h"

class Grid;
class GridView;
class Simulation;
class TemplateManager;
class GridMouseTool;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* = nullptr);
    virtual ~MainWindow();

private slots:
    void controlSimulation();

    void setupCellPainter();
    void setupTemplatePainter();
    void setupTemplateFilter(const QString& filter);

    void onSimulationStarted();
    void onSimulationEnded();

    void saveGridAsTemplate();

signals:
    void templatePaintingDone();

private:
    void setupUI();
    void setupChildObjects();
    void setupSignalsAndSlots();

    QAbstractItemModel *templateListModel()
    {
        return m_ui->listView->model();
    }

    Ui::MainWindow *m_ui;
    Grid *m_grid;
    GridView *m_gridview;
    GridMouseTool *m_currentTool = nullptr;
    Simulation *m_simulation;
    QModelIndex m_lastTemplatePainted;
    TemplateManager *m_templateManager;
    QSortFilterProxyModel *m_sortedModel;
};

#endif /* MAINWINDOW_H_INCLUDED */
