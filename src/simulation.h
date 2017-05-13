#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include <QObject>
#include <QPointer>
#include "grid.h"

class QTimer;
class Worker;

class Simulation : public QObject
{
    Q_OBJECT
public:
    Simulation(Grid *grid, QObject *parent = nullptr);

    bool isRunning() const { return m_worker != nullptr; }
    const Grid *preSimulationGrid() const { return m_preSimulationGrid; }
public slots:
    void startOrContinue();
    void startOrDoSingleStep();
    void stop();
    void reset();
    void setDelay(int milis);

signals:
    void started();
    void ended();

private slots:
    void simulationStep();
    void waitForAndDeleteFinishedWorker();

private:
    void startWorker();

    QPointer<Grid> m_grid;
    QTimer *m_timer;
    Worker *m_worker = nullptr;
    int m_delay = 100;
    Grid *m_preSimulationGrid = nullptr;
};

#endif /* SIMULATION_H_INCLUDED */
