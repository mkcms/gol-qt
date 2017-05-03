#include <QDirIterator>
#include <QTextStream>
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
        if (in.status() != QTextStream::Ok) {
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
    if (out.status() == QTextStream::Ok && f.flush()) {
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
    auto existing = findItems(fname);
    if (!existing.isEmpty())
        item = existing.front();
    else {
        item = new QStandardItem(fname);
        appendRow(item);
        item->setEditable(false);
    }
    setData(item->index(), QVariant::fromValue(path),
            TemplateManagerDataRole::FilePathDataRole);
    calculateIconForItem(path);
}

void TemplateManager::calculateIconForItem(const QString& path)
{
    // TODO: implement calculateIconForItem()
}
