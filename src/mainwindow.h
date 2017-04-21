#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>
#include "ui_mainwindow.h"

class Grid;
class GridView;
class Simulation;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* = nullptr);
    virtual ~MainWindow();

private slots:
    void onSimulationStarted();
    void onIdleStateEntered();

private:
    void setupStateMachine();

    Ui::MainWindow *m_ui;
    Grid *m_grid;
    GridView *m_gridview;
    QObject *m_activePainter = nullptr;
    Simulation *m_simulation;
};

#endif /* MAINWINDOW_H_INCLUDED */
