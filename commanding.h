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
