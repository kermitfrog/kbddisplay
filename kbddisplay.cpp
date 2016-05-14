#include "kbddisplay.h"

#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QXmlStreamReader>

/*!
 * @brief Constructor of the main window. 
 * Here starts the interesting part of the application.
 * See main.ui for ui-> stuff.
 */
KbdDisplay::KbdDisplay()
{
	// load settings
	QCoreApplication::setApplicationName("KbdDisplay");
	QCoreApplication::setOrganizationName("de.aguzinski");
	//settings.load();

	// ui stuff
	
	// main
	ui = new Ui_KbdDisplay();
	ui->setupUi(this);
	setWindowTitle("KbdDisplay");
	connect(ui->actionQuit , SIGNAL(triggered()), SLOT(close()) );
	
	scene = new QGraphicsScene();
	ui->graphicsView->setScene(scene);
	scene->setBackgroundBrush(QBrush(QColor(Qt::white)));
	ui->graphicsView->scale(4.0, 4.0);
	
    //paintStuff();
    loadKbd("/home/arek/projects/kbddisplay/freestyle2.xml");
	
	// Mainwindows size. For some reason has to be at the end of the constructor.
	//setGeometry(settings.value("MainWindowGeometry", QRect(800,0,1200,600)).toRect());
}

/*!
 * @brief Deconstructor... *should* save Position and size of the main window.
 */
KbdDisplay::~KbdDisplay()
{
	//settings.setValue("MainWindowGeometry", geometry());
}

void KbdDisplay::paintStuff()
{
	qDebug() << "Scene = " << scene;
	QGraphicsRectItem * lastRect = scene->addRect(0.0, 0.0, 40.0, 40.0);
	QGraphicsRectItem * newRect;
	for (int i = 0; i < 7; i++)
	{
		newRect = scene->addRect(0.0, 0.0, 40.0, 40.0);
		newRect->setPos(lastRect->pos().x() + 45.0, lastRect->pos().y() + 4.0);
		lastRect = newRect;
		qDebug() << "Rect->x: " << lastRect->x() << "  Pointer: " << lastRect;
	}
	
	QGraphicsItemGroup *row = new QGraphicsItemGroup();
	scene->addItem(row);
	
	lastRect = scene->addRect(0.0, 0.0, 40.0, 40.0);
	row->addToGroup(lastRect);
	for (int i = 0; i < 8; i++)
	{
		newRect = scene->addRect(0.0, 0.0, 40.0, 40.0);
		newRect->setPos(lastRect->pos().x() + 45.0, lastRect->pos().y() + 4.0);
		row->addToGroup(newRect);
		lastRect = newRect;
		qDebug() << "Rect->x: " << lastRect->x() << "  Pointer: " << lastRect;
	}
	row->setRotation(20.0);
	
}

void KbdDisplay::loadKbd(QString filename)
{
	
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
		qDebug() << "could not open configuration File for reading: " << filename;
	
    QXmlStreamReader reader(&f);
	
	drawGroup(reader, nullptr, 0.0, 0.0);
	printItemTree();

}

