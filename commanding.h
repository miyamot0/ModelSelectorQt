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

#ifndef COMMANDING_H
#define COMMANDING_H

#include <QUndoCommand>
#include <QtWidgets>

class UpdateCommand : public QUndoCommand
{
public:
    enum
    {
        Id = 65536
    };

    int id() const override
    {
        return Id;
    }

    UpdateCommand(const QModelIndex *mIndex, const QString &oldContent, QString &newContent, QUndoCommand *parent = 0);

    void undo() override;
    void redo() override;

    QString mOldContent;
    QString mNewContent;

private:
    const QAbstractItemModel *mItemModelRef;
    const QModelIndex *mItemIndex;


    int mColumn = -1;
    int mRow = -1;
};

class UpdateCommandBlock : public QUndoCommand
{
public:
    enum
    {
        Id = 65535
    };

    int id() const override
    {
        return Id;
    }

    UpdateCommandBlock(const QModelIndex *mIndex, const QStringList &oldContent, QStringList &newContent, QUndoCommand *parent = 0);

    void undo() override;
    void redo() override;

    QStringList mOldContent;
    QStringList mNewContent;

private:
    const QAbstractItemModel *mTopLeftModelRef;
    const QModelIndex *mTopLeftIndex;

    int mLeftColumn = -1;
    int mLeftRow = -1;
};

class UpdateCommandBlockInvert : public QUndoCommand
{
public:
    enum
    {
        Id = 65535
    };

    int id() const override
    {
        return Id;
    }

    UpdateCommandBlockInvert(const QModelIndex *mIndex, const QStringList &oldContent, QStringList &newContent, QUndoCommand *parent = 0);

    void undo() override;
    void redo() override;

    QStringList mOldContent;
    QStringList mNewContent;

private:
    const QAbstractItemModel *mTopLeftModelRef;
    const QModelIndex *mTopLeftIndex;

    int mLeftColumn = -1;
    int mLeftRow = -1;
};

#endif // COMMANDING_H
