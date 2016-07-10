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

#include "style.h"

Style::Style()
{
	Style("default");
}

Style::Style(QString name, QColor fgCol, QColor bgCol)
{
	this->name = name;
	fg.setColor(fgCol);
	bg.setColor(bgCol);
	bg.setStyle(Qt::SolidPattern);
}


Style::Style(const Style& other)
{
	operator=(other);
}

bool Style::operator=(const Style& other)
{
	name = other.name;
	fg = other.fg;
	bg = other.bg;
	font = other.font;
}

void Style::setSize(qreal size)
{
	font.setPointSizeF(size);
}

void Style::paint(QPainter& painter)
{

}

void Style::setFont(QString f)
{

}

void Style::setFont(QFont f)
{

}

QString Style::toString()
{
	QString s = name + ": ";
	s += "fg= " + fg.color().name();
	s += " bg= " + bg.color().name();
	s += " font= " + font.rawName();
	return s;
}