QGraphicsItem* KbdDisplay::drawGroup(QXmlStreamReader &reader, QGraphicsItemGroup* parent, 
									 double keywidth, double keyheight, bool row)
{
	QString id;
	QXmlStreamAttributes attr;
	QGraphicsItem * item, * lastItem = nullptr, * resultItem;
	double_t groupX = 0.0, groupY = 0.0;
	
	while (!reader.atEnd()) {
		reader.readNext();
		
		if ( (reader.isEndElement() && reader.name() != "key" )
			|| reader.isEndDocument())
			return resultItem;
		
		if (!reader.isStartElement())
			continue;
		
		double_t x = 0.0, y = 0.0;
		attr = reader.attributes();
		if (attr.hasAttribute("keywidth"))
			keywidth = attr.value("keywidth").toDouble();
		if (attr.hasAttribute("keyheight"))
			keyheight = attr.value("keyheight").toDouble();
		if (attr.hasAttribute("x"))
			x = attr.value("x").toDouble();
		if (attr.hasAttribute("y"))
			y = attr.value("y").toDouble();
		if (attr.hasAttribute("id"))
			id = attr.value("id").toString();
		
		
		if (reader.name() == "keyboard")
		{
			id = attr.value("name").toString();
			continue;
		}
		else if (reader.name() == "group" || reader.name() == "row")
		{
			if (reader.isEndElement())
				return resultItem;
			QGraphicsItemGroup * group = new QGraphicsItemGroup();
			scene->addItem(group);
			//if (parent != nullptr)
			//	parent->addToGroup(group);
			item = group;  //TODO clean up unneccessary variables
			drawGroup (reader, group, keywidth, keyheight, reader.name() == "row");
			resultItem = item;
			groups[id] = (QGraphicsItemGroup*) item;
			groupX = x;
			groupY = y;
		}
		else if (reader.name() == "key")
		{
			//TODO nicer looking keys
			double_t w = keywidth, h = keyheight;
			if (attr.hasAttribute("w"))
				w = attr.value("w").toDouble();
			if (attr.hasAttribute("h"))
				h = attr.value("h").toDouble();
			item = scene->addRect(0.0, 0.0, w, h);
			keys.insert(id, item);
			addLabel(id, item);
		}
		else 
			continue;
		
		if (parent != nullptr)
			parent->addToGroup(item);
		
		if (attr.hasAttribute("relativeTo"))
			lastItem = getItem(attr.value("relativeTo").toString());
		
		QPointF p(0.0, 0.0);
		
		if (lastItem != nullptr)
		{
			QString corner;
			if (!attr.hasAttribute("corner"))
				corner = "ne";
			else
				corner = attr.value("corner").toString();
			
			if (corner == "ne")
				p = lastItem->mapRectToParent(lastItem->boundingRect()).topRight();
			else if (corner == "sw")
				p = lastItem->mapRectToParent(lastItem->boundingRect()).bottomLeft();
			else if (corner == "se")
				p = lastItem->mapRectToParent(lastItem->boundingRect()).bottomRight();
			else if (corner == "nw")
				p = lastItem->mapRectToParent(lastItem->boundingRect()).topLeft();
			else
				qDebug() << "Warning: invalid corner value \"" << corner << "\""; 
		}
		
		
		item->moveBy(groupX + x + p.x() + 0.5, groupY + y + p.y() + 0.5);
		lastItem = item;
		qDebug() << "boundingRect of " << id << " is " << item->boundingRect()
				 << ", to Parent: " << lastItem->mapRectToParent(lastItem->boundingRect());
		
	}

}

QGraphicsItem* KbdDisplay::getItem(QString value)
{
	QGraphicsItem* item;
	QMap<QString, QGraphicsItem*>::const_iterator it = keys.find(value);
	if (it != keys.end())
		return it.value();
	return groups[value];
}

void KbdDisplay::addLabel(QString id, QGraphicsItem* item)
{
	QFont font;
	font.setPointSizeF(2.0);
	QGraphicsTextItem * label = scene->addText(id, font);
	label->setDefaultTextColor(QColor(Qt::black));
	label->setTextWidth(item->boundingRect().width());
	label->setParentItem(item);
	
}

void KbdDisplay::printItemTree(QGraphicsItem* root, int level)
{
	if (root == nullptr)
	{
		printItemTree(groups["left"]);
		printItemTree(groups["right"]);
		return;
	}
	
	QGraphicsTextItem *text = qgraphicsitem_cast<QGraphicsTextItem*>(root);
	QString output1, output2; 
	output1.fill(' ', level * 2);
	if (text != 0)
		output2 += ", id: " + text->toPlainText();
	qDebug() << output1 << "boundingRect: " << root->boundingRect()
				   << ", to Scene: " << root->sceneBoundingRect()
				   << output2;
	QGraphicsItemGroup *group = qgraphicsitem_cast<QGraphicsItemGroup*>(root);
	if (group != 0)
		qDebug() << "GroupBR: " << group->childrenBoundingRect();
	

	QList<QGraphicsItem*> children = root->childItems();
	foreach(QGraphicsItem* item, children)
		printItemTree(item, level + 1);
	
	
	
	
	
	
}



void KbdDisplay::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}



