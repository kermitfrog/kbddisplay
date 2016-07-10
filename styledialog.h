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

#ifndef STYLEDIALOG_H
#define STYLEDIALOG_H

#include <QDialog>
#include <ui_styledialog.h>
#include <QGraphicsTextItem>
#include <QColorDialog>
#include "style.h"

class StyleDialog : public QDialog
{
	Q_OBJECT
public:
    StyleDialog();
    virtual ~StyleDialog();
	int exec(Style *styleToEdit, bool edit);
	Style style;
	
public slots:
	void changeFG(QColor color);
	void changeBG(QColor color);
	void changFont(QFont font);
	void changeSize(double size);
	void changeScale(double size);
	void changeName(QString name);
	
protected:
	Ui_StyleDialog *ui;
	QGraphicsTextItem * it;
	QColorDialog *fgDialog, *bgDialog;
	double scale = 1.0;
};

#endif // STYLEDIALOG_H
