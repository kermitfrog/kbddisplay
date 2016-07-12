/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Arek <arek@ag.de1.cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "stylechooser.h"
#include <QPainter>
#include <QDebug>

KeyStyleDelegate::KeyStyleDelegate(QObject* parent): QItemDelegate(parent)
{
}

void KeyStyleDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int margin = 5;
    painter->save();
    QPen pen = index.data(Qt::ForegroundRole).value<QPen>();
    pen.setWidthF(1.0);
    painter->setPen(pen);
    QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();
	QFont font = index.data(Qt::FontRole).value<QFont>();
	font.setPointSizeF(9.0);

    // if Selected...
    if (option.state & QStyle::State_Selected)
	    margin = 1;

    painter->setBrush(brush);

    // Background
    painter->drawRect(option.rect.x() + margin,
					  option.rect.y() + margin,
                      option.rect.width() - 2*margin,
					  option.rect.height() - 2*margin);
    // Text
    pen.setStyle(Qt::SolidLine);
	painter->setFont(font);
    painter->drawText(option.rect.x() + 10,
					  option.rect.y() + 18,
					  index.data().toString());
	
	/*QString rtl = "\u200F١٤:٥٠:١٣";
	qDebug() << rtl.isRightToLeft();
    painter->drawText(option.rect.x() + 10,
					  option.rect.y() + 18,
					  rtl);*/
	

    painter->restore();
}

QSize KeyStyleDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //return QSize(((StyleChooser*)parent())->width(), HEIGHT);
    return QSize(((StyleChooser*)parent())->drawRect().width(), HEIGHT);
}

StyleChooser::StyleChooser(QWidget* parent) : QListWidget(parent)
{
    if (parent == nullptr) {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        setWindowTitle("stylechooser");
    }
    setItemDelegate(new KeyStyleDelegate(this));
    //defaultItem = addStyle(new Style());
	
	connect(StyleModel::model, SIGNAL(stylesChanged(StyleModel::ChangeType,Style*)), 
			SLOT(updateStyles(StyleModel::ChangeType,Style*)));
	updateStyles(StyleModel::All, nullptr);
}

void StyleChooser::updateStyles(StyleModel::ChangeType type, Style* style)
{
	QListWidgetItem * item;
	switch (type) {
		case StyleModel::All:
			clear();
			foreach (QString key, StyleModel::model->styles.keys()) {
				item = addStyle(StyleModel::model->styles[key]);
				if (key == "default")
					defaultItem = item;
			}
			break;
		case StyleModel::Delete:
			item = itemMap.take(style);
			if (item != nullptr) {
				removeItemWidget(item);
				delete item;
			}
			break;
		case StyleModel::Edit:
		case StyleModel::Name:
			item = itemMap[style];
			if (item != nullptr) {
				item->setText(style->name);
				item->setData(Qt::ForegroundRole, style->fg);
				item->setData(Qt::BackgroundRole, style->bg);
				item->setData(Qt::FontRole, style->font);
			} 
			else 
				qDebug() << "nullptr in updateStyles, style is " << style;
			break;
		case StyleModel::New:
			addStyle(style);
			break;
	}
}

QListWidgetItem* StyleChooser::addStyle(Style * style)
{
    QListWidgetItem *item = new QListWidgetItem(style->name, this);
    item->setData(Qt::ForegroundRole, style->fg);
    item->setData(Qt::BackgroundRole, style->bg);
	itemMap[style] = item;
    return item;
}

QListWidgetItem* StyleChooser::findItem(QString name)
{
    QList<QListWidgetItem*> list = findItems(name, Qt::MatchFixedString);
    if (list.isEmpty())
        return getDefault();
    return list[0];
}

void StyleChooser::setCurrentText(QString text)
{
    setCurrentItem(findItem(text));
}

bool StyleChooser::isValidItem(QListWidgetItem* item)
{
	return itemMap.values().contains(item);
}


