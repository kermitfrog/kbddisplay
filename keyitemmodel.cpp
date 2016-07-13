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
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <QGuiApplication>

KeyItemModel::KeyItemModel()
{
	//loadCodeToIdMap();
	connect (StyleModel::model, SIGNAL(stylesChanged(StyleModel::ChangeType,Style*)),
			 SLOT(updateAllItems()));
}

KeyItemModel::~KeyItemModel()
{

}

void KeyItemModel::setKeys(QMultiMap<QString,QGraphicsItem*> keymap)
{
	// clean up
	foreach (KeyItem * key, items) {
		// FIXME i think, there is a memory leak --> investigate
		//foreach (QGraphicsKeyItem * item, key->graphicItems)
		//	qDebug() << item->getKey();
		key->graphicItems.clear();
	}
	
	QMap<QString, int> ctiMap = loadCodeToIdMap();
	codeToKeyItemMap.clear();
	foreach (QString s, keymap.uniqueKeys()) {
		KeyItem *item;
		if (sortedItems.contains(s))
			item = sortedItems[s];
		else {
			item = new KeyItem(s);
			sortedItems[s] = item;
			items.append(item);
		}
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
					return StyleModel::model->getBrushV(item->style[0], role);
				case 2:
					return StyleModel::model->getBrushV(item->style[1], role);
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


KeyItem* KeyItemModel::key(const QModelIndex& index) const
{
	return nullptr;//TODO implement??
}

QMap< QString, int > KeyItemModel::loadCodeToIdMap()
{
	QMap<QString, int> codeToIdMap;
	
	QFile f(QDir::currentPath() + "/keymapping.csv");
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "could not open mapping File for reading";
		return codeToIdMap;
	}
	int idIndex, codeIndex = -1;
	QTextStream csvStream(&f);
	QStringList csvLine;
	csvLine = csvStream.readLine().split("\t");
	idIndex = csvLine.indexOf("xkb");
	codeIndex = csvLine.indexOf(QGuiApplication::platformName());
	qDebug() << idIndex << " " << codeIndex;
	
	while (!csvStream.atEnd()) {
		csvLine = csvStream.readLine().split("\t");
		if (csvLine.length() < qMax<int>(idIndex, codeIndex))
			continue;
		codeToIdMap[csvLine[idIndex]] = csvLine[codeIndex].toInt();
	}
	qDebug() << codeToIdMap.count();
	return codeToIdMap;
}

QMap<QString, int> KeyItemModel::loadCodeToIdMapXML()
{
	QMap<QString, int> codeToIdMap;
	
	QFile f(QDir::currentPath() + "/keymapping.xml");
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
	{
		if (it->getKey() == this)
			it->updateContent();
		else
			qDebug() << "Bah! This should not happen! ( KeyItem::updateItems() )";
	}
}

void KeyItemModel::save(QString filename)
{
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "could not open " << filename << "for writing";
		return;
	}
    QXmlStreamWriter writer(&f);
	QXmlStreamAttributes attr;

	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	writer.writeStartElement("kbdDisplayLayout");
	StyleModel::model->saveStyles(writer);
	
	writer.writeStartElement("keylayout");
	foreach(KeyItem* key, items) {
		writer.writeStartElement("key");
		writer.writeAttribute("id", key->keyId);
		writer.writeAttribute("mainlabel", key->labelTop);
		writer.writeAttribute("secondlabel", key->labelBottom);
		writer.writeAttribute("mainstyle", key->style[0]);
		writer.writeAttribute("secondstyle", key->style[1]);
		writer.writeEndElement();
	}
	writer.writeEndElement(); // keylayout
	writer.writeEndElement(); // kbdDisplayLayout
	writer.writeEndDocument();
	f.close();
}

bool KeyItemModel::load(QString filename)
{
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "could not open " << filename << "for reading";
		return false;
	}
    QXmlStreamReader reader(&f);
	QXmlStreamAttributes attr;
	
	while (!reader.atEnd()) {
		reader.readNext();
		if (reader.isStartElement() && reader.name() == "kbdDisplayLayout")
			continue;
		
		if (reader.isStartElement() && reader.name() == "keylayout")
			while (!reader.atEnd()) {
				reader.readNext();
				if (reader.isEndElement() && reader.name() == "keylayout")
					break;
				
				if (!reader.isStartElement() || reader.name() != "key")
					continue;
				
				attr = reader.attributes();
				KeyItem *key = getKey(attr.value("id").toString());
				key->labelTop = attr.value("mainlabel").toString();
				key->labelBottom = attr.value("secondlabel").toString();
				key->style[0] = attr.value("mainstyle").toString();
				key->style[1] = attr.value("secondstyle").toString();
				key->updateItems();
			}
		else if (reader.isStartElement() && reader.name() == "styles")
			StyleModel::model->loadStyles(reader);
	}
	f.close();
	return true;
}

KeyItem* KeyItemModel::getKey(QString id)
{
	KeyItem *key = sortedItems.value(id);
	if (key == nullptr) {
		key = new KeyItem(id);
		sortedItems[id] = key;
		items.append(key);
	}
	return key;
}

void KeyItemModel::updateAllItems()
{
	qDebug() << "updateAllItems";
	foreach (KeyItem* key, items)
		key->updateItems();
}








