#ifndef MASSINDEXITEMDELEGATE_H
#define MASSINDEXITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QPainter>

class MassIndexItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	int w;
    int h;

public:
	MassIndexItemDelegate(QWidget* parent = 0);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
	bool editorEvent(QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index);

};

#endif
