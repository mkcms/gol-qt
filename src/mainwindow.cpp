#include "mainwindow.h"
#include <QStateMachine>
#include <QDebug>
#include "simulation.h"
#include "gridview.h"
#include "grid.h"
#include "gridpainter.h"

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
    m_simulation = new Simulation(m_grid, this);

    setupStateMachine();
    QIcon repeatIcon(":/repeat.png");
    QIcon stepForwardIcon(":/step-forward.png");

    m_ui->pushButtonResetSimulation->setIcon(repeatIcon);
    m_ui->pushButtonSimulationStep->setIcon(stepForwardIcon);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setupStateMachine()
{
    QIcon playIcon(":/play.png");
    QIcon pauseIcon(":/pause.png");

    QStateMachine *machine = new QStateMachine(this);
    QState *idle = new QState(machine);
    QState *simulationRunning = new QState(machine);
    QState *awaitUserInteraction = new QState(simulationRunning);
    QState *doSingleStep = new QState(simulationRunning);
    QState *normalSimulationMode = new QState(simulationRunning);

    machine->setInitialState(idle);

    idle->addTransition(m_ui->pushButtonStartSimulation, SIGNAL(clicked()),
                        normalSimulationMode);
    idle->addTransition(m_ui->pushButtonSimulationStep, SIGNAL(clicked()),
                        doSingleStep);

    idle->assignProperty(m_ui->pushButtonStartSimulation, "icon", playIcon);
    idle->assignProperty(m_ui->pushButtonStartSimulation, "checked", false);
    connect(idle, &QState::entered, [this] {
            m_activePainter = new GridPainter(m_gridview, this);
            m_ui->spinBoxGridSizeX->setEnabled(true);
            m_ui->spinBoxGridSizeY->setEnabled(true);
        });
    connect(idle, &QState::exited, [this] {
            delete m_activePainter;
            m_activePainter = nullptr;
            m_ui->spinBoxGridSizeX->setEnabled(false);
            m_ui->spinBoxGridSizeY->setEnabled(false);
        });

    simulationRunning->addTransition(m_ui->pushButtonSimulationStep,
                                     SIGNAL(clicked()), doSingleStep);


    normalSimulationMode->assignProperty(m_ui->pushButtonStartSimulation, "icon", pauseIcon);
    normalSimulationMode->assignProperty(m_ui->pushButtonStartSimulation, "checked", true);
    connect(normalSimulationMode, SIGNAL(entered()), m_simulation, SLOT(startOrContinue()));
    normalSimulationMode->addTransition(m_ui->pushButtonStartSimulation, SIGNAL(clicked()),
                                        doSingleStep);


    awaitUserInteraction->assignProperty(m_ui->pushButtonStartSimulation, "icon", playIcon);
    awaitUserInteraction->assignProperty(m_ui->pushButtonStartSimulation, "checked", false);
    awaitUserInteraction->addTransition(m_ui->pushButtonStartSimulation,
                                        SIGNAL(clicked()), normalSimulationMode);

    connect(doSingleStep, &QState::entered,
        m_simulation, &Simulation::startOrDoSingleStep);
    doSingleStep->addTransition(awaitUserInteraction);

    simulationRunning->addTransition(m_simulation, SIGNAL(ended()), idle);

    machine->start();
}
