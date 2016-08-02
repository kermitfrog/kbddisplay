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

bool QGraphicsKeyItem::globalSetupMade = false;
QPen QGraphicsKeyItem::middlePen;

QGraphicsKeyItem::QGraphicsKeyItem(QGraphicsItem* parent) : QGraphicsPolygonItem(parent)
{
	commonInit();
}

void QGraphicsKeyItem::commonInit()
{
	if (!globalSetupMade) {
		middlePen.setWidthF(SIZEFACTOR / 5.0);
		globalSetupMade = true;
	}
	upperPolygon.clear();
	lowerPolygon.clear();
	
	for(int i = 0; i < 2; i++)
		textItems[i] = nullptr;
	
	if (polygon().size() < 3)
		return;
	
	qreal middle = boundingRect().height() / 2.0 - penWidth/2.0; // TODO - PenWidth/2 ???
	bool lastIsUp = polygon().first().y() < middle;
	double middleX[2];
	int middleXId = 0;
	foreach (QPointF p, polygon())
	{
		if (p.y() < middle)
		{
			if (lastIsUp)
				upperPolygon.append(p);
			else
			{
				if (middleXId < 2)
					middleX[middleXId++] = p.x();
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
				if (middleXId < 2)
					middleX[middleXId++] = p.x();
				upperPolygon.append(QPointF(p.x(), middle));
				lowerPolygon.append(QPointF(p.x(), middle));
				lowerPolygon.append(p);
				lastIsUp = false;
			}
		}
	}
	
	middleLine.setLine(middleX[0], middle, middleX[1], middle);
	
	QPen pen;
	pen.setCosmetic(true);
	pen.setWidthF(SIZEFACTOR / 2.0);
	setPen(pen);
}

void QGraphicsKeyItem::setKey(KeyItem* key)
{
 this->key = key; 
 key->graphicItems.append(this);
 updateContent();
}

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

QGraphicsKeyItem::QGraphicsKeyItem(qreal diameter, QGraphicsItem* parent): QGraphicsPolygonItem(parent)
{
	QPolygonF mockCircle;
	QLineF helper;
	helper.setP1(QPointF(diameter/2, diameter/2));
	helper.setP2(QPointF(diameter, diameter/2));
// 	mockCircle << helper.p2();
	for (int i = 1; i < 36; i++) {
		helper.setAngle(10.0 * i);
		mockCircle << helper.p2();
	}
	helper.setAngle(0.0);
	mockCircle << helper.p2();
	setPolygon(mockCircle);
// 	qDebug() << "mockCircle: " << mockCircle;
	commonInit();
}

void QGraphicsKeyItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	if (key == nullptr) 
		return;
	painter->setPen(QPen(Qt::NoPen));
	
	
	painter->setBrush(upperBrush);
	
	if (!key->labelBottom.isEmpty())
	{
		painter->drawPolygon(upperPolygon);
		painter->setBrush(lowerBrush);
		painter->drawPolygon(lowerPolygon);
	}
	else
	{
		painter->drawPolygon(polygon());
	}
	
	if (!key->labelTop.isEmpty() && !key->labelBottom.isEmpty()) {
		painter->setPen(middlePen);
		painter->drawLine(middleLine);
	}
	
	
	QGraphicsPolygonItem::paint(painter, option, widget);
}

void QGraphicsKeyItem::paintText(QString text, QPolygonF polygon, int index)
{
	
	if (key->style[index] == "")
		return;
	
	const double TEXTMARGIN = 0.5 * SIZEFACTOR;
	QGraphicsTextItem * item = textItems[index];
	if (item == nullptr) {
		item = (QGraphicsTextItem*)scene()->addText(text);
		item->setParentItem(this);
		textItems[index] = item;
		item->document()->setDocumentMargin(TEXTMARGIN);
		item->setTextWidth(boundingRect().width()-SIZEFACTOR/2.0);
		item->document()->setDefaultTextOption(
			QTextOption(Qt::AlignCenter|Qt::AlignBottom));
		if (index == 1)
			item->setY(boundingRect().height()/2.0);
	}
	item->setPlainText(text);
	if (text == "")
		return;
	
	
	item->setDefaultTextColor(StyleModel::model->getColor(key->style[index], Qt::ForegroundRole));
	QFont font = StyleModel::model->getFont(key->style[index]);
	qreal size = font.pointSizeF();
	item->setFont(font);
	while (size > 5.0 && !polygon.boundingRect().contains(
		item->mapRectToParent(item->boundingRect())))
	{
		size -= 0.25;
		font.setPointSizeF(size);
		item->setFont(font);
	}
	qDebug() << "bla: " << size << ", " << polygon.boundingRect() << " --- " << item->boundingRect();
	//item->moveBy(0.0, 7.0);
	//item->moveBy(0.0, (polygon.boundingRect().height() - item->boundingRect().height()) / 2.0);
}

void QGraphicsKeyItem::updateContent()
{
	
	upperBrush.setColor(StyleModel::model->getColor(key->style[0], Qt::BackgroundRole));
	upperBrush.setStyle(Qt::SolidPattern);
	lowerBrush.setColor(StyleModel::model->getColor(key->style[1], Qt::BackgroundRole));
	lowerBrush.setStyle(Qt::SolidPattern);
	
	if (!key->labelBottom.isEmpty())
	{
		paintText(key->labelTop, upperPolygon);
		paintText(key->labelBottom, lowerPolygon, 1);
	}
	else
	{
		if (textItems[1] != nullptr && !textItems[1]->toPlainText().isEmpty())
			textItems[1]->setPlainText("");
		paintText(key->labelTop, polygon());
	}
	update();
	
}

int QGraphicsKeyItem::getPartIndex(QPointF pos)
{
	if (key->labelBottom.isEmpty()
		|| mapFromScene(pos).y() <= upperPolygon.boundingRect().height())
		return 0;
	return 1;
}

QRectF QGraphicsKeyItem::placementRect()
{
	//qDebug() << boundingRect() << " vs. " << boundingRect().marginsRemoved(margins);
	
	return boundingRect().marginsRemoved(margins);
}





