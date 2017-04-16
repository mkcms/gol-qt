#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include <QObject>

class Grid;
class QTimer;

enum class SimulationMode
{
    Normal, Step
};

class Simulation : public QObject
{
    Q_OBJECT
public:
    Simulation(Grid *grid, QObject *parent = nullptr);

public slots:
    void start(SimulationMode mode = SimulationMode::Normal);
    void stop();
    void step();

signals:
    void started();
    void ended();

private slots:
    void simulationStep();

private:
    Grid *m_grid;
    QTimer *m_timer;
};

#endif /* SIMULATION_H_INCLUDED */
