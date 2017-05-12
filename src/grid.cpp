#include <boost/functional/hash.hpp>
#include <QDebug>
#include "grid.h"

template <typename Container>
void writePoints(const Container& points, QTextStream& stream)
{
    stream << points.size();
    for (const QPoint& pt : points)
        stream << "\n" << pt.x() << " " << pt.y();
    stream << "\n";
}

uint qHash(const QPoint& key)
{
    return std::hash<QPoint>()(key);
}

std::size_t std::hash<QPoint>::operator()(const QPoint& key) const
{
    std::size_t seed = 12345;
    boost::hash_combine(seed, key.x());
    boost::hash_combine(seed, key.y());
    return static_cast<uint>(seed);
}

namespace {
    int readUnsignedInt(QTextStream& stream)
    {
        int ret;
        QString str;
        bool ok;

        stream >> str;
        ret = str.toInt(&ok);

        if (!ok)
            stream.setStatus(QTextStream::ReadCorruptData);
        return ret;
    }
}

Grid::Grid(const QSize& size, QObject *parent)
    : QObject(parent)
{
    setSize(size);
}

Grid * Grid::clone() const
{
    Grid *ret = new Grid(m_size);
    ret->m_activeCells = m_activeCells;
    // data not copied.
    return ret;
}

void Grid::copyStateFrom(const Grid *grid)
{
    if (!grid->isValid())
        return;
    setSize(grid->m_size);
    auto oldcells = m_activeCells;
    for (auto&& cell : oldcells)
        setCellStateAt(cell, false);
    for (auto&& cell : *grid)
        setCellStateAt(cell, true);
}

void Grid::setSize(const QSize& size)
{
    Q_ASSERT(size.isValid() && size.width() * size.height() != 0);
    if (size == m_size)
        return;
    if (!isValid())
        m_size = {0, 0};
    int rows = size.height(), cols = size.width();

    if (cols > this->cols())
        m_data.resize(cols);

    while (cols > this->cols()) {
        m_size.rwidth()++;
        emit columnAdded();
    }

    while (cols < this->cols()) {
        m_size.rwidth()--;
        emit columnRemoved();
        for (int i = 0; i < this->rows(); ++i) {
            m_activeCells -= {this->cols(), i};
            m_data[this->cols()].remove(i);
        }
    }

    m_data.resize(cols);

    while (rows > this->rows()) {
        m_size.rheight()++;
        emit rowAdded();
    }

    while (rows < this->rows()) {
        m_size.rheight()--;
        emit rowRemoved();
        for (int i = 0; i < this->cols(); ++i) {
            m_activeCells -= {i, this->rows()};
            m_data[i].remove(this->rows());
        }
    }

    emit sizeChanged(m_size);
}

void Grid::setCellStateAt(const QPoint& cell, bool state)
{
    if (stateAt(cell) == state)
        return;

    if (state)
        m_activeCells += cell;
    else
        m_activeCells -= cell;

    emit cellStateChanged(cell, state);
}

void Grid::setCellDataAt(const QPoint& cell, const QVariant& data)
{
    m_data[cell.x()].insert(cell.y(), data);
}

void Grid::clear()
{
    auto state = m_activeCells;
    for (const QPoint& cell : state)
        setCellStateAt(cell, false);
}

void Grid::invalidate()
{
    while (cols() > 0) {
        m_size.rwidth()--;
        emit columnRemoved();
    }
    while (rows() > 0) {
        m_size.rheight()--;
        emit rowRemoved();
    }
    m_size = {-1, -1};
    m_data.clear();
    m_activeCells.clear();
}

QTextStream& operator<<(QTextStream& out, const Grid& grid)
{
    if (grid.m_activeCells.isEmpty())
        return out << 1 << " " << 1 << "\n" << 0 << "\n";

    int maxX = 0, minX = INT_MAX, maxY = 0, minY = INT_MAX;
    for (const QPoint& pt : grid.m_activeCells) {
        if (pt.x() > maxX)
            maxX = pt.x();
        if (pt.x() < minX)
            minX = pt.x();
        if (pt.y() > maxY)
            maxY = pt.y();
        if (pt.y() < minY)
            minY = pt.y();
    }

    QSet<QPoint> activeCells;
    for (const QPoint& pt : grid.m_activeCells)
        activeCells += pt - QPoint{minX, minY};

    int cols = maxX - minX + 1, rows = maxY - minY + 1;
    out << cols << " " << rows << "\n";
    writePoints(activeCells, out);
    return out << "\n";
}

QTextStream& operator>>(QTextStream& out, Grid& grid)
{
    int cols = readUnsignedInt(out);
    int rows = readUnsignedInt(out);

    grid.clear();
    if (out.status() != QTextStream::Ok
        || cols <= 0 || rows <= 0) {
        qWarning() << "Read corrupted data or IO error occurred: size read = {"
                   << cols << "," << rows << "}, stream status = " << out.status();
        grid.invalidate();
        return out;
    }

    Grid g({cols, rows});
    g.readPoints(out);
    if (!g.isValid()) {
        out.setStatus(QTextStream::ReadCorruptData);
        grid.invalidate();
        return out;
    }

    grid.copyStateFrom(&g);
    return out;
}

void Grid::readPoints(QTextStream& stream)
{
    bool valid = true;
    int n = readUnsignedInt(stream);
    if (n <= 0)
        valid = false;
    while (n-- > 0 && stream.status() == QTextStream::Ok) {
        int x = readUnsignedInt(stream);
        int y = readUnsignedInt(stream);
        if (stream.status() != QTextStream::Ok
            || x < 0 || y < 0 || x >= cols() || y >= rows()) {
            qWarning() << "Read corrupted data or IO error occurred: point = {"
                       << x << "," << y
                       << "}, grid size = " << m_size
                       << "), stream status = " << stream.status();
            valid = false;
            break;
        }
        m_activeCells += QPoint{x, y};
    }

    if (!valid || stream.status() != QTextStream::Ok)
        invalidate();
}
