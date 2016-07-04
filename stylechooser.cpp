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
	
	// if Selected...
	if (option.state & QStyle::State_Selected)
		margin = 1;
	
	painter->setBrush(brush);
	
	// Background
	painter->drawRect(margin,margin + index.row()*HEIGHT,
					  ((StyleChooser*)parent())->width() - 2*margin,HEIGHT - margin*2);
	// Text
	pen.setStyle(Qt::SolidLine);
	painter->drawText(10,index.row()*HEIGHT + 18, index.data().toString());
	
	painter->restore();
}

QSize KeyStyleDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize(((StyleChooser*)parent())->width(), HEIGHT);
}

StyleChooser::StyleChooser(QWidget* parent) : QListWidget(parent)
{
	if (parent == nullptr) {
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
		setWindowTitle("stylechooser");
	}
	setItemDelegate(new KeyStyleDelegate(this));
	connect(this, SIGNAL(itemClicked(QListWidgetItem*)),
		this, SLOT(hide()));
}

void StyleChooser::updateStyles()
{
	clear();
	foreach (QString key, KeyItemModel::colors.keys()) {
		QListWidgetItem *item = new QListWidgetItem(key, this);
		QPair<QColor, QColor> itemColors = KeyItemModel::colors[key];
		QBrush bg;
		QPen fg;
		fg.setColor(itemColors.first);
		//fg.setStyle(Qt::SolidPattern);
		bg.setColor(itemColors.second);
		bg.setStyle(Qt::SolidPattern);
		item->setData(Qt::ForegroundRole, fg);
		item->setData(Qt::BackgroundRole, bg);
	}
}

void KeyStyleDelegate::addStyle(QString name, QPair< QColor, QColor > colors)
{

}


