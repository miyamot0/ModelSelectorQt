
#include <QtWidgets>
#include <QTableWidgetItem>

#include "sheetdelegate.h"
#include "sheetwidget.h"
#include "commanding.h"

SheetDelegate::SheetDelegate()
{

}

void SheetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);

    if (edit) {

        SheetWidget *temp = qobject_cast <SheetWidget *>(model->parent()->parent());
        QString mNew = edit->text();
        temp->undoStack->push(new UpdateCommand(&index, model->data(index, Qt::EditRole).toString(), mNew));
        model->setData(index, edit->text(), Qt::EditRole);

        return;
    }
}
