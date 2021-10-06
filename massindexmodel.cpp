#include "massindexmodel.h"
#include <QtDebug>

MassIndexModel::MassIndexModel(std::vector<MassItem*>& mis, QObject* parent)
:QAbstractListModel(parent), massitems(mis)
{
	
}

int MassIndexModel::rowCount(const QModelIndex& parent) const {
	return massitems.size();
}
QVariant MassIndexModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid())
        return QVariant();

    if (index.row() >= massitems.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return massitems[index.row()]->getName();
    else
        return QVariant();

}
bool MassIndexModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {

        dataChanged(index, index);
        return true;
    }
 
    return false;
}
bool MassIndexModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        //labels.insert(position, "");
    }

    endInsertRows();
    return true;
 
}
bool MassIndexModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        //labels.removeAt(position);
    }

    endRemoveRows();
    return true;
 
}

void MassIndexModel::setActive(const QModelIndex &index, bool a) const {
	if(index.isValid() && index.row()<massitems.size()) {
		massitems[index.row()]->setActive(a);
	}
}
bool MassIndexModel::getActive(const QModelIndex &index) const {
	if(index.isValid()) {
		return massitems[index.row()]->isActive();
	}
	return false;
}
void MassIndexModel::setVisible(const QModelIndex &index, bool a) const {
	if(index.isValid() && index.row()<massitems.size()) {
		massitems[index.row()]->setVisible(a);
	}
}
bool MassIndexModel::getVisible(const QModelIndex &index) const {
	if(index.isValid()) {
		return massitems[index.row()]->isVisible();
	}
	return false;
}
void MassIndexModel::setColor(const QModelIndex &index, QColor c) const {
	if(index.isValid() && index.row()<massitems.size()) {
		massitems[index.row()]->setColor(c);
	}
}
QColor MassIndexModel::getColor(const QModelIndex &index) const {
	if(index.isValid()) {
		return massitems[index.row()]->getColor();
	}
	return QColor(170,170,170);
}
QString MassIndexModel::getType(const QModelIndex &index) const {
	if(index.isValid()) {
		return massitems[index.row()]->getType();
	}
	return "";
}