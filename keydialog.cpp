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

#include "keydialog.h"
#include <QtGui/QKeyEvent>
#include <QtCore/QtGlobal>
#include <QAbstractButton>
#include <QMessageBox>
#include <QDebug>
#include "keyboardview.h"

KeyDialog::KeyDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	ui = new Ui_KeyDialog();
	ui->setupUi(this);
	ui->mainLabelLineEdit->installEventFilter(this);
	ui->secondLabelLineEdit->installEventFilter(this);
	setModal(true);
	resetCurrentItems();
	connect(ui->styleChooser, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
		this, SLOT(styleChanged(QListWidgetItem*)));
	foreach(QAbstractButton* button, ui->buttonBox->buttons())
		button->setFocusPolicy(Qt::NoFocus);
	styleDialog = new StyleDialog();
	connect(ui->addStyleButton, SIGNAL(pressed()), SLOT(addStyle()));
	connect(ui->editStyleButton, SIGNAL(pressed()), SLOT(editStyle()));
	connect(ui->deleteStyleButton, SIGNAL(pressed()), SLOT(deleteStyle()));
	connect((KeyboardView*) parent, SIGNAL(zoomChanged(double)), 
			styleDialog, SLOT(changeScale(double)));
}

void KeyDialog::styleChanged(QListWidgetItem* item)
{
	if (focusChanging)
		return;
	if (ui->secondLabelLineEdit->hasFocus())
		currentItems[1] = item;
	else
		currentItems[0] = item;
}

int KeyDialog::exec(KeyItem* key)
{
	if (key == nullptr) {
		qDebug() << "can't open dialog for unknown key!";
		return Rejected;
	}
	
	setWindowTitle(tr("Set labels for ") + key->keyId);
	if (key == nullptr || ui->styleChooser->count() == 0)
		return QDialog::Rejected;
	keyItem = key;
	ui->mainLabelLineEdit->setFocus();
	if (keyItem->labelTop != "") {
		ui->styleChooser->setCurrentText(key->style[0]);
		currentItems[0] = ui->styleChooser->findItem(key->style[0]);
	}
	if (keyItem->labelTop != "")
		currentItems[1] = ui->styleChooser->findItem(key->style[1]);
	
	ui->mainLabelLineEdit->setText(key->labelTop);
	ui->secondLabelLineEdit->setText(key->labelBottom);
	
	int result = QDialog::exec();
	if (result == Accepted)
	{
		keyItem->labelTop = ui->mainLabelLineEdit->text();
		keyItem->labelBottom = ui->secondLabelLineEdit->text();
		keyItem->setStyle(currentItems[0]->text(), 0);
		if (keyItem->labelBottom != "")
			keyItem->setStyle(currentItems[1]->text(), 1);
		keyItem->updateItems();
	}
	return result;
}

bool KeyDialog::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == ui->mainLabelLineEdit || obj == ui->secondLabelLineEdit)
	{
		if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) 
		{
			QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up)
				return QApplication::sendEvent(ui->styleChooser, event);
		}
		if (event->type() == QEvent::FocusOut)
		{
			focusChanging = true;
			if (obj == ui->mainLabelLineEdit)
				currentItems[0] = ui->styleChooser->currentItem();
			else
				currentItems[1] = ui->styleChooser->currentItem();
		}
		if (event->type() == QEvent::FocusIn)
		{
			checkCurrentItems();
			if (obj == ui->mainLabelLineEdit)
				ui->styleChooser->setCurrentItem(currentItems[0]);
			else
				ui->styleChooser->setCurrentItem(currentItems[1]);
			focusChanging = false;
		}
	}
	return QDialog::eventFilter(obj, event);
}

void KeyDialog::resetCurrentItems()
{
	currentItems[0] = currentItems[1] = ui->styleChooser->getDefault();
}

void KeyDialog::addStyle()
{
	int result = styleDialog->exec(StyleModel::model->styles[ui->styleChooser->currentItem()->text()], false);
	if (result == Accepted)
	{
		Style *style = new Style(styleDialog->style);
		StyleModel::model->addStyle(style);
	}
}

void KeyDialog::deleteStyle()
{
	QString name = ui->styleChooser->currentItem()->text();
	if (QMessageBox::question(this, "Really Delete?", "Really delete style " + name) 
		== QMessageBox::Yes) {
		// FIXME occasional crash when style is used on some key -> race conditions
		StyleModel::model->deleteStyle(name);
	}
}

void KeyDialog::editStyle()
{
	Style *edStyle = StyleModel::model->styles[ui->styleChooser->currentItem()->text()];
	int result = styleDialog->exec(edStyle, true);
	if (result == Accepted)
	{
		(*edStyle) = styleDialog->style;
		StyleModel::model->styleChangedOk(edStyle);
	}
}

void KeyDialog::checkCurrentItems()
{
	for (int i = 0; i < 2; i++) {
		if (!ui->styleChooser->isValidItem(currentItems[i]))
			currentItems[i] = ui->styleChooser->getDefault();
	}
}


