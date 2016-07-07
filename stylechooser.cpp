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

#include "stylechooser.h"
#include <QPainter>
#include <QDebug>

KeyStyleDelegate::KeyStyleDelegate(QObject* parent): QItemDelegate(parent)
{
}

void KeyStyleDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int margin = 5;
    painter->save();
    QPen pen = index.data(Qt::ForegroundRole).value<QPen>();
    pen.setWidthF(1.0);
    painter->setPen(pen);
    QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();

    // if Selected...
    if (option.state & QStyle::State_Selected)
        margin = 1;

    painter->setBrush(brush);

    // Background
    painter->drawRect(margin,margin + index.row()*HEIGHT,
                      ((StyleChooser*)parent())->width() - 2*margin,HEIGHT - margin*2);
    // Text
    pen.setStyle(Qt::SolidLine);
    painter->drawText(10,index.row()*HEIGHT + 18, index.data().toString());

    painter->restore();
}

QSize KeyStyleDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(((StyleChooser*)parent())->width(), HEIGHT);
}

StyleChooser::StyleChooser(QWidget* parent) : QListWidget(parent)
{
    if (parent == nullptr) {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        setWindowTitle("stylechooser");
    }
    setItemDelegate(new KeyStyleDelegate(this));
    defaultItem = addStyle(new Style());
}

void StyleChooser::updateStyles()
{
    clear();
    QListWidgetItem * item;
    foreach (QString key, StyleModel::model->styles.keys()) {
        item = addStyle(StyleModel::model->styles[key]);
        if (key == "default")
            defaultItem = item;
    }
}

QListWidgetItem* StyleChooser::addStyle(Style * style)
{
    QListWidgetItem *item = new QListWidgetItem(style->name, this);
    item->setData(Qt::ForegroundRole, style->fg);
    item->setData(Qt::BackgroundRole, style->bg);
    return item;
}

QListWidgetItem* StyleChooser::findItem(QString name)
{
    QList<QListWidgetItem*> list = findItems(name, Qt::MatchFixedString);
    if (list.isEmpty())
        return getDefault();
    return list[0];
}

void StyleChooser::setCurrentText(QString text)
{
    setCurrentItem(findItem(text));
}


