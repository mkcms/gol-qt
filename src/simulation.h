#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include <QObject>

class Grid;
class QTimer;
class Worker;

enum class SimulationMode
{
    Normal, Step
};

class Simulation : public QObject
{
    Q_OBJECT
public:
    Simulation(Grid *grid, QObject *parent = nullptr);

    bool isRunning() const { return m_worker != nullptr; }
public slots:
    void start(SimulationMode mode = SimulationMode::Normal);
    void stop();
    void step();
    void setDelay(int milis);

signals:
    void started();
    void ended();

private slots:
    void simulationStep();

private:
    Grid *m_grid;
    QTimer *m_timer;
    Worker *m_worker = nullptr;
};

#endif /* SIMULATION_H_INCLUDED */
