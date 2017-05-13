#include <QDirIterator>
#include <QBuffer>
#include <QPainter>
#include <QTextStream>
#include <QPixmap>
#include <QFileSystemWatcher>
#include <QDebug>
#include "templatemanager.h"
#include "grid.h"

class AbstractGridTemplateItem : public QStandardItem
{
public:
    AbstractGridTemplateItem(const QString& name)
        : QStandardItem(name)
    { }

    virtual Grid *grid() = 0;
    bool initializeOrUpdate();

private:
    QPixmap pixMapForGrid();
    bool initializeToolTip(QPixmap pixmap);
};

class SavedTemplateItem : public AbstractGridTemplateItem
{
public:
    SavedTemplateItem(const QString& path);

    virtual Grid *grid() override;

private:
    QString m_path;
};

class ResourceTemplateItem : public SavedTemplateItem
{
public:
    ResourceTemplateItem(const QString& path);
};

SavedTemplateItem::SavedTemplateItem(const QString& path)
    : AbstractGridTemplateItem(QFileInfo(path).fileName()),
      m_path(path)
{ }

Grid *SavedTemplateItem::grid()
{
    QFile file{m_path};
    Grid *ret = nullptr;
    qDebug() << "SavedTemplateItem::grid: open" << m_path;

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in{&file};
        ret = new Grid({1, 1});
        in >> *ret;

        if (!ret->isValid()) {
            delete ret;
            ret = nullptr;
        }
    }
    else
        qDebug() << "SavedTemplateItem::grid: open failed";

    return ret;
}

ResourceTemplateItem::ResourceTemplateItem(const QString& path)
    : SavedTemplateItem(path)
{ }

bool AbstractGridTemplateItem::initializeOrUpdate()
{
    QPixmap pixmap{pixMapForGrid()};
    if (pixmap.isNull())
        return false;

    setIcon(QIcon(pixmap));
    return initializeToolTip(pixmap);
}

QPixmap AbstractGridTemplateItem::pixMapForGrid()
{
    Grid *grid = this->grid();
    if (!grid)
        return { };

    double rectSize = 7, borderOffset = 5;
    QSizeF size{grid->cols() * rectSize + borderOffset * 2,
                grid->rows() * rectSize + borderOffset * 2};

    if (size.width() > 720 || size.height() > 720) {
        double ratioRS = rectSize / size.width(),
               ratioBO = borderOffset / size.width();

        size.scale(720, 720, Qt::KeepAspectRatio);
        rectSize = ratioRS * size.width();
        borderOffset = ratioBO * size.width();
    }

    QPixmap ret{size.toSize()};
    ret.fill(Qt::white);
    QPainter painter{&ret};

    for (const QPoint& cell : *grid)
        painter.fillRect(cell.x() * rectSize + borderOffset,
                         cell.y() * rectSize + borderOffset,
                         rectSize, rectSize, Qt::black);

    delete grid;
    return ret;
}

bool AbstractGridTemplateItem::initializeToolTip(QPixmap pixmap)
{
    static QString tooltipFormat =
        QObject::tr(
            "<b>Name</b>: %1<br>"
            "<b>Width</b>: %2<br>"
            "<b>Height</b>: %3<br>"
            "<center><img align=\"middle\" src='data:image/png;base64, %4'></center>"
            );

    Grid *grid = this->grid();
    if (!grid)
        return false;

    QByteArray encodedPixmap;
    QBuffer buffer(&encodedPixmap);
    pixmap.save(&buffer, "PNG", 100);

    setToolTip(tooltipFormat
               .arg(text())
               .arg(grid->cols())
               .arg(grid->rows())
               .arg(QString(encodedPixmap.toBase64())));

    delete grid;
    return true;
}

TemplateManager::TemplateManager(QObject *parent)
    : QStandardItemModel(parent)
{
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPath(templatesDirectory().absolutePath());
    connect(watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(scanTemplates()));
    scanTemplates();
}

QVariant TemplateManager::data(const QModelIndex& index, int role) const
{
    if (role == GridDataRole) {
        if (auto *item = static_cast<AbstractGridTemplateItem*>(itemFromIndex(index)))
            if (Grid *ret = item->grid())
                return QVariant::fromValue(ret);

        return QVariant();
    }

    return QStandardItemModel::data(index, role);
}

void TemplateManager::scanTemplates()
{
    qDebug() << "TemplateManager::scanTemplates: Rescanning templates";
    clear();

    {
        QDirIterator iterator{templatesDirectory().path(), QDir::Files | QDir::Readable};
        while (iterator.hasNext())
            addSavedItem(iterator.next());
    }

    {
        QDirIterator iterator{":/templates"};
        while (iterator.hasNext())
            addResourceItem(iterator.next());
    }
}

bool TemplateManager::addTemplate(const QString& name, Grid *grid)
{
    if (auto *existing = existingItemWithName(name))
        if (auto *resource = dynamic_cast<ResourceTemplateItem*>(existing)) {
            qWarning() << "TemplateManager::addTemplate:"
                     << "builtin template with the same name already exists:"
                     << name;
            return false;
        }

    QFile f{templatesDirectory().absoluteFilePath(name)};
    if (!f.open(QIODevice::WriteOnly))
        return false;

    QTextStream out{&f};
    out << *grid;
    f.close();

    if (out.status() == QTextStream::Ok)
        return true;

    return false;
}

QDir TemplateManager::templatesDirectory()
{
    QDir dir = QDir::home();
    dir.mkdir(".gameoflife");
    dir.cd(".gameoflife");
    return dir;
}

bool TemplateManager::addSavedItem(const QString& path, bool isResource)
{
    AbstractGridTemplateItem *item = nullptr;
    QString fname = QFileInfo(path).fileName();

    item = existingItemWithName(fname);

    if (!item) {
        if (isResource)
            item = new ResourceTemplateItem(path);
        else
            item = new SavedTemplateItem(path);
        item->setEditable(false);
        item->initializeOrUpdate();
        appendRow(item);
    }
    else {
        auto *savedItem = dynamic_cast<SavedTemplateItem*>(item);
        if (!savedItem)
            return false;
    }

    return true;
}

bool TemplateManager::addResourceItem(const QString& path)
{
    if (auto *item = existingItemWithName(QFileInfo(path).fileName()))
        removeRow(indexFromItem(item).row());

    return addSavedItem(path, true);
}

AbstractGridTemplateItem *TemplateManager::existingItemWithName(const QString& name)
{
    for (int i = 0; i < rowCount(); ++i)
        if (item(i)->text() == name)
            return static_cast<AbstractGridTemplateItem*>(item(i));

    return nullptr;
}
