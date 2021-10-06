#ifndef MASSINDEXMODEL_H
#define MASSINDEXMODEL_H
#include <QAbstractListModel>
#include "massitem.h"

class MassIndexModel : public QAbstractListModel
{
	Q_OBJECT

	std::vector<MassItem*>& massitems;

public:
	MassIndexModel(std::vector<MassItem*>& mis, QObject* parent);

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

	void setActive(const QModelIndex &index, bool a) const;
	bool getActive(const QModelIndex &index) const;
	void setVisible(const QModelIndex &index, bool a) const;
	bool getVisible(const QModelIndex &index) const;
	void setColor(const QModelIndex &index, QColor c) const;
	QColor getColor(const QModelIndex &index) const;

	QString getType(const QModelIndex &index) const;
};

#endif