#ifndef TEMPLATEMANAGER_H_INCLUDED
#define TEMPLATEMANAGER_H_INCLUDED

#include <QStandardItemModel>
#include <QDir>

class Grid;

enum TemplateManagerDataRole
{
    GridDataRole = Qt::UserRole + 1,
    FilePathDataRole
};

class TemplateManager : public QStandardItemModel
{
    Q_OBJECT
public:
    TemplateManager(QObject *parent = nullptr);

    static QDir templatesDirectory();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public slots:
    void rescanTemplates();
    bool addTemplate(QString name, Grid *grid);

private:
    void addItem(const QString& path);
    void createImageForItem(const QModelIndex& path);
};

#endif /* TEMPLATEMANAGER_H_INCLUDED */
