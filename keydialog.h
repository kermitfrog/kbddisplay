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

#ifndef KEYDIALOG_H
#define KEYDIALOG_H

#include <qt5/QtWidgets/qdialog.h>
#include <ui_keydialog.h>
#include "keyitemmodel.h"
#include "styledialog.h"

class KeyDialog : public QDialog
{
	Q_OBJECT
public:
    KeyDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
	int exec(KeyItem* key);
	StyleChooser* getStyleChooser() {return ui->styleChooser;};
	
public slots:
	void styleChanged(QListWidgetItem* item);
	void resetCurrentItems();
	void addStyle();
	void editStyle();
	void deleteStyle();
	
protected:
    virtual bool eventFilter(QObject* obj, QEvent* event);
    void checkCurrentItems();
	Ui_KeyDialog* ui;
	KeyItem *keyItem;
	QListWidgetItem * currentItems[2];
	bool focusChanging = false;
	StyleDialog *styleDialog;
};

#endif // KEYDIALOG_H
