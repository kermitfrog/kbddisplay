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

#include "keyitemmodel.h"
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

QMap< QString, QPair<QColor, QColor> > KeyItemModel::colors;

KeyItemModel::KeyItemModel()
{
	loadColors();
	//loadCodeToIdMap();
}

KeyItemModel::~KeyItemModel()
{

}

void KeyItemModel::setKeys(QMultiMap<QString,QGraphicsItem*> keymap)
{
	QMap<QString, int> ctiMap = loadCodeToIdMap();
	// FIXME :: each item is assigned (num of mappings) times to each mapping
	foreach (QString s, keymap.keys()) {
		KeyItem *item = new KeyItem(s);
		items.append(item);
		foreach (QGraphicsItem *gi, keymap.values(s))
		{
			if (gi->type() == QGraphicsKeyItem::Type)
				((QGraphicsKeyItem*) gi)->setKey(item);
			//qDebug() << "assigning " << s << " to Key at" << gi->sceneBoundingRect();
		}
		codeToKeyItemMap[ctiMap[item->keyId]] = item;
	}
}

int KeyItemModel::columnCount(const QModelIndex& parent) const
{
	return 3;
}

int KeyItemModel::rowCount(const QModelIndex& parent) const
{
	return items.count();
}

QVariant KeyItemModel::data(const QModelIndex& index, int role) const
{
	// TODO Qt::BackgroundRole, Qt::ForegroundRole
	KeyItem * item = items.at(index.row());
	
	switch (role) {
		case Qt::DisplayRole:
			switch (index.column()) {
				case 0:
					return item->keyId;
				case 1:
					return item->labelTop;
				case 2:
					return item->labelBottom;
				default:
					return QVariant();
			}
		case Qt::BackgroundRole:
		case Qt::ForegroundRole:
			switch (index.column()) {
				case 1:
					return getBrushV(item->style[0], role);
				case 2:
					return getBrushV(item->style[1], role);
				default:
					return QVariant();
			}
			
		default:
			return QVariant();
	}
}

bool KeyItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.row() < 0 || index.row() > items.count())
		return false;
	
	if (index.column() == 1)
		items[index.row()]->labelTop = value.toString();
	else if (index.column() == 2)
		items[index.row()]->labelBottom = value.toString();
	else 
		return false;
	
	QVector<int> roles;
	roles.append(role);
	emit dataChanged(index, index, roles);
	emit keyChanged(items[index.row()]);
	return true;
}

QVariant KeyItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// TODO return headerData
    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags KeyItemModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags flags = QAbstractTableModel::flags(index);
	if (index.column() > 0)
		flags |= Qt::ItemIsEditable | Qt::ItemIsSelectable;
	return flags;
}

void KeyItemModel::loadColors()
{
	addColor("default",   Qt::black, Qt::white);
	addColor("black",     Qt::white, Qt::black);
	addColor("lightgray", Qt::black, Qt::lightGray);
	addColor("darkgray",  Qt::white, Qt::darkGray);
	addColor("brownish",  Qt::black, Qt::darkYellow);
	addColor("cyan",      Qt::black, Qt::cyan);
	addColor("red",       Qt::white, Qt::darkRed);
	//addColor("", Qt::, Qt::);
	emit stylesChanged();
}

void KeyItemModel::addColor(QString name, QColor fg, QColor bg)
{
	colors.insert(name, QPair<QColor, QColor>(fg, bg));
}

QVariant KeyItemModel::getBrushV(QString name, int role) const
{
	if (name == "")
		return getBrushV("default", role);
	QBrush brush;
	if (role == Qt::ForegroundRole)
		brush.setColor(colors[name].first);
	else
		brush.setColor(colors[name].second);
	return QVariant(brush);
}

QColor KeyItemModel::getColor(QString name, int role) 
{
	if (name == "")
		return getColor("default", role);
	if (role == Qt::ForegroundRole)
		return colors[name].first;
	else
		return colors[name].second;
}

KeyItem* KeyItemModel::key(const QModelIndex& index) const
{
	return nullptr;//TODO implement??
}

QMap<QString, int> KeyItemModel::loadCodeToIdMap()
{
	QMap<QString, int> codeToIdMap;
	
	QFile f("/home/arek/projects/kbddisplay/keymapping.xml");
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "could not open mapping File for reading";
		return codeToIdMap;
	}
    QXmlStreamReader reader(&f);
	QXmlStreamAttributes attr;
	while (!reader.atEnd()) {
		reader.readNext();
		
		if ( (reader.isEndElement() && reader.name() != "key" )
			|| reader.isEndDocument())
			return codeToIdMap;
		if (!reader.isStartElement() || reader.name() != "key")
			continue;
		
		
		attr = reader.attributes();
		if (attr.hasAttribute("lxcode") && attr.hasAttribute("xevdevid"))
			codeToIdMap[attr.value("xevdevid").toString()] = attr.value("lxcode").toInt();
	}
	return codeToIdMap;
	
}

void KeyItem::setStyle(QString style, int index)
{
	this->style[index] = style;
	updateItems();
}

void KeyItem::updateItems()
{
	foreach(QGraphicsKeyItem* it, graphicItems)
		it->updateContent();
}











