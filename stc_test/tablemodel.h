#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVector>
#include <QVariant>

struct Texteditor {
    // Structure fields are named as tags in xml
    QString texteditor;
    QString fileformats;
    QString encoding;
    QString hasintellisense;
    QString hasplugins;
    QString cancompile;

    // Objects are equal if their names are equal
    bool operator==(const Texteditor& other) const {
        return (texteditor == other.texteditor);
    }
};

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(QObject *parent = nullptr);
    TableModel(const QVector<Texteditor>& texteditors, QObject *parent = nullptr);

    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags (const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
    const QVector<Texteditor>& getTexteditors();

    void resetModel();
    void setFields(const Texteditor& editor);

private:
    QVector<Texteditor> texteditors;
};

#endif // TABLEMODEL_H
