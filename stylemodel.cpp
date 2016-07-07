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

#include "stylemodel.h"

StyleModel *StyleModel::model = new StyleModel();

StyleModel::StyleModel()
{
	loadColors();
}

StyleModel::~StyleModel()
{

}

void StyleModel::loadColors()
{
	addStyle("default",   Qt::black, Qt::white);
	addStyle("black",     Qt::white, Qt::black);
	addStyle("lightgray", Qt::black, Qt::lightGray);
	addStyle("darkgray",  Qt::white, Qt::darkGray);
	addStyle("brownish",  Qt::black, Qt::darkYellow);
	addStyle("cyan",      Qt::black, Qt::cyan);
	addStyle("red",       Qt::white, Qt::darkRed);
	//addColor("", Qt::, Qt::);
	emit stylesChanged(All, nullptr);
}

void StyleModel::addStyle(QString name, QColor fg, QColor bg)
{
	while (styles.contains(name))
		name += "_";
	Style *newStyle = new Style(name, fg, bg);
	styles.insert(name, newStyle);
	stylesByPointer[newStyle] = name;
	emit stylesChanged(StyleModel::New, newStyle);
}

QVariant StyleModel::getBrushV(QString name, int role) const
{
	if (name == "")
		return getBrushV("default", role);
	QBrush brush;
	if (role == Qt::ForegroundRole)
		brush.setColor(styles[name]->fg.color());
	else
		brush = styles[name]->bg;
	return QVariant(brush);
}

QColor StyleModel::getColor(QString name, int role) 
{
	if (name == "")
		return getColor("default", role);
	if (role == Qt::ForegroundRole)
		return styles[name]->fg.color();
	else
		return styles[name]->bg.color();
}

void StyleModel::deleteStyle(Style* style)
{
	if (style->name == "default")
		return;
	if (styles[style->name] == style)
		delete styles.take(style->name);
	else {
		QString name = stylesByPointer[style];
		delete styles.take(name);
	}
	stylesByPointer.remove(style);
	emit stylesChanged(StyleModel::Delete, style);
}

bool StyleModel::styleChangedOk(Style* style)
{
	if (styles[style->name] == style) {
		emit stylesChanged(StyleModel::Edit, style);
		return true;
	}
	bool result = true;
	while (styles.contains(style->name)) {
		result = false;
		style->name += "_";
	}
	
	QString name = stylesByPointer.take(style);
	styles.remove(name);
	styles[name] = style;
	stylesByPointer[style] = name;
	emit stylesChanged(StyleModel::Name, style);
	return result;
}

