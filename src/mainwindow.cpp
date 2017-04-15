#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_grid = new Grid(m_ui->spinBoxGridSizeX->value(),
                      m_ui->spinBoxGridSizeY->value(), this);
    m_gridview = new GridView(m_grid, m_ui->canvas, this);
    connect(m_ui->spinBoxGridSizeX, SIGNAL(valueChanged(int)),
            m_grid, SLOT(setColCount(int)));
    connect(m_ui->spinBoxGridSizeY, SIGNAL(valueChanged(int)),
            m_grid, SLOT(setRowCount(int)));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
