#include "massindexitemdelegate.h"
#include "massindexmodel.h"
#include <QModelIndex>
#include <QPainter>
#include <QMouseEvent>
#include <QtDebug>
#include <QColorDialog>

MassIndexItemDelegate::MassIndexItemDelegate(QWidget* parent)
:QStyledItemDelegate(parent), w(100), h(20)
{


}	
void MassIndexItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex& index) const {
	QStyledItemDelegate::paint(painter, option, QModelIndex()); //note here that I am passing an invalid index, QModelIndex(). This is to get the pretty selection paint, without the default display of data.

	//Code duplication with editorEvent()
	int iconheight = option.rect.height()-4;
	QRect activerect(option.rect.left()+2, option.rect.top()+2, iconheight, iconheight);
	QRect visiblerect(option.rect.left()+2+iconheight+2, option.rect.top()+2, iconheight, iconheight);
	QRect colorrect(option.rect.left()+2+iconheight+2+iconheight+2, option.rect.top()+2, iconheight ,iconheight);
	QRect typerect(option.rect.left()+2+iconheight+2+iconheight+2+iconheight+2, option.rect.top()+2, iconheight, iconheight);
	QRect textrect(option.rect.left()+2+iconheight+2+iconheight+2+iconheight+2+iconheight+2, option.rect.top()+2, 800, iconheight);
	//

	painter->drawText(textrect, index.data(Qt::DisplayRole).toString(), QTextOption(Qt::AlignVCenter));
	if(const MassIndexModel* massindexmodel = dynamic_cast<const MassIndexModel*>(index.model())) {
		painter->drawImage(typerect, QImage(QString(":/%1").arg(massindexmodel->getType(index))));
		painter->drawImage(activerect, QImage(QString(":/%1").arg(massindexmodel->getActive(index)?"check":"uncheck")));
		painter->drawImage(visiblerect, QImage(QString(":/%1").arg(massindexmodel->getVisible(index)?"eye":"dimeye")));
		painter->fillRect(colorrect, massindexmodel->getColor(index));
	}
}
QSize MassIndexItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
	return QSize(w,h);
}

bool MassIndexItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {

	//code duplication with ::paint() function. If that code changes, change this too.
	int iconheight = option.rect.height()-4;
	QRect activerect(option.rect.left()+2, option.rect.top()+2, iconheight, iconheight);
	QRect visiblerect(option.rect.left()+2+iconheight+2, option.rect.top()+2, iconheight, iconheight);
	QRect colorrect(option.rect.left()+2+iconheight+2+iconheight+2, option.rect.top()+2, iconheight ,iconheight);
	QRect typerect(option.rect.left()+2+iconheight+2+iconheight+2+iconheight+2, option.rect.top()+2, iconheight, iconheight);
	QRect textrect(option.rect.left()+2+iconheight+2+iconheight+2+iconheight+2+iconheight+2, option.rect.top()+2, 100, iconheight);
	//

	if((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::MouseButtonDblClick)) {
		QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
		if (mouse_event->button() == Qt::LeftButton ) {
			if(activerect.contains(mouse_event->pos())) {
				if(const MassIndexModel* massindexmodel = dynamic_cast<const MassIndexModel*>(index.model())) {
					massindexmodel->setActive(index, !massindexmodel->getActive(index)); //toggle the massitem's active
				}
				return true;
			} else if(visiblerect.contains(mouse_event->pos())) {
				if(const MassIndexModel* massindexmodel = dynamic_cast<const MassIndexModel*>(index.model())) {
					massindexmodel->setVisible(index, !massindexmodel->getVisible(index)); //toggle the massitem's visible
				}
				return true;
			} else if(colorrect.contains(mouse_event->pos())) {
				if(const MassIndexModel* massindexmodel = dynamic_cast<const MassIndexModel*>(index.model())) {
					QColor newcol = QColorDialog::getColor(massindexmodel->getColor(index));
					massindexmodel->setColor(index, newcol.isValid()?newcol:massindexmodel->getColor(index)); //toggle the massitem's visible
				}
				return true;
			} else
				return false;
		}
		if (event->type() == QEvent::MouseButtonDblClick) {
			return true;
		}
	} else if (event->type() == QEvent::KeyPress) {
		if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select) {
			return false;
		}
	} else {
		return false;
	}

	return false;
}