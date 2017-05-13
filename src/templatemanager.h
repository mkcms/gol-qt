#ifndef TEMPLATEMANAGER_H_INCLUDED
#define TEMPLATEMANAGER_H_INCLUDED

#include <QStandardItemModel>
#include <QDir>

class Grid;
class AbstractGridTemplateItem;

enum TemplateManagerDataRole
{
    GridDataRole = Qt::UserRole + 1
};

class TemplateManager : public QStandardItemModel
{
    Q_OBJECT
public:
    TemplateManager(QObject *parent = nullptr);

    static QDir templatesDirectory();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public slots:
    bool addTemplate(const QString& name, Grid *grid);

private slots:
    void scanTemplates();

private:
    AbstractGridTemplateItem *existingItemWithName(const QString& name);
    bool addSavedItem(const QString& path);
};

#endif /* TEMPLATEMANAGER_H_INCLUDED */
