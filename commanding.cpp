#include "commanding.h"

#include <QtWidgets>

#include "commanding.h"
#include "sheetwidget.h"

UpdateCommand::UpdateCommand(const QModelIndex *mIndex, const QString &oldContent, QString &newContent, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mItemModelRef = mIndex->model();
    mItemIndex = mIndex;
    mNewContent = newContent;
    mOldContent = oldContent;

    mColumn = mIndex->column();
    mRow = mIndex->row();
}

void UpdateCommand::undo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mItemModelRef->parent()->parent());
    temp->table->item(mRow, mColumn)->setText(mOldContent);
}

void UpdateCommand::redo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mItemModelRef->parent()->parent());

    if (temp->table->item(mRow, mColumn) != NULL)
    {
        temp->table->item(mRow, mColumn)->setText(mNewContent);
    }
    else
    {
        temp->table->setItem(mRow, mColumn, new QTableWidgetItem(mNewContent));
    }
}

UpdateCommandBlock::UpdateCommandBlock(const QModelIndex *mIndex, const QStringList &oldContent, QStringList &newContent, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mTopLeftModelRef = mIndex->model();
    mTopLeftIndex = mIndex;
    mNewContent = newContent;
    mOldContent = oldContent;

    mLeftColumn = mIndex->column();
    mLeftRow = mIndex->row();
}

void UpdateCommandBlock::undo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mTopLeftModelRef->parent()->parent());

    for (int row = 0; row < mOldContent.count(); row++)
    {
        QStringList columns = mOldContent[row].split('\t');

        for (int col = 0; col < columns.count(); col++)
        {
            temp->table->item(mLeftRow + row, mLeftColumn + col)->setText(columns[col]);
        }
    }
}

void UpdateCommandBlock::redo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mTopLeftModelRef->parent()->parent());

    for (int row = 0; row < mNewContent.count(); row++)
    {
        QStringList columns = mNewContent[row].split('\t');

        for (int col = 0; col < columns.count(); col++)
        {
            if (temp->table->item(mLeftRow + row, mLeftColumn + col) != NULL)
            {
                temp->table->item(mLeftRow + row, mLeftColumn + col)->setText(columns[col]);
            }
            else
            {
                temp->table->setItem(mLeftRow + row, mLeftColumn + col, new QTableWidgetItem(columns[col]));
            }
        }
    }
}

UpdateCommandBlockInvert::UpdateCommandBlockInvert(const QModelIndex *mIndex, const QStringList &oldContent, QStringList &newContent, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mTopLeftModelRef = mIndex->model();
    mTopLeftIndex = mIndex;
    mNewContent = newContent;
    mOldContent = oldContent;

    mLeftColumn = mIndex->column();
    mLeftRow = mIndex->row();
}

void UpdateCommandBlockInvert::undo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mTopLeftModelRef->parent()->parent());

    for (int row = 0; row < mOldContent.count(); row++)
    {
        QStringList columns = mOldContent[row].split('\t');

        for (int col = 0; col < columns.count(); col++)
        {
            temp->table->item(mLeftRow + col, mLeftColumn + row)->setText(columns[col]);
        }
    }
}

void UpdateCommandBlockInvert::redo()
{
    SheetWidget *temp = qobject_cast <SheetWidget *>(mTopLeftModelRef->parent()->parent());

    for (int row = 0; row < mNewContent.count(); row++)
    {
        QStringList columns = mNewContent[row].split('\t');

        for (int col = 0; col < columns.count(); col++)
        {
            if (temp->table->item(mLeftRow + col, mLeftColumn + row) != NULL)
            {
                temp->table->item(mLeftRow + col, mLeftColumn + row)->setText(columns[col]);
            }
            else
            {
                temp->table->setItem(mLeftRow + col, mLeftColumn + row, new QTableWidgetItem(columns[col]));
            }
        }
    }
}
