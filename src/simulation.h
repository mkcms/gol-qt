#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include <QObject>

class Grid;
class QTimer;
class Worker;

class Simulation : public QObject
{
    Q_OBJECT
public:
    Simulation(Grid *grid, QObject *parent = nullptr);

    bool isRunning() const { return m_worker != nullptr; }
public slots:
    void startOrContinue();
    void startOrDoSingleStep();
    void stop();
    void setDelay(int milis);

signals:
    void started();
    void ended();

private slots:
    void simulationStep();
    void waitForAndDeleteFinishedWorker();

private:
    void startWorker();

    Grid *m_grid;
    QTimer *m_timer;
    Worker *m_worker = nullptr;
    int m_delay = 100;
};

#endif /* SIMULATION_H_INCLUDED */
