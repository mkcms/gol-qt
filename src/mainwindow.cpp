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
    setupUI();
    m_grid = new Grid(m_ui->spinBoxGridSizeX->value(),
                      m_ui->spinBoxGridSizeY->value(), this);
    m_gridview = new GridView(m_grid, m_ui->canvas, this);
    m_simulation = new Simulation(m_grid, this);
    setupStateMachine();
    setupSignalsAndSlots();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setupUI()
{
    m_ui->setupUi(this);
    QIcon repeatIcon(":/repeat.png");
    QIcon stepForwardIcon(":/step-forward.png");

    m_ui->pushButtonResetSimulation->setIcon(repeatIcon);
    m_ui->pushButtonSimulationStep->setIcon(stepForwardIcon);
}

void MainWindow::setupSignalsAndSlots()
{
    connect(m_ui->spinBoxGridSizeX, SIGNAL(valueChanged(int)),
            m_grid, SLOT(setColCount(int)));
    connect(m_ui->spinBoxGridSizeY, SIGNAL(valueChanged(int)),
            m_grid, SLOT(setRowCount(int)));

    connect(m_ui->dialSimulationSpeed, &QDial::valueChanged,
            [this] (int value) {
                value = m_ui->dialSimulationSpeed->maximum() - value;
                m_simulation->setDelay(value);
        });

    connect(m_ui->pushButtonResetSimulation, &QPushButton::clicked, [this] {
            m_ui->pushButtonResetSimulation->setEnabled(false);
        });
}

void MainWindow::onSimulationStarted()
{
    delete m_activePainter;
    m_activePainter = nullptr;
    m_ui->spinBoxGridSizeX->setEnabled(false);
    m_ui->spinBoxGridSizeY->setEnabled(false);
    m_ui->pushButtonResetSimulation->setEnabled(true);
}

void MainWindow::onIdleStateEntered()
{
    m_activePainter = new GridPainter(m_gridview, this);
    m_ui->spinBoxGridSizeX->setEnabled(true);
    m_ui->spinBoxGridSizeY->setEnabled(true);
    m_ui->pushButtonResetSimulation->setEnabled(m_simulation->preSimulationGrid() != nullptr);
}

void MainWindow::setupStateMachine()
{
    QIcon playIcon(":/play.png");
    QIcon pauseIcon(":/pause.png");

    QStateMachine *machine = new QStateMachine(this);
    QState *topLevel = new QState(machine);
    QState *idle = new QState(topLevel);
    QState *simulationRunning = new QState(topLevel);
    QState *awaitUserInteraction = new QState(simulationRunning);
    QState *doSingleStep = new QState(simulationRunning);
    QState *normalSimulationMode = new QState(simulationRunning);
    QState *resetSimulation = new QState(simulationRunning);

    topLevel->addTransition(m_ui->pushButtonStartSimulation, SIGNAL(clicked()),
                            normalSimulationMode);
    topLevel->addTransition(m_ui->pushButtonSimulationStep, SIGNAL(clicked()),
                            doSingleStep);
    topLevel->addTransition(m_ui->pushButtonResetSimulation, SIGNAL(clicked()),
                            resetSimulation);


    idle->assignProperty(m_ui->pushButtonStartSimulation, "icon", playIcon);
    idle->assignProperty(m_ui->pushButtonStartSimulation, "checked", false);
    connect(idle, SIGNAL(entered()), this, SLOT(onIdleStateEntered()));


    simulationRunning->addTransition(m_simulation, SIGNAL(ended()), idle);
    connect(simulationRunning, SIGNAL(entered()), this, SLOT(onSimulationStarted()));


    normalSimulationMode->assignProperty(m_ui->pushButtonStartSimulation, "icon", pauseIcon);
    normalSimulationMode->assignProperty(m_ui->pushButtonStartSimulation, "checked", true);
    connect(normalSimulationMode, SIGNAL(entered()), m_simulation, SLOT(startOrContinue()));


    awaitUserInteraction->assignProperty(m_ui->pushButtonStartSimulation, "icon", playIcon);
    awaitUserInteraction->assignProperty(m_ui->pushButtonStartSimulation, "checked", false);


    doSingleStep->addTransition(awaitUserInteraction);
    connect(doSingleStep, SIGNAL(entered()), m_simulation, SLOT(startOrDoSingleStep()));


    resetSimulation->addTransition(idle);
    connect(resetSimulation, SIGNAL(entered()), m_simulation, SLOT(reset()));



    QState *initial = new QState(topLevel);
    topLevel->setInitialState(initial);
    initial->addTransition(idle);
    machine->setInitialState(topLevel);
    machine->start();
}
