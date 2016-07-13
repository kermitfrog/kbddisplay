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

#ifndef KEYBOARDVIEW_H
#define KEYBOARDVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QPrinter>
#include "stylechooser.h"
#include "keyitemmodel.h"
#include "keydialog.h"

class KeyboardView : public QGraphicsView
{
	Q_OBJECT
public:
    KeyboardView(QWidget* parent = 0);
	void setModel(KeyItemModel* model);
	KeyDialog *keyDialog;
public slots:
	void setStyle(QListWidgetItem* item);
	void exportSVG(QString filename);
	void print(QPrinter &printer);
	void autoMap();
	void diffAutoMap();
	//void updateItem(KeyItem* key);
	
signals:
	void zoomChanged(double zoom);
	
protected:
	KeyItem * currentKey = nullptr;
	int currentIndex = 0;
    virtual void contextMenuEvent(QContextMenuEvent* event);
	StyleChooser *stylechooser;
	KeyItemModel *model = nullptr;
	virtual void keyPressEvent(QKeyEvent *event);
	bool autoMapping = false;
	virtual void resizeEvent(QResizeEvent *event);
	void doAutoMap(bool diff);
};

#endif // KEYBOARDVIEW_H
