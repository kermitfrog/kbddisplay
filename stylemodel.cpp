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
#include <QDebug>
#include <QFile>
#include <QDir>

StyleModel *StyleModel::model;

StyleModel::StyleModel()
{
	loadColors();
}

StyleModel::~StyleModel()
{

}

void StyleModel::loadColors()
{
	QFile f(QDir::currentPath() + "/defaultstyles.xml");
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		surpressSignals = true;
		addStyle("default",   Qt::black, Qt::white);
		addStyle("black",     Qt::white, Qt::black);
		addStyle("lightgray", Qt::black, Qt::lightGray);
		addStyle("darkgray",  Qt::white, Qt::darkGray);
		addStyle("brownish",  Qt::black, Qt::darkYellow);
		addStyle("cyan",      Qt::black, Qt::cyan);
		addStyle("red",       Qt::white, Qt::darkRed);
		//addColor("", Qt::, Qt::);
		surpressSignals = false;
		return;
	}
    QXmlStreamReader reader(&f);
	QXmlStreamAttributes attr;
	
	while (!reader.atEnd()) {
		reader.readNext();
		if (reader.isStartElement() && reader.name() == "styles")
			loadStyles(reader);
	}
	f.close();
	surpressSignals = false;
	emit stylesChanged(All, nullptr);
}

void StyleModel::addStyle(QString name, QColor fg, QColor bg)
{
	while (styles.contains(name))
		name += "_";
	Style *newStyle = new Style(name, fg, bg);
	styles.insert(name, newStyle);
	stylesByPointer[newStyle] = name;
	if (!surpressSignals)
		emit stylesChanged(StyleModel::New, newStyle);
}

void StyleModel::addStyle(Style* style)
{
	while (styles.contains(style->name))
		style->name += "_";
	styles.insert(style->name, style);
	stylesByPointer[style] = style->name;
	if (!surpressSignals)
		emit stylesChanged(StyleModel::New, style);
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
	if (!surpressSignals)
		emit stylesChanged(StyleModel::Delete, style);
}

void StyleModel::deleteStyle(QString style)
{
	deleteStyle(styles[style]);
}


bool StyleModel::styleChangedOk(Style* style)
{
	qDebug() << style->toString();
	if (styles[style->name] == style) {
		if (!surpressSignals)
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
	if (!surpressSignals)
		emit stylesChanged(StyleModel::Name, style);
	return result;
}

QFont StyleModel::getFont(QString name) const
{
	if (styles.contains(name))
		return styles[name]->font;
	return QFont();
}

bool StyleModel::loadStyles(QXmlStreamReader& reader)
{
	surpressSignals = true;
	foreach (Style *style, stylesByPointer.keys())
		delete style;
	stylesByPointer.clear();
	styles.clear();
	
	QXmlStreamAttributes attr;
	while (!reader.atEnd()) {
		reader.readNext();
		
		if (reader.isEndElement() && reader.name() == "styles" )
			break;
		if ( (reader.isEndElement() && reader.name() != "style" )
			|| reader.isEndDocument()) {
			qDebug() << "not ok";
			surpressSignals = false;
			return false;
		}
		if (!reader.isStartElement() || reader.name() != "style")
			continue;
		
		attr = reader.attributes();
		Style *style = new Style( 
			attr.value("name").toString(),
			QColor(attr.value("fg").toString()),
			QColor(attr.value("bg").toString())
		);
		style->setFont(attr.value("font").toString());
		style->setSize(attr.value("maxFontSize").toDouble());
		addStyle(style);
	}
	surpressSignals = false;
	emit stylesChanged(All, nullptr);
}

void StyleModel::saveStyles(QXmlStreamWriter& writer)
{
	writer.writeStartElement("styles");
	foreach(Style *style, stylesByPointer.keys()) {
		writer.writeStartElement("style");
		writer.writeAttribute("name", style->name);
		writer.writeAttribute("fg", style->fg.color().name());
		writer.writeAttribute("bg", style->bg.color().name());
		writer.writeAttribute("font", style->font.family());
		writer.writeAttribute("maxFontSize", QString::number(style->font.pointSizeF()));
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

void StyleModel::saveDefaultStyles()
{
	QFile f(QDir::currentPath() + "/defaultstyles.xml");
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "could not open \"defaultstyles.xml\" for writing";
		return;
	}
    QXmlStreamWriter writer(&f);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	saveStyles(writer);
	writer.writeEndDocument();
	f.close();
}



