/*
 * Version: 0.0.7
 */

#include <QApplication>
#include <QTranslator>
#include <QMessageBox>

#include "qt_fll_iso2usb_gui.h"

int main( int argc, char** argv )
{
	
	QApplication app(argc, argv);

	QString appName = "qt-fll-iso2usb.gui";
	QString version = "0.0.8";

	QString locale = QLocale::system().name().left(2);
	QTranslator translator;
	translator.load(appName+"_"+locale, "/usr/share/"+appName+"/translations" );
		
	if ( getuid() ){
		QWidget* window = new QWidget();
		QMessageBox::warning(window, appName, app.tr("You need to run the application as root"));
		delete window;
		exit(1);
	}
	
	app.installTranslator(&translator);
	qt_fll_iso2usb_gui fllISO2usbGUI(appName, version, locale);
	
	fllISO2usbGUI.show();
	return app.exec();
	
}
