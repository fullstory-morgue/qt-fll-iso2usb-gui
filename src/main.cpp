/*
 * Version: 0.0.11
 */

#include <QApplication>
#include <QTranslator>
#include <QMessageBox>

#include "qt_fll_iso2usb_gui.h"

int main( int argc, char** argv )
{
	
	QApplication app(argc, argv);

	QString appName = "qt-fll-iso2usb-gui";
	QString version = "0.0.11";

	QString locale = QLocale::system().name().left(2);
	QTranslator translator;
	translator.load(appName+"_"+locale, "/usr/share/"+appName+"/translations" );
	
	bool in_live = false;
	bool is_toram = false;
	
	if ( ! getenv("FLL_DISTRO_MODE") ){
		QWidget* window = new QWidget();
		QMessageBox::warning(window, appName, app.tr("Needed environment variable not set. Please run the application with qt-fll-iso2usb-gui.bash"));
		delete window;
		exit(1);
	} else {
		if ( ! strcmp( getenv("FLL_DISTRO_MODE"), "live" ) )
			in_live = true;
		if ( ! strcmp( getenv("CHEATCODE_TORAM"), "0" ) )
			is_toram = false;
		else
			is_toram = true;
			
	}
	
	if ( getuid() ){
		QWidget* window = new QWidget();
		QMessageBox::warning(window, appName, app.tr("You need to run the application as root"));
		delete window;
		exit(1);
	}
	
	app.installTranslator(&translator);
	qt_fll_iso2usb_gui fllISO2usbGUI(appName, version, locale, in_live, is_toram);
	
	fllISO2usbGUI.show();
	return app.exec();
	
}
