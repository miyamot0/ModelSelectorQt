/**
   Copyright 2017 Shawn Gilroy

   This file is part of Discounting Model Selector, Qt port.

   Discounting Model Selector is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Discounting Model Selector is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Discounting Model Selector.  If not, see http://www.gnu.org/licenses/.

   The Discounting Model Selector is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#include <QtWidgets>
#include <QTableWidgetItem>

#include "Controls/sheetwidget.h"

#include "Utilities/commanding.h"
#include "Utilities/sheetdelegate.h"

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
