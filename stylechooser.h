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

#pragma once
#include <QListWidget>
#include <QWidget>
#include <QItemDelegate>
#include "keyitemmodel.h"

class KeyStyleDelegate : public QItemDelegate
{
	const int HEIGHT = 30;
public:
    KeyStyleDelegate(QObject* parent = 0);
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) 
	const Q_DECL_OVERRIDE;
	
protected:
	virtual void paint(
		QPainter *painter, 
		const QStyleOptionViewItem &option, 
		const QModelIndex &index) const;
};

class StyleChooser : public QListWidget
{
	Q_OBJECT
public:
    explicit StyleChooser(QWidget* parent = 0);
	QListWidgetItem* getDefault() {return defaultItem;};
	QListWidgetItem* addStyle(QString name, QPair<QColor, QColor> colors);
	QListWidgetItem* findItem(QString name);
public slots:
	void updateStyles();
	void setCurrentText(QString text);
protected:
	QListWidgetItem* defaultItem;
};

