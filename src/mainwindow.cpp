#include "mainwindow.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <QStateMachine>
#include <QDebug>
#include "simulation.h"
#include "gridview.h"
#include "grid.h"
#include "gridpainter.h"
#include "templatemanager.h"
#include "templatepainter.h"

class CurrentMousePositionIndicator : public QObject
{
    Q_OBJECT
public:
    CurrentMousePositionIndicator(GridView *view, QMainWindow *parent)
        : QObject(parent),
          m_view(view)
    {
        m_view->view()->viewport()->installEventFilter(this);
    }

    virtual bool eventFilter(QObject *object, QEvent *event) override
    {
        if (auto* mouseEvent = dynamic_cast<QMouseEvent*>(event)) {
            QStatusBar *statusBar = qobject_cast<QMainWindow *>(parent())->statusBar();
            if (auto cell = m_view->cellAtPos(mouseEvent->pos()))
                statusBar->showMessage(QString("(%1, %2)").arg(cell->x()).arg(cell->y()));
            else
                statusBar->clearMessage();
        }

        return QObject::eventFilter(object, event);
    }

private:
    GridView *m_view;
};

#include "mainwindow.moc"

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
    new CurrentMousePositionIndicator(m_gridview, this);
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
    connect(this, &MainWindow::templatePaintingDone, [this] {
            m_lastTemplatePainted = QModelIndex();
            m_ui->listView->clearSelection();
        });
}

void MainWindow::onSimulationStarted()
{
    delete m_activePainter;
    m_activePainter = nullptr;
    m_ui->groupBoxGridSizeSettings->setEnabled(false);
    m_ui->pushButtonResetSimulation->setEnabled(true);
    m_ui->groupBoxTemplates->setEnabled(false);
}

void MainWindow::onIdleStateEntered()
{
    m_ui->groupBoxGridSizeSettings->setEnabled(true);
    m_ui->pushButtonResetSimulation->setEnabled(m_simulation->preSimulationGrid() != nullptr);
    m_ui->groupBoxTemplates->setEnabled(true);
}

void MainWindow::setupNormalPainter()
{
    delete m_activePainter;
    m_activePainter = new GridPainter(m_gridview, this);
}

void MainWindow::setupTemplatePainter()
{
    QModelIndex index = m_ui->listView->currentIndex();
    if (!index.isValid() || index == m_lastTemplatePainted) {
        emit templatePaintingDone();
        return;
    }

    delete m_activePainter;
    Grid *grid = qvariant_cast<Grid*>(templateManager()->data(index, GridDataRole));
    if (!grid) {
        QMessageBox::critical(this,
                              tr("Error ocurred"),
                              tr("Error ocurred when loading template file"));
        m_activePainter = nullptr;
        emit templatePaintingDone();
        return;
    }
    m_activePainter = new GridTemplatePainter(m_gridview, grid, this);
    grid->setParent(m_activePainter);
    connect(m_activePainter, SIGNAL(done()), this, SIGNAL(templatePaintingDone()));
    m_lastTemplatePainted = index;
    m_ui->canvas->setFocus(Qt::OtherFocusReason);
    statusBar()->showMessage(tr("Press [ESC] to quit insertion mode."));
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

void MainWindow::setupStateMachine()
{
    QStateMachine *machine = new QStateMachine(this);
    QState *topLevel = new QState(machine);
    QState *idle = new QState(topLevel);
    QState *paintingNormal = new QState(idle);
    QState *paintingTemplate = new QState(idle);
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

    idle->setInitialState(paintingNormal);
    idle->assignProperty(m_ui->pushButtonStartSimulation, "checked", false);
    connect(idle, SIGNAL(entered()), this, SLOT(onIdleStateEntered()));

    connect(paintingNormal, SIGNAL(entered()), this, SLOT(setupNormalPainter()));
    connect(paintingTemplate, SIGNAL(entered()), this, SLOT(setupTemplatePainter()));
    paintingTemplate->addTransition(this, SIGNAL(templatePaintingDone()), paintingNormal);
    idle->addTransition(m_ui->listView, SIGNAL(clicked(const QModelIndex&)), paintingTemplate);
    idle->addTransition(m_ui->listView, SIGNAL(activated(const QModelIndex&)), paintingTemplate);

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
