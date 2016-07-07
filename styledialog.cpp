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

#include "styledialog.h"
#include <QDebug>

StyleDialog::StyleDialog()
{
	ui = new Ui_StyleDialog();
	ui->setupUi(this);
	setModal(true);
	ui->preView->setScene(new QGraphicsScene(ui->preView));
	it = ui->preView->scene()->addText(tr("Preview"));
	fgDialog = new QColorDialog(this);
	fgDialog->setWindowTitle(tr("Text Color"));
	connect(fgDialog, SIGNAL(currentColorChanged(QColor)), SLOT(changeFG(QColor)));
	connect(ui->fontColorButton, SIGNAL(pressed()), fgDialog, SLOT(show()));
	
	bgDialog = new QColorDialog(this);
	bgDialog->setWindowTitle(tr("Background Color"));
	connect(bgDialog, SIGNAL(currentColorChanged(QColor)), SLOT(changeBG(QColor)));
	connect(ui->bgColorButton, SIGNAL(pressed()), bgDialog, SLOT(show()));
	
	connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), SLOT(changFont(QFont)));
	connect(ui->fontSizeSpinBox, SIGNAL(valueChanged(double)), SLOT(changeSize(double)));
}

StyleDialog::~StyleDialog()
{
	delete fgDialog;
	delete bgDialog;
	delete ui; // TODO find out if its auto deleted
}

int StyleDialog::exec(Style *styleToEdit, bool edit)
{
	style = (*styleToEdit);
	if (!edit)
	{
		style.name = "New Style";
		setWindowTitle(tr("Create new style "));
	}
	else
		setWindowTitle(tr("Set up style ") + style.name);
	ui->nameEdit->setText(style.name);
	fgDialog->setCurrentColor(style.fg.color());
	bgDialog->setCurrentColor(style.bg.color());
	ui->fontComboBox->setCurrentFont(style.font);
	ui->fontSizeSpinBox->setValue(style.font.pointSizeF());
	QDialog::exec();
}

void StyleDialog::changeBG(QColor color)
{
	style.bg.setColor(color);
	ui->preView->setBackgroundBrush(style.bg);
}

void StyleDialog::changeFG(QColor color)
{
	style.fg.setColor(color);
	it->setDefaultTextColor(color);
}

void StyleDialog::changFont(QFont font)
{
	font.setPointSizeF(ui->fontSizeSpinBox->value());
	style.font = font;
	it->setFont(font);
}

void StyleDialog::changeSize(double size)
{
	style.font.setPointSizeF(size);
	it->setFont(style.font);
}

void StyleDialog::changeName(QString& name)
{
	style.name = name;
}

