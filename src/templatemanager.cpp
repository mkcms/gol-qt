#include <QDirIterator>
#include <QBuffer>
#include <QPainter>
#include <QTextStream>
#include <QPixmap>
#include "templatemanager.h"
#include "grid.h"

namespace
{
    Grid *loadGridFromFile(const QString &path)
    {
        QFile file{path};
        Grid *ret = nullptr;
        if (!file.open(QIODevice::ReadOnly))
            return ret;
        QTextStream in{&file};
        ret = new Grid(1, 1);

        in >> *ret;
        if (!ret->isValid()) {
            delete ret;
            ret = nullptr;
            return ret;
        }

        return ret;
    }
}

QVariant TemplateManager::data(const QModelIndex& index, int role) const
{
    if (role == GridDataRole) {
        QString path = qvariant_cast<QString>(data(index, FilePathDataRole));
        return QVariant::fromValue(loadGridFromFile(path));
    }
    return QStandardItemModel::data(index, role);
}

void TemplateManager::rescanTemplates()
{
    QDirIterator iterator{templatesDirectory().path(), QDir::Files | QDir::Readable};

    while (iterator.hasNext())
        addItem(iterator.next());
}

bool TemplateManager::addTemplate(QString name, Grid *grid)
{
    QFile f{templatesDirectory().absoluteFilePath(name)};
    if (!f.open(QIODevice::WriteOnly))
        return false;

    QTextStream out{&f};
    out << *grid;
    f.close();
    if (out.status() == QTextStream::Ok) {
        addItem(f.fileName());
        return true;
    }
    return false;
}

QDir TemplateManager::templatesDirectory()
{
    QDir dir = QDir::home();
    dir.mkdir(".gameoflife");
    dir.cd(".gameoflife");
    return dir;
}

void TemplateManager::addItem(const QString& path)
{
    QStandardItem *item = nullptr;
    QString fname = QFileInfo(path).fileName();

    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *it = this->item(i);
        if (qvariant_cast<QString>(it->data(FilePathDataRole)) == path) {
            item = it;
            break;
        }
    }

    if (!item) {
        item = new QStandardItem(fname);
        appendRow(item);
        item->setEditable(false);
    }
    setData(item->index(), QVariant::fromValue(path),
            TemplateManagerDataRole::FilePathDataRole);
    createImageForItem(item->index());
}

namespace
{
    QPixmap pixmapForGrid(Grid *grid)
    {
        constexpr int RectSize = 7, borderOffset = 5;
        QPixmap ret{grid->cols() * RectSize + borderOffset * 2,
                    grid->rows() * RectSize + borderOffset * 2};
        ret.fill(Qt::white);
        QPainter painter{&ret};

        for (const QPoint& cell : *grid) {
            painter.fillRect(cell.x() * RectSize + borderOffset,
                             cell.y() * RectSize + borderOffset,
                             RectSize, RectSize, Qt::black);
        }

        return ret;
    }

    QString encodeImage(QPixmap pixmap)
    {
        QByteArray data;
        QBuffer buffer(&data);
        pixmap.save(&buffer, "PNG", 100);
        return data.toBase64();
    }
}

void TemplateManager::createImageForItem(const QModelIndex& index)
{
    static QString tooltipFormat =
        tr(
            "<b>Name</b>: %1<br>"
            "<b>Width</b>: %2<br>"
            "<b>Height</b>: %3<br>"
            "<center><img align=\"middle\" src='data:image/png;base64, %4'></center>"
        );

    Grid *grid = qvariant_cast<Grid*>(data(index, GridDataRole));
    if (!grid)
        return;
    QPixmap pixmap{pixmapForGrid(grid)};
    if (QStandardItem *item = itemFromIndex(index)) {
        QIcon icon{pixmap};
        QString name = item->text();
        item->setIcon(icon);
        item->setText("");
        item->setToolTip(tooltipFormat.arg(name)
                         .arg(grid->cols())
                         .arg(grid->rows())
                         .arg(encodeImage(pixmap)));
    }
    delete grid;
}
