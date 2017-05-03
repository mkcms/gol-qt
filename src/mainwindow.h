#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>
#include <QModelIndex>
#include "ui_mainwindow.h"

class Grid;
class GridView;
class Simulation;
class TemplateManager;
class GridEventFilter;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* = nullptr);
    virtual ~MainWindow();

private slots:
    void onSimulationStarted();
    void onIdleStateEntered();
    void saveCurrentGrid();
    void setupNormalPainter();
    void setupTemplatePainter();

signals:
    void templatePaintingDone();

private:
    void setupUI();
    void setupSignalsAndSlots();
    void setupStateMachine();
    TemplateManager *templateManager();

    Ui::MainWindow *m_ui;
    Grid *m_grid;
    GridView *m_gridview;
    GridEventFilter *m_activePainter = nullptr;
    Simulation *m_simulation;
    QModelIndex m_lastTemplatePainted;
};

#endif /* MAINWINDOW_H_INCLUDED */
