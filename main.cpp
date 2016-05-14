#include <QApplication>
#include "kbddisplay.h"

/*!
 * @brief  A completely normal main function for a GUI application.
 */
int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	KbdDisplay foo;
	foo.show();
	return app.exec();
}
