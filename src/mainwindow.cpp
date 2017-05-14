#include "mainwindow.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <QStateMachine>
#include "simulation.h"
#include "gridview.h"
#include "grid.h"
#include "cellpainter.h"
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
    setupChildObjects();
    setupSignalsAndSlots();
    setupCellPainter();
}

MainWindow::~MainWindow()
{
    delete m_gridview;
    delete m_ui;
}

void MainWindow::controlSimulation()
{
    if (sender() == m_ui->pushButtonSimulationStep) {
        m_simulation->startOrDoSingleStep();
        m_ui->pushButtonStartSimulation->setChecked(false);
        return;
    }

    if (!m_simulation->isRunning() || m_ui->pushButtonStartSimulation->isChecked())
        m_simulation->startOrContinue();
    else
        m_simulation->startOrDoSingleStep();
}

void MainWindow::saveGridAsTemplate()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Save grid as template"),
                                         tr("Template name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !name.isEmpty())
        if (!m_templateManager->addTemplate(name, m_grid))
            QMessageBox::critical(this,
                                  tr("Error ocurred"),
                                  tr("Error ocurred when saving template file"));
}

void MainWindow::setupCellPainter()
{
    delete m_currentTool;
    m_currentTool = new CellPainter(m_gridview, this);
}

void MainWindow::setupTemplatePainter()
{
    QModelIndex index = m_ui->listView->currentIndex();

    if (!index.isValid() || index == m_lastTemplatePainted) {
        emit templatePaintingDone();
        return;
    }

    delete m_currentTool;

    QVariant gridVariant = m_sortedModel->data(index, GridDataRole);
    if (!gridVariant.isValid()) {
        QMessageBox::critical(this,
                              tr("Error ocurred"),
                              tr("Error ocurred when loading template file"));
        m_currentTool = nullptr;
        emit templatePaintingDone();
        return;
    }

    Grid *grid = qvariant_cast<Grid*>(gridVariant);
    m_currentTool = new GridTemplatePainter(m_gridview, grid, this);
    grid->setParent(m_currentTool);
    connect(m_currentTool, SIGNAL(done()), this, SIGNAL(templatePaintingDone()));
    m_lastTemplatePainted = index;
    m_ui->canvas->setFocus(Qt::OtherFocusReason);
    statusBar()->showMessage(tr("Press [ESC] to quit insertion mode."));
}

void MainWindow::setupTemplateFilter(const QString& filter)
{
    bool filterIsValid = true;

    if (filter.isEmpty())
        m_sortedModel->setFilterRegExp("");
    else {
        QRegExp re{filter + ".*"};
        if (!re.isValid())
            filterIsValid = false;
        else
            m_sortedModel->setFilterRegExp(re);
    }

    QColor textColor;
    if (!filterIsValid) {
        statusBar()->showMessage(tr("Invalid regular expression"));
        textColor = Qt::red;
    }
    else {
        statusBar()->clearMessage();
        textColor = palette().color(QPalette::Text);
    }

    QPalette pal = m_ui->lineEditTemplateSearch->palette();

    pal.setColor(QPalette::Text, textColor);
    m_ui->lineEditTemplateSearch->setAutoFillBackground(true);
    m_ui->lineEditTemplateSearch->setPalette(pal);
}

void MainWindow::onSimulationStarted()
{
    delete m_currentTool;
    m_currentTool = nullptr;
    m_ui->spinBoxGridSizeX->setEnabled(false);
    m_ui->spinBoxGridSizeY->setEnabled(false);
    m_ui->pushButtonClearGrid->setEnabled(false);
    m_ui->pushButtonResetSimulation->setEnabled(true);
    m_ui->groupBoxTemplates->setEnabled(false);
}

void MainWindow::onSimulationEnded()
{
    m_ui->spinBoxGridSizeX->setEnabled(true);
    m_ui->spinBoxGridSizeY->setEnabled(true);
    m_ui->pushButtonClearGrid->setEnabled(true);
    m_ui->pushButtonResetSimulation->setEnabled(m_simulation->preSimulationGrid() != nullptr);
    m_ui->groupBoxTemplates->setEnabled(true);
    m_ui->pushButtonStartSimulation->setChecked(false);
    setupCellPainter();
}

void MainWindow::setupUI()
{
    m_ui->setupUi(this);
}

void MainWindow::setupChildObjects()
{
    m_grid = new Grid({m_ui->spinBoxGridSizeX->value(),
                m_ui->spinBoxGridSizeY->value()}, this);
    m_gridview = new GridView(m_grid, m_ui->canvas, this);
    m_simulation = new Simulation(m_grid, this);
    m_templateManager = new TemplateManager(this);
    m_sortedModel = new QSortFilterProxyModel(this);

    m_sortedModel->setSourceModel(m_templateManager);
    m_sortedModel->sort(0);
    m_ui->listView->setModel(m_sortedModel);

    new CurrentMousePositionIndicator(m_gridview, this);

    m_simulation->setDelay(m_ui->dialSimulationDelay->value());
}

void MainWindow::setupSignalsAndSlots()
{
    connect(m_ui->spinBoxGridSizeX, SIGNAL(valueChanged(int)),
            m_grid, SLOT(setColCount(int)));
    connect(m_ui->spinBoxGridSizeY, SIGNAL(valueChanged(int)),
            m_grid, SLOT(setRowCount(int)));
    connect(m_grid, &Grid::sizeChanged, [this] (const QSize& size) {
            m_ui->spinBoxGridSizeX->setValue(size.width());
            m_ui->spinBoxGridSizeY->setValue(size.height());
            m_ui->canvas->setSceneRect(m_ui->canvas->scene()->itemsBoundingRect());
        });

    connect(m_ui->pushButtonClearGrid, SIGNAL(clicked()), m_grid, SLOT(clear()));
    connect(m_ui->pushButtonSaveGrid, SIGNAL(clicked()), this, SLOT(saveGridAsTemplate()));

    connect(m_ui->lineEditTemplateSearch, SIGNAL(textChanged(const QString&)),
            this, SLOT(setupTemplateFilter(const QString&)));

    connect(m_ui->listView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(setupTemplatePainter()));

    connect(m_ui->dialSimulationDelay, SIGNAL(valueChanged(int)),
            m_simulation, SLOT(setDelay(int)));

    connect(this, &MainWindow::templatePaintingDone, [this] {
            m_lastTemplatePainted = QModelIndex();
            m_ui->listView->clearSelection();
            setupCellPainter();
        });

    connect(m_ui->spinBoxZoomAmount,
            static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [this] (double value) {
                m_ui->canvas->setZoomFactor(value / 100.0);
        });

    connect(m_ui->canvas, &GraphicsView::zoomFactorChanged, [this] (qreal factor) {
            m_ui->spinBoxZoomAmount->setValue(factor * 100);
        });

    connect(this, SIGNAL(destroyed()), m_simulation, SLOT(stop()));
    connect(m_simulation, SIGNAL(started()), this, SLOT(onSimulationStarted()));
    connect(m_simulation, SIGNAL(ended()), this, SLOT(onSimulationEnded()));

    connect(m_ui->pushButtonStartSimulation, SIGNAL(clicked()), this, SLOT(controlSimulation()));
    connect(m_ui->pushButtonSimulationStep, SIGNAL(clicked()), this, SLOT(controlSimulation()));
    connect(m_ui->pushButtonResetSimulation, &QPushButton::clicked, [this] {
            m_simulation->reset();
            m_ui->pushButtonResetSimulation->setEnabled(false);
        });
}
