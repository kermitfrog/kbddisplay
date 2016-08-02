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
#include <QPrintDialog>
#include <QPrinter>

//const double KbdDisplay::SIZEFACTOR = 10.0;

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
	
	// toolbar
	setUpToolbar();
	
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
	connect(ui->actionPrint, SIGNAL(triggered(bool)), SLOT(print()));
	
	
	scene = new QGraphicsScene();
	ui->graphicsView->setScene(scene);
	scene->setBackgroundBrush(QBrush(QColor(Qt::white)));
	ui->graphicsView->scale(4.0, 4.0);
	
	// set up table
	model = new KeyItemModel();
	ui->tableView->setModel(model);
	ui->graphicsView->setModel(model);
	connect(model, SIGNAL(keyChanged(KeyItem*)), this, SLOT(keyChanged(KeyItem*)));
	
	ui->graphicsView->setFocus();
	loadKbd("default.xml");
	// Mainwindows size. For some reason has to be at the end of the constructor.
	//setGeometry(settings.value("MainWindowGeometry", QRect(800,0,1200,600)).toRect());
	//ui->tableView->resize(600, ui->tableView->height());
	ui->tableView->hide();
}

/*!
 * @brief Deconstructor... *should* save Position and size of the main window.
 */
KbdDisplay::~KbdDisplay()
{
	//settings.setValue("MainWindowGeometry", geometry());
}

void KbdDisplay::setUpToolbar()
{
	keyboardsComboBox = new QComboBox(ui->toolBar);
	ui->toolBar->addWidget(keyboardsComboBox);
	QDir dir;
	dir.cd("keyboards");
	QStringList filters;
    filters << "*.xml";
    dir.setNameFilters(filters);
	QStringList kbdList = dir.entryList();
	keyboardsComboBox->addItems(kbdList);
	connect(keyboardsComboBox, SIGNAL(currentTextChanged(QString)), SLOT(loadKbd(QString)));
}


void KbdDisplay::loadKbd(QString filename)
{
	filename = QDir::currentPath() + "/keyboards/" + filename;
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
	
	drawGroup(reader, nullptr, 0.0, 0.0, 0.0);
	//printItemTree();
	f.close();
	model->setKeys(keys);
	scene->setSceneRect(scene->itemsBoundingRect());
	qDebug() << "scene->itemsBoundingRect :" << scene->itemsBoundingRect();
	
	//FIXME next line crashes the programm ... why????
	//view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

QGraphicsItem* KbdDisplay::drawGroup(QXmlStreamReader &reader, QGraphicsItemGroup* parent, 
									 double keywidth, double keyheight, double margin)
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
		
		double x = 0.0, y = 0.0;
		double curKeyheight = keyheight, curKeywidth = keywidth, curMargin = margin;
		attr = reader.attributes();
		if (attr.hasAttribute("x"))
			x = SIZEFACTOR * attr.value("x").toDouble();
		if (attr.hasAttribute("y"))
			y = SIZEFACTOR * attr.value("y").toDouble();
		if (attr.hasAttribute("id"))
			id = attr.value("id").toString();
		
		
		if (reader.name() == "keyboard")
		{
			id = attr.value("name").toString();
			if (attr.hasAttribute("keywidth"))
				keywidth = SIZEFACTOR * attr.value("keywidth").toDouble();
			if (attr.hasAttribute("keyheight"))
				keyheight = SIZEFACTOR * attr.value("keyheight").toDouble();
			if (attr.hasAttribute("margin"))
				margin = SIZEFACTOR * attr.value("margin").toDouble();
			continue;
		}
		else if (reader.name() == "group" || reader.name() == "row")
		{
			if (reader.isEndElement())
				return resultItem;
			if (attr.hasAttribute("keywidth"))
				curKeywidth = SIZEFACTOR * attr.value("keywidth").toDouble();
			if (attr.hasAttribute("keyheight"))
				curKeyheight = SIZEFACTOR * attr.value("keyheight").toDouble();
			if (attr.hasAttribute("margin"))
				curMargin = SIZEFACTOR * attr.value("margin").toDouble();
			QGraphicsItemGroup * group = new QGraphicsItemGroup();
			scene->addItem(group);
			//if (parent != nullptr)
			//	parent->addToGroup(group);
			item = group;  //TODO clean up unneccessary variables
			drawGroup (reader, group, curKeywidth, curKeyheight, curMargin);
			resultItem = item;
			groups[id] = (QGraphicsItemGroup*) item;
			groupX = x;
			groupY = y;
			curMargin = 0.0;
			qDebug() << id << ": " << group->childrenBoundingRect();
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
					polygon.append(QPointF(SIZEFACTOR * plist[0].toDouble(),
										   SIZEFACTOR * plist[1].toDouble()));
				}
				item = new QGraphicsKeyItem(polygon);
			}
			else if (attr.hasAttribute("d"))
			{
				qreal diameter = SIZEFACTOR * attr.value("d").toDouble();
				item = new QGraphicsKeyItem(diameter);
			}
			else
			{
				double_t w = curKeywidth, h = curKeyheight;
				if (attr.hasAttribute("w"))
					w = SIZEFACTOR * attr.value("w").toDouble();
				if (attr.hasAttribute("h"))
					h = SIZEFACTOR * attr.value("h").toDouble();
				item = new QGraphicsKeyItem(QRectF(0.0, 0.0, w, h));
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
			QRectF bRect;
			if (lastItem->type() == QGraphicsKeyItem::Type)
				bRect = lastItem->mapRectToParent(
					qgraphicsitem_cast<QGraphicsKeyItem*>(lastItem)->placementRect());
			else {
				bRect = lastItem->mapRectToParent(lastItem->boundingRect()
						| lastItem->childrenBoundingRect());
				qreal rMargin = SIZEFACTOR / 4.0;
				bRect = bRect.marginsRemoved(QMarginsF(rMargin, rMargin, rMargin, rMargin));
			}
			
			if (corner == "ne")
				p = bRect.topRight() + QPointF(margin, 0.0);
			else if (corner == "sw")
				p = bRect.bottomLeft() + QPointF(0.0, margin);
			else if (corner == "se")
				p = bRect.bottomRight() + QPointF(margin, margin);
			else if (corner == "nw")
				p = bRect.topLeft();
			else
				qDebug() << "Warning: invalid corner value \"" << corner << "\""; 
		}
		
		
		item->moveBy(groupX + x + p.x(), groupY + y + p.y());
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


// TODO do i need this function???
void KbdDisplay::keyChanged(KeyItem* item)
{
	foreach(QGraphicsItem* it, keys.values(item->keyId))
	{
		if (it->type() == QGraphicsKeyItem::Type)
			((QGraphicsKeyItem*)it)->updateContent();
		it->update();
	}
}

void KbdDisplay::exportSVG()
{
	QString svgFile = QFileDialog::getSaveFileName(this, tr("Save As..."), 
		QDir::currentPath(), tr("SVG file (*.svg)")
	);
	ui->graphicsView->exportSVG(svgFile);
}

void KbdDisplay::print()
{
	QPrinter printer(QPrinter::HighResolution);
	printer.setOrientation(QPrinter::Landscape);
	QPrintDialog printDialog(&printer);
	if (printDialog.exec() == QDialog::Accepted) {
		ui->graphicsView->print(printer);
	}
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



