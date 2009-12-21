#ifndef QT_FLL_ISO2USB_GUI_H
#define QT_FLL_ISO2USB_GUI_H 

#include <QCloseEvent>
#include <QProcess>
#include <QTimer>
#include <QDialog>
#include <QWebView>

#include "ui_qt_fll_iso2usb_gui.h"

class qt_fll_iso2usb_gui : public QMainWindow, Ui::qt_fll_iso2usb_gui
{
	Q_OBJECT
	
	public:
		qt_fll_iso2usb_gui(QString, QString, QString, bool, bool, QMainWindow *parent = 0);
		~qt_fll_iso2usb_gui();
	
	private slots:
		int start();
		void browse_for_iso();
		void show_cmd();
		void print_cmd_output();
		void print_cmd_error();
		void process_finished(int);
		void process_errored(QProcess::ProcessError);
		void list_devices();
		void update_progressbar();
		void call_cheatcodes();
		void help();
		void about();

	private:
		int set_cmd_options();
		void start_progressbar();
		void stop_progressbar();
		void set_defaults();
		void set_about();
		void set_help();
		QStringList cmd_options;
		QString cmd_output;
		QString about_msg;
		QString help_msg;
		QString appName;
		QString version;
		QString locale;
		QTimer *timer;
		QDialog help_window;
		QWebView help_page;
		int lastFrame_was;
		bool is_toram;
		bool in_live;
		
	protected:
		void closeEvent(QCloseEvent* event);
};

#endif // QT_FLL_ISO2USB_GUI_H
