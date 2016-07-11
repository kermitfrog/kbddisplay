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

#ifndef QGRAPHICSKEYITEM_H
#define QGRAPHICSKEYITEM_H

#include <QGraphicsPolygonItem>
#include <QBrush>
#include "keyitemmodel.h"

class KeyItemModel;
class KeyItem;

class QGraphicsKeyItem : public QGraphicsPolygonItem
{
public:
	enum { Type = UserType + 1 };
    QGraphicsKeyItem(QGraphicsItem* parent = Q_NULLPTR);
    //virtual ~QGraphicsKeyItem();
	
    QGraphicsKeyItem(const QPolygonF& polygon, QGraphicsItem* parent = Q_NULLPTR);
    QGraphicsKeyItem(const QRectF& polygon, QGraphicsItem* parent = Q_NULLPTR);
    QGraphicsKeyItem(qreal diameter, QGraphicsItem* parent = Q_NULLPTR);
	
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	void setKey(KeyItem * key);
	virtual int type() const {return Type; };
	int getPartIndex(QPointF pos);
	void setStyle(QString style, int index = 0);
	void updateContent();
	KeyItem *getKey() {return key;};
	
protected:
	void commonInit();
	void paintText(QString text, QPolygonF polygon, int index = 0);
	QPolygonF upperPolygon, lowerPolygon;
	QBrush upperBrush, lowerBrush;
	QGraphicsTextItem *textItems[2];
	
	KeyItem *key = nullptr;
	QModelIndex data;
	
};

#endif // QGRAPHICSKEYITEM_H
