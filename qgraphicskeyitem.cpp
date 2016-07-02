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

#include "qgraphicskeyitem.h"
#include <QDebug>
#include <QPainter>
#include <QGraphicsScene>
#include <QTextDocument>
#include "stylechooser.h"



QGraphicsKeyItem::QGraphicsKeyItem(QGraphicsItem* parent) : QGraphicsPolygonItem(parent)
{
	commonInit();
}

void QGraphicsKeyItem::commonInit()
{
	upperPolygon.clear();
	lowerPolygon.clear();
	
	for(int i = 0; i < 2; i++)
		textItems[i] = nullptr;
	
	if (polygon().size() < 3)
		return;
	
	qreal middle = boundingRect().height() / 2.0 -0.5; // TODO - PenWidth/2 ???
	bool lastIsUp = polygon().first().y() < middle;
	foreach (QPointF p, polygon())
	{
		if (p.y() < middle)
		{
			if (lastIsUp)
				upperPolygon.append(p);
			else
			{
				upperPolygon.append(QPointF(p.x(), middle));
				lowerPolygon.append(QPointF(p.x(), middle));
				upperPolygon.append(p);
				lastIsUp = true;
			}
		}
		else
		{
			if (!lastIsUp)
				lowerPolygon.append(p);
			else
			{
				upperPolygon.append(QPointF(p.x(), middle));
				lowerPolygon.append(QPointF(p.x(), middle));
				lowerPolygon.append(p);
				lastIsUp = false;
			}
		}
	}
	
	
	// TODO: set real brushes
	upperBrush = QBrush(Qt::gray);
	lowerBrush = QBrush(Qt::darkGreen);
}


//QGraphicsKeyItem::~QGrapicsKeyItem()// : ~QGraphicsPolygonItem()
//{
//}

QGraphicsKeyItem::QGraphicsKeyItem(const QRectF& rect, QGraphicsItem* parent) 
{
	QGraphicsPolygonItem(QPolygonF(rect), parent);
	setPolygon(QPolygonF(rect));
	//qDebug() << "Rect:" << rect << ", this: " << boundingRect();
	commonInit();
}

QGraphicsKeyItem::QGraphicsKeyItem(const QPolygonF& polygon, QGraphicsItem* parent) 
	: QGraphicsPolygonItem(polygon, parent)
{
	commonInit();
}


void QGraphicsKeyItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	if (key == nullptr)
		return;
	painter->setPen(QPen(Qt::NoPen));
	
	upperBrush.setColor(KeyItemModel::getColor(key->style[0], Qt::BackgroundRole));
	lowerBrush.setColor(KeyItemModel::getColor(key->style[1], Qt::BackgroundRole));
	
	painter->setBrush(upperBrush);
	
	if (!key->labelBottom.isEmpty())
	{
		painter->drawPolygon(upperPolygon);
		painter->setBrush(lowerBrush);
		painter->drawPolygon(lowerPolygon);
		paintText(key->labelTop, upperPolygon);
		paintText(key->labelBottom, lowerPolygon, 1);
	}
	else
	{
		painter->drawPolygon(polygon());
		paintText(key->labelTop, polygon());
	}
	
	painter->setPen(QPen());
	
	QGraphicsPolygonItem::paint(painter, option, widget);
	
	//painter->setBrush(lowerBrush);
	//painter->drawPolygon(lowerPolygon);
	
}

void QGraphicsKeyItem::paintText(QString text, QPolygonF polygon, int index)
{
	const double TEXTMARGIN = 0.5;
	QGraphicsTextItem * item = textItems[index];
	if (item == nullptr) {
		item = (QGraphicsTextItem*)scene()->addText(text);
		item->setParentItem(this);
		textItems[index] = item;
		item->document()->setDocumentMargin(TEXTMARGIN);
		item->setTextWidth(boundingRect().width()-1.0);
		if (index == 1)
			item->setY(boundingRect().height()/2.0);
		//TODO trigger update of other texts size
	}
	else if (text == item->toPlainText())
	{
		return;
	}
	item->setPlainText(text);
	if (text == "")
		return;
	
	
	item->setDefaultTextColor(KeyItemModel::getColor(key->style[index], Qt::ForegroundRole));
	QFont font = item->font();
	qreal size = polygon.boundingRect().height()/2.2;
	font.setPointSizeF(size);
	item->setFont(font);
	while (size > 0.25 && !polygon.boundingRect().contains(
		item->mapRectToParent(item->boundingRect())))
	{
		//qDebug() << "bla: " << size << ", " << polygon.boundingRect() << " --- " << item->boundingRect();
		size -= 0.25;
		font.setPointSizeF(size);
		item->setFont(font);
	}
}

void QGraphicsKeyItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	qDebug() << "contextMenuEvent";
	StyleChooser *st = new StyleChooser();
	st->show();
}

void QGraphicsKeyItem::keyPressEvent(QKeyEvent* event)
{

}

void QGraphicsKeyItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	qDebug() << "mousePressEvent";
}
