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
#include "qgraphicskeyitem.h"

class KeyItem {
public:
	KeyItem(QString id) : keyId(id) {};
	QString keyId, labelTop, labelBottom, style[2];
};
	
class KeyItemModel : public QAbstractTableModel
{
Q_OBJECT
public:
    KeyItemModel();
	void setKeys(QMultiMap<QString,QGraphicsItem*> keys);
    ~KeyItemModel();
	
	static QMap< QString, QPair<QColor, QColor> > colors;
	QVariant getBrushV(QString name, int role) const;
	static QColor getColor(QString name, int role);
	
signals:
	void keyChanged(KeyItem*);
	
protected:
	
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual KeyItem* key(const QModelIndex& index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    void loadColors();
	void addColor(QString name, QColor fg, QColor bg);
	
	
	QList<KeyItem*> items;
	
};

#endif // KEYITEMMODEL_H
