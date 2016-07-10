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

#ifndef STYLE_H
#define STYLE_H
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QPainter>

class Style
{
public:
    Style();
	Style(QString name, QColor fgCol = Qt::black, QColor bgCol = Qt::white);
    Style(const Style& other);
	void setFont(QFont f);
	void setFont(QString f);
	void setSize(qreal size);
    bool operator=(const Style& other);
	void paint(QPainter &painter); // area
	QString name;
	QPen fg;
	QBrush bg;
	QFont font;
	QString toString();
};

#endif // STYLE_H
