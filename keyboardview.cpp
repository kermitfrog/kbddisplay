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

#include "keyboardview.h"
#include "qgraphicskeyitem.h"
#include <QContextMenuEvent>
#include <QDebug>
#include <QSvgGenerator>
#include <QProcess>
#include <QMessageBox>

KeyboardView::KeyboardView(QWidget* parent) 
  : QGraphicsView(parent)
{
	keyDialog = new KeyDialog();
	stylechooser = keyDialog->getStyleChooser();
	connect(stylechooser, SIGNAL(itemChanged(QListWidgetItem*)),
			this, SLOT(setStyle(QListWidgetItem*)));
	connect(stylechooser, SIGNAL(itemClicked(QListWidgetItem*)),
			this, SLOT(setStyle(QListWidgetItem*)));
}

void KeyboardView::setStyle(QListWidgetItem* item)
{
	if (currentKey)
		currentKey->setStyle(item->data(Qt::DisplayRole).toString(), currentIndex);
}

void KeyboardView::setModel(KeyItemModel* model)
{
	if (this->model != nullptr)
		QObject::disconnect(this->model, 0, stylechooser, 0);
	this->model = model;
	connect(model, SIGNAL(stylesChanged()), stylechooser, SLOT(updateStyles()));
	stylechooser->updateStyles();
}

void KeyboardView::contextMenuEvent(QContextMenuEvent* event)
{
	QGraphicsKeyItem *keyitem = nullptr;
	foreach(QGraphicsItem *item, items(event->pos())) {
		if (item->type() != QGraphicsKeyItem::Type)
			continue;
		keyitem = (QGraphicsKeyItem*)item;
		break;
	}
	if (keyitem == nullptr)
		return;
	currentKey = keyitem->getKey();
	currentIndex = keyitem->getPartIndex(mapToScene(event->pos()));
	keyDialog->exec(currentKey);
	

}

void KeyboardView::keyPressEvent(QKeyEvent* event)
{
	/*qDebug("Key( %d ), nativeScanCode( %d ), nativeVirtualKey( %d )",
		event->key(), event->nativeScanCode(), event->nativeVirtualKey()
	);*/
	currentKey = model->codeToKeyItemMap[event->nativeScanCode()];
// 	qDebug() << currentKey->keyId << " : " << event->nativeScanCode();
	keyDialog->exec(currentKey);
	
    QGraphicsView::keyPressEvent(event);
}

void KeyboardView::autoMap()
{
	QList<int> codes = model->codeToKeyItemMap.keys();
	QString platform = QGuiApplication::platformName();
	if (platform == "xcb") {
		QProcess xmodmap;
		xmodmap.setProgram("xmodmap");
		QStringList args;
		args.append("-pk");
		xmodmap.setArguments(args);
		xmodmap.start();
		if (!xmodmap.waitForFinished()) {
			QMessageBox::warning(this, "Meh!", 
			"xmodmap failed or is not installed");
			return;
		}
		QString outString = QString(xmodmap.readAllStandardOutput());
		QStringList output = outString.split('\n');
		for(int i = 0; i < output.length(); i++) {
			bool ok;
			QStringList line = output[i].trimmed().split(
				QRegExp("\\s"), QString::SkipEmptyParts);
			if (line.length() < 3)
				continue;
			int code = line[0].toInt(&ok);
			if (!ok)
				continue;
			KeyItem * key = model->codeToKeyItemMap[code];
			if (key == nullptr)
				continue;
			int value = line[1].toInt(&ok, 16);
			QString name = line[2].mid(1, line[2].length()-2);
			
			if (name.length() > 1 && value < 255)
				name = QChar(value);
			key->labelTop=name;
			key->updateItems();
		}
	}
	else
		QMessageBox::information(this, "Meh!", 
			"sorry, this function currently only works on X11");
}


void KeyboardView::exportSVG(QString filename)
{
	QSvgGenerator generator;
	generator.setFileName(filename);
	
	QRect rect = scene()->sceneRect().toRect();
	const double_t scale = 3.0;
	rect.setRect(rect.x() * scale, rect.y() * scale,
				 rect.width() * scale, rect.height() * scale);
	
	
	generator.setSize(rect.size());
	generator.setViewBox(rect);
	
	QPainter painter( &generator );
	scene()->render( &painter );
}














