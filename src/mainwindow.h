#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* = nullptr);
    virtual ~MainWindow();

private:
    Ui::MainWindow *m_ui;
};

#endif /* MAINWINDOW_H_INCLUDED */
