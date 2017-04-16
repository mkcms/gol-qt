#include "simulation.h"
#include <QTimer>

Simulation::Simulation(Grid *grid, QObject *parent)
    : QObject(parent),
      m_grid(grid),
      m_timer(new QTimer(this))
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(simulationStep()));
}

void Simulation::start(SimulationMode mode)
{
    m_timer->setInterval(100);
    m_timer->setSingleShot(mode == SimulationMode::Step);
    m_timer->start();
}

void Simulation::step()
{
    m_timer->setSingleShot(true);
    m_timer->start(0);
}

void Simulation::stop()
{
    m_timer->stop();
}

void Simulation::simulationStep()
{
}
