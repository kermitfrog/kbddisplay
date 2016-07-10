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

#ifndef STYLEMODEL_H
#define STYLEMODEL_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QVariant>
#include "style.h"

class StyleModel : public QObject
{
	Q_OBJECT
public:
	static StyleModel *model;
    StyleModel();
    ~StyleModel();
	QVariant getBrushV(QString name, int role) const;
	QColor getColor(QString name, int role);
	void addStyle(QString name, QColor fg, QColor bg);
	void addStyle(Style* style);
	bool styleChangedOk(Style * style);
	void deleteStyle(Style * style);
	void deleteStyle(QString style);
	
	QMap< QString, Style*> styles;
	QMap< Style*, QString> stylesByPointer;
	enum ChangeType{All, Edit, Name, New, Delete};
	
signals:
	void stylesChanged(StyleModel::ChangeType type, Style* style);
	
protected:
	void loadColors();
};

#endif // STYLEMODEL_H
