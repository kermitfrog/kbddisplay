#ifndef kbdisplay_h
#define kbdisplay_h

#include <QtGui>
#include "ui_kbddisplay.h"

/*!
 * @brief Applications main window.
 */
class KbdDisplay : public QMainWindow
{
Q_OBJECT
public:
	KbdDisplay();
	virtual ~KbdDisplay();
    void paintStuff();
    void loadKbd(QString filename);
    void printItemTree(QGraphicsItem * root = nullptr, int level = 0);
	
public slots:

protected:	
	virtual void resizeEvent(QResizeEvent * event);
	QTableView *table;
	QGraphicsView *view;
	QGraphicsScene *scene;
	QMultiMap<QString,QGraphicsItem*> keys;
	QMap<QString,QGraphicsItemGroup*> groups;
	QGraphicsItem * drawGroup(QXmlStreamReader &reader, QGraphicsItemGroup * parent,
		double keywidth, double keyheight, bool row = false);
    QGraphicsItem* getItem(QString value);
    void addLabel(QString id, QGraphicsItem* item);
	

private:
	Ui_KbdDisplay * ui;
};

#endif // kbdisplay_h
