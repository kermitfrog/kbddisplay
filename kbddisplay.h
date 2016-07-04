#ifndef kbdisplay_h
#define kbdisplay_h

#include <QtGui>
#include "ui_kbddisplay.h"
#include "keyitemmodel.h"
#include "qgraphicskeyitem.h"
#include "keyboardview.h"

/*!
 * @brief Applications main window.
 */
class KbdDisplay : public QMainWindow
{
Q_OBJECT
public:
	KbdDisplay();
	virtual ~KbdDisplay();
    void loadKbd(QString filename);
    void printItemTree(QGraphicsItem * root = nullptr, int level = 0);
	
public slots:
	void keyChanged(KeyItem* item);

protected:	
	virtual void resizeEvent(QResizeEvent * event);
	QTableView *table;
	KeyboardView *view;
	QGraphicsScene *scene;
	KeyItemModel *model;
	QMultiMap<QString,QGraphicsItem*> keys;
	QMap<QString,QGraphicsItemGroup*> groups;
	QGraphicsItem * drawGroup(QXmlStreamReader &reader, QGraphicsItemGroup * parent,
		double keywidth, double keyheight, bool row = false);
    QGraphicsItem* getItem(QString value);
	

private:
	Ui_KbdDisplay * ui;
};

#endif // kbdisplay_h
