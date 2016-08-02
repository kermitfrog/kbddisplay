#ifndef kbdisplay_h
#define kbdisplay_h

#include <QtGui>
#include "ui_kbddisplay.h"
#include "keyitemmodel.h"
#include "qgraphicskeyitem.h"
#include "keyboardview.h"
#include "global.h"


/*!
 * @brief Applications main window.
 */
class KbdDisplay : public QMainWindow
{
Q_OBJECT
public:
	KbdDisplay();
	virtual ~KbdDisplay();
    void printItemTree(QGraphicsItem * root = nullptr, int level = 0);
	
public slots:
	void keyChanged(KeyItem* item);
	void open();
	void save();
	void saveAs();
	void exportSVG();
	void print();
    void loadKbd(QString filename);

protected:	
	virtual void resizeEvent(QResizeEvent * event);
	QTableView *table;
	KeyboardView *view;
	QGraphicsScene *scene;
	KeyItemModel *model;
	QComboBox *keyboardsComboBox;
	QMultiMap<QString,QGraphicsItem*> keys;
	QMap<QString,QGraphicsItemGroup*> groups;
	QGraphicsItem * drawGroup(QXmlStreamReader &reader, QGraphicsItemGroup * parent,
		double keywidth, double keyheight, double margin);
    QGraphicsItem* getItem(QString value);
    void setUpToolbar();
	QString filename;
	

private:
	Ui_KbdDisplay * ui;
};

#endif // kbdisplay_h
