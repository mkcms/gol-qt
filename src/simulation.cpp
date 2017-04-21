#include <QTimer>
#include <QTime>
#include <QWaitCondition>
#include <QMutex>
#include <QQueue>
#include <QMutexLocker>
#include <boost/optional.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap.hpp>
#include <QThread>
#include "simulation.h"
#include "grid.h"

static constexpr int MaxQueueSize = 512;

class ChangeSet
{
public:
    QVector<QPoint> died;
    QVector<QPoint> spawned;

    void apply(Grid *grid)
    {
        for (const QPoint& cell : spawned)
            grid->setCellStateAt(cell, true);
        for (const QPoint& cell : died)
            grid->setCellStateAt(cell, false);
    }
};

class Worker : public QThread
{
    Q_OBJECT
public:
    Worker(Grid *grid)
        : m_grid(grid->clone())
    {
        m_grid->setParent(this);
        moveToThread(this);
    }

    boost::optional<ChangeSet> pop(unsigned long waitTime = ULONG_MAX);

    void stop()
    {
        {
            QMutexLocker lock(&m_mutex);
            m_quit = true;
        }

        m_cond.wakeAll();
    }
signals:
    void exhausted();

protected:
    virtual void run() override
    {
        while (true) {
            ChangeSet cs = nextGeneration();

            if (cs.died.empty() && cs.spawned.empty()) {
                emit exhausted();
                break;
            }

            cs.apply(m_grid);

            push(cs);
            QMutexLocker lock(&m_mutex);
            if (m_quit)
                break;
        }

        stop();
    }

private:
    bool push(const ChangeSet& elem);
    ChangeSet nextGeneration();

    Grid *m_grid;
    QQueue<ChangeSet> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_quit = false;
};

// include the definitions for Worker.
#include "simulation.moc"

Simulation::Simulation(Grid *grid, QObject *parent)
    : QObject(parent),
      m_grid(grid),
      m_timer(new QTimer(this))
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(simulationStep()));
}

void Simulation::start(SimulationMode mode)
{
    if (mode == SimulationMode::Step)
        step();
    else {
        m_timer->setSingleShot(false);
        m_timer->start(0);
    }

    if (m_worker != nullptr)
        return;

    m_worker = new Worker(m_grid);
    connect(m_worker, SIGNAL(exhausted()), this, SLOT(stop()));
    connect(m_worker, SIGNAL(finished()), this, SLOT(waitForAndDeleteFinishedWorker()));

    m_worker->start();

    emit started();
}

void Simulation::step()
{
    Q_ASSERT(m_worker != nullptr);

    m_timer->setSingleShot(true);
    m_timer->start(0);
}

void Simulation::stop()
{
    Q_ASSERT(m_worker != nullptr);

    m_worker->stop();

    while (true) {
        if (auto changeset = m_worker->pop(0))
            changeset->apply(m_grid);
        else break;
    }

    m_worker->wait();
    m_worker = nullptr;
    m_timer->stop();

    emit ended();
}

void Simulation::setDelay(int millis)
{
    Q_ASSERT(millis >= 0);

    m_timer->setInterval(millis);
}

void Simulation::simulationStep()
{
    if (auto changeset = m_worker->pop())
        changeset->apply(m_grid);
}

void Simulation::waitForAndDeleteFinishedWorker()
{
    Worker *worker = qobject_cast<Worker*>(sender());

    worker->wait();
    delete worker;
}

boost::optional<ChangeSet> Worker::pop(unsigned long waitTime)
{
    QMutexLocker lock(&m_mutex);

    if (m_queue.isEmpty()) {
        if (!m_cond.wait(&m_mutex, waitTime) || m_queue.isEmpty())
            return boost::none;
    }

    auto ret = m_queue.dequeue();
    m_cond.wakeOne();
    return ret;
}

ChangeSet Worker::nextGeneration()
{
    namespace bimaps = boost::bimaps;

    ChangeSet ret;
    boost::bimap<bimaps::unordered_set_of<QPoint, std::hash<QPoint>>,
                 bimaps::multiset_of<int>> activeNeighbourCount;

    for (auto&& cell : *m_grid) {
        int count = 0;
        for (auto neighbour = m_grid->neighbourIterator(cell);
             neighbour != GridCellNeighbourIterator(); ++neighbour) {
            if (m_grid->stateAt(*neighbour))
                count++;
            else {
                auto it = activeNeighbourCount.left.find(*neighbour);
                if (it != activeNeighbourCount.left.end())
                    activeNeighbourCount.left.replace_data(it, it->second + 1);
                else
                    activeNeighbourCount.left.insert({*neighbour, 1});
            }
        }

        if (count < 2 || count > 3)
            ret.died += cell;
    }

    auto spawnedCells = activeNeighbourCount.right.equal_range(3);
    for (auto it = spawnedCells.first; it != spawnedCells.second; ++it)
        ret.spawned += it->second;

    return ret;
}

bool Worker::push(const ChangeSet& elem)
{
    QMutexLocker lock(&m_mutex);

    if (m_queue.size() == MaxQueueSize) {
        if (!m_cond.wait(&m_mutex) || m_queue.size() == MaxQueueSize)
            return false;
    }

    m_queue.enqueue(elem);
    m_cond.wakeOne();
    return true;
}
