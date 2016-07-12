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

#ifndef KEYITEMMODEL_H
#define KEYITEMMODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QBrush>
#include <QList>
#include <QDebug>
#include "qgraphicskeyitem.h"
#include "style.h"
#include "stylemodel.h"
class QGraphicsKeyItem;

class KeyItem {
public:
	KeyItem(QString id) : keyId(id) {};
	QString keyId, labelTop, labelBottom, style[2];
	QList<QGraphicsKeyItem*> graphicItems;
	void setStyle(QString style, int index);
	void updateItems();
	
};
	
class KeyItemModel : public QAbstractTableModel
{
Q_OBJECT
public:
    KeyItemModel();
	void setKeys(QMultiMap<QString,QGraphicsItem*> keys);
    ~KeyItemModel();
	
	QMap< int, KeyItem* > codeToKeyItemMap;
	bool load(QString filename);
	void checkKey(KeyItem *key) {
		qDebug() << key;
		qDebug() << items.indexOf(key);
	};
	
signals:
	void keyChanged(KeyItem*);
	
public slots:
	void save(QString filename);
	void updateAllItems();
	
protected:
	
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual KeyItem* key(const QModelIndex& index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	QMap<QString, int> loadCodeToIdMap();
	
	QList<KeyItem*> items;
	QMap<QString, KeyItem*> sortedItems;
	
};

#endif // KEYITEMMODEL_H
