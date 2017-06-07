#ifndef SHEETDELEGATE_H
#define SHEETDELEGATE_H

#include <QObject>
#include <QAbstractTableModel>
#include <QItemDelegate>

class SheetDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    SheetDelegate();
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

#endif // SHEETDELEGATE_H
