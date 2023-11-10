#include "tablemodel.h"

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

TableModel::TableModel(const QVector<Texteditor> &texteditors, QObject *parent)
    :QAbstractTableModel(parent), texteditors(texteditors)
{

}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : texteditors.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 6;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= texteditors.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const auto& texteditor = texteditors.at(index.row());

        switch (index.column()) {
        case 0:
            return texteditor.texteditor;
        case 1:
            return texteditor.fileformats;
        case 2:
            return texteditor.encoding;
        case 3:
            return texteditor.hasintellisense;
        case 4:
            return texteditor.hasplugins;
        case 5:
            return texteditor.cancompile;
        default:
            break;
        }
    }

    if (role == Qt::TextAlignmentRole )
        return Qt::AlignCenter;

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Text Editor");
        case 1:
            return tr("File Formats");
        case 2:
            return tr("Encoding");
        case 3:
            return tr("Has Intellisense");
        case 4:
            return tr("Has Plugins");
        case 5:
            return tr("Can Compile");
        default:
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        auto texteditor = texteditors.value(row);

        switch (index.column()) {
        case 0:
            texteditor.texteditor = value.toString();
            break;
        case 1:
            texteditor.fileformats = value.toString();
            break;
        case 2:
            texteditor.encoding = value.toString();
            break;
        case 3:
            texteditor.hasintellisense = value.toString();
            break;
        case 4:
            texteditor.hasplugins = value.toString();
            break;
        case 5:
            texteditor.cancompile = value.toString();
            break;
        default:
            return false;
        }

        texteditors.replace(row, texteditor);
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

        return true;
    }

    return false;
}

bool TableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index)
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        texteditors.insert(position, {QString(), QString(), QString(), QString(), QString(), QString()});

    endInsertRows();
    return  true;
}

bool TableModel::removeRows(int position, int rows, const QModelIndex &index)
{
     Q_UNUSED(index)
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        texteditors.removeAt(position);

    endRemoveRows();

    return true;
}

const QVector<Texteditor> &TableModel::getTexteditors()
{
    return texteditors;
}

void TableModel::resetModel()
{
    this->beginResetModel();
    this->endResetModel();
}

void TableModel::setFields(const Texteditor &editor)
{
    QModelIndex index = this->index(0, 0, QModelIndex());
    this->setData(index, editor.texteditor, Qt::EditRole);

    index = this->index(0, 1, QModelIndex());
    this->setData(index, editor.encoding, Qt::EditRole);

    index = this->index(0, 2, QModelIndex());
    this->setData(index, editor.fileformats, Qt::EditRole);

    index = this->index(0, 3, QModelIndex());
    this->setData(index, editor.hasintellisense, Qt::EditRole);

    index = this->index(0, 4, QModelIndex());
    this->setData(index, editor.hasplugins, Qt::EditRole);

    index = this->index(0, 5, QModelIndex());
    this->setData(index, editor.cancompile, Qt::EditRole);
}

