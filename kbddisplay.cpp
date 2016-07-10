#include "kbddisplay.h"

#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QXmlStreamReader>
#include <QString>
#include <QFileDialog>

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
	StyleModel::init();

	// ui stuff
	
	// main
	ui = new Ui_KbdDisplay();
	ui->setupUi(this);
	setWindowTitle("KbdDisplay");
	
	// menu
	connect(ui->actionQuit , SIGNAL(triggered()), SLOT(close()) );
	connect(ui->action_Open, SIGNAL(triggered()), SLOT(open()));
	connect(ui->actionExport_to_SVG, SIGNAL(triggered()), SLOT(exportSVG()));
	connect(ui->actionSave, SIGNAL(triggered()), SLOT(save()));
	connect(ui->actionSave_As, SIGNAL(triggered()), SLOT(saveAs()));
	connect(ui->actionAutomap_current_Layout, SIGNAL(triggered()), ui->graphicsView, SLOT(autoMap()));
	connect(ui->actionLayout_diff, SIGNAL(triggered(bool)), ui->graphicsView, SLOT(diffAutoMap()));
	connect(ui->action_Save_as_Default, SIGNAL(triggered(bool)), 
		StyleModel::model, SLOT(saveDefaultStyles()));
	
	
	scene = new QGraphicsScene();
	ui->graphicsView->setScene(scene);
	scene->setBackgroundBrush(QBrush(QColor(Qt::white)));
	ui->graphicsView->scale(4.0, 4.0);
	
    //paintStuff();
    loadKbd(QDir::currentPath() +  "/freestyle2.xml");
	
	// set up table
	model = new KeyItemModel();
	model->setKeys(keys);
	ui->tableView->setModel(model);
	ui->graphicsView->setModel(model);
	connect(model, SIGNAL(keyChanged(KeyItem*)), this, SLOT(keyChanged(KeyItem*)));
	
	ui->graphicsView->setFocus();
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

void KbdDisplay::loadKbd(QString filename)
{
	
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "could not open configuration File for reading: " << filename;
		return;
	}
	keys.clear();
	groups.clear();
	scene->clear();
	
    QXmlStreamReader reader(&f);
	
	drawGroup(reader, nullptr, 0.0, 0.0);
	//printItemTree();
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
			if(attr.hasAttribute("polygon"))
			{
				QPolygonF polygon;
				QString points = attr.value("polygon").toString();
				foreach (QString point, points.split(' ', QString::SkipEmptyParts))
				{
					QStringList plist = point.split(',', QString::SkipEmptyParts);
					if (plist.size() != 2)
					{
						qDebug() << "Invalid point in " << points;
						continue;
					}
					polygon.append(QPointF(plist[0].toDouble(), plist[1].toDouble()));
				}
				item = new QGraphicsKeyItem(polygon);
			}
			else
			{
				double_t w = keywidth, h = keyheight;
				if (attr.hasAttribute("w"))
					w = attr.value("w").toDouble();
				if (attr.hasAttribute("h"))
					h = attr.value("h").toDouble();
				item = new QGraphicsKeyItem(QRect(0.0, 0.0, w, h));
			}
			scene->addItem(item);
			keys.insert(id, item);
			//addLabel(id, item);
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
			QRectF bRect = lastItem->mapRectToParent(lastItem->boundingRect() | lastItem->childrenBoundingRect());
			
			if (corner == "ne")
				p = bRect.topRight();
			else if (corner == "sw")
				p = bRect.bottomLeft();
			else if (corner == "se")
				p = bRect.bottomRight();
			else if (corner == "nw")
				p = bRect.topLeft();
			else
				qDebug() << "Warning: invalid corner value \"" << corner << "\""; 
		}
		
		
		item->moveBy(groupX + x + p.x() + 0.5, groupY + y + p.y() + 0.5);
		//item->moveBy(groupX + x + p.x() + 1, groupY + y + p.y() + 1);
		//item->moveBy(groupX + x + p.x() , groupY + y + p.y());
		lastItem = item;
		//qDebug() << "boundingRect of " << id << " is " << item->boundingRect()
		//		 << ", to Parent: " << lastItem->mapRectToParent(lastItem->boundingRect());
		
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

void KbdDisplay::keyChanged(KeyItem* item)
{
	foreach(QGraphicsItem* it, keys.values(item->keyId))
	{
		if (it->type() == QGraphicsKeyItem::Type)
			((QGraphicsKeyItem*)it)->updateContent();
		it->update();
		continue;
		/*QGraphicsTextItem *t = (QGraphicsTextItem*)it->childItems().at(0);
		t->setPlainText(item->labelTop);
		QGraphicsRectItem *r = (QGraphicsRectItem*)it;
		QBrush brush;
		brush.setColor(model->getColor(item->styleTop, Qt::BackgroundRole));
		r->setBrush(brush);
		*/
	}
}

void KbdDisplay::exportSVG()
{
	QString svgFile = QFileDialog::getSaveFileName(this, tr("Save As..."), 
		QDir::currentPath(), tr("SVG file (*.svg)")
	);
	ui->graphicsView->exportSVG(svgFile);
}

void KbdDisplay::open()
{
	QString fileToOpen = QFileDialog::getOpenFileName(this, tr("Open File"),
		QDir::currentPath(), tr("KeyLayoutXML (*.xml)")
	);
	if (model->load(fileToOpen))
		filename = fileToOpen;
}

void KbdDisplay::save()
{
	if (filename.isEmpty()) {
		saveAs();
		return;
	}
	model->save(filename);
}

void KbdDisplay::saveAs()
{
	filename = QFileDialog::getSaveFileName(this, tr("Save As..."), 
		QDir::currentPath(), tr("KeyLayoutXML (*.xml)")
	);
	model->save(filename);
}



