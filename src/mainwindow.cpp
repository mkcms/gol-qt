#include "mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QStateMachine>
#include <QDebug>
#include "simulation.h"
#include "gridview.h"
#include "grid.h"
#include "gridpainter.h"
#include "templatemanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow)
{
    setupUI();
    m_grid = new Grid(m_ui->spinBoxGridSizeX->value(),
                      m_ui->spinBoxGridSizeY->value(), this);
    m_gridview = new GridView(m_grid, m_ui->canvas, this);
    m_simulation = new Simulation(m_grid, this);
    m_ui->listView->setModel(new TemplateManager);
    templateManager()->rescanTemplates();
    m_ui->dialSimulationSpeed->setValue(m_ui->dialSimulationSpeed->value());
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

    connect(m_grid, &Grid::sizeChanged, [this] (int cols, int rows) {
            QSignalBlocker block1(m_ui->spinBoxGridSizeX);
            QSignalBlocker block2(m_ui->spinBoxGridSizeY);
            m_ui->spinBoxGridSizeX->setValue(cols);
            m_ui->spinBoxGridSizeY->setValue(rows);
        });

    connect(m_ui->pushButtonClearGrid, SIGNAL(clicked()), m_grid, SLOT(clear()));
    connect(m_ui->pushButtonSaveGrid, SIGNAL(clicked()), this, SLOT(saveCurrentGrid()));
}

void MainWindow::onSimulationStarted()
{
    delete m_activePainter;
    m_activePainter = nullptr;
    m_ui->groupBoxGridSizeSettings->setEnabled(false);
    m_ui->pushButtonResetSimulation->setEnabled(true);
}

void MainWindow::onIdleStateEntered()
{
    m_activePainter = new GridPainter(m_gridview, this);
    m_ui->groupBoxGridSizeSettings->setEnabled(true);
    m_ui->pushButtonResetSimulation->setEnabled(m_simulation->preSimulationGrid() != nullptr);
}

void MainWindow::saveCurrentGrid()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Save grid as template"),
                                         tr("Template name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !name.isEmpty())
        if (!templateManager()->addTemplate(name, m_grid))
            QMessageBox::critical(this,
                                  tr("Error ocurred"),
                                  tr("Error ocurred when saving template file"));
}

void MainWindow::onTemplateItemActivated(const QModelIndex& index)
{
    // TODO: implement onTemplateItemActivated()
}

void MainWindow::setupStateMachine()
{
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


    idle->assignProperty(m_ui->pushButtonStartSimulation, "checked", false);
    connect(idle, SIGNAL(entered()), this, SLOT(onIdleStateEntered()));


    simulationRunning->addTransition(m_simulation, SIGNAL(ended()), idle);
    connect(simulationRunning, SIGNAL(entered()), this, SLOT(onSimulationStarted()));


    normalSimulationMode->assignProperty(m_ui->pushButtonStartSimulation, "checked", true);
    normalSimulationMode->addTransition(m_ui->pushButtonStartSimulation,
                                        SIGNAL(clicked()), doSingleStep);
    connect(normalSimulationMode, SIGNAL(entered()), m_simulation, SLOT(startOrContinue()));


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

TemplateManager *MainWindow::templateManager()
{
    return static_cast<TemplateManager*>(m_ui->listView->model());
}
