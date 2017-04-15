#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>
#include "ui_mainwindow.h"

class Grid;
class GridView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* = nullptr);
    virtual ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    Grid *m_grid;
    GridView *m_gridview;
};

#endif /* MAINWINDOW_H_INCLUDED */
