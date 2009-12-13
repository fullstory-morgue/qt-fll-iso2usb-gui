#include <QCompleter>
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>
#include <iostream>
#include <signal.h>

#include "qt_fll_iso2usb_gui.h"

QProcess *fll_iso2usb = new QProcess();

void cleanup(int para)
{

	// if is running/starting
	if ( fll_iso2usb->state() ){
		fll_iso2usb->terminate();
		if ( ! fll_iso2usb->waitForFinished ( 5000 ) )
			fll_iso2usb->kill();
	}

	exit (para);
}

qt_fll_iso2usb_gui::qt_fll_iso2usb_gui(QString name, QString ver, QString loc, QMainWindow *parent)
    : QMainWindow(parent)
{
	setupUi(this);
	
	locale = loc;
	version = ver;
	appName = name;
	
	set_defaults();
	
	// checkBox connections
	connect(checkBox_Persist, SIGNAL(clicked(bool)), spinBox_Persist, SLOT(setVisible(bool)));
	connect(checkBox_Persist, SIGNAL(clicked(bool)), label_Persist, SLOT(setVisible(bool)));
	connect(checkBox_Label, SIGNAL(clicked(bool)), lineEdit_Label, SLOT(setVisible(bool)));
	
	// pushButton connections
	connect(pushButton_Start, SIGNAL(clicked()), this, SLOT(start()));
	connect(pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pushButton_Browse, SIGNAL(clicked()), this, SLOT(browse_for_iso()));
	connect(pushButton_Show_Cmd, SIGNAL(clicked()), this, SLOT(show_cmd()));
	connect(pushButton_Refresh, SIGNAL(clicked()), this, SLOT(list_devices()));
	connect(pushButton_Cheatcodes, SIGNAL(clicked()), this, SLOT(call_cheatcodes()));
	connect(pushButton_Help, SIGNAL(clicked()), this, SLOT(help()));
	connect(pushButton_About, SIGNAL(clicked()), this, SLOT(about()));
	
	timer = new QTimer();
	
	signal (SIGTERM, cleanup);
	signal (SIGKILL, cleanup);
	signal (SIGINT, cleanup);
	
}

qt_fll_iso2usb_gui::~qt_fll_iso2usb_gui()
{
}

void qt_fll_iso2usb_gui::set_defaults()
{

	spinBox_Persist->setVisible(false);
	label_Persist->setVisible(false);
	lineEdit_Label->setVisible(false);
	frame_Progressbar->setVisible(false);
	
	// add auto-complete abilities to the lineEdit_ISO object
	QCompleter *auto_complete = new QCompleter(this);
	auto_complete->setModel(new QDirModel(auto_complete));
	lineEdit_ISO->setCompleter(auto_complete);

	textEdit_Cmd_Output->setReadOnly(true);

	list_devices();
	set_about();
	set_help();

	spinBox_Persist->setMinimum(10);
	spinBox_Persist->setMaximum(90);

	lineEdit_Grub_Options->setText("lang="+locale);
}

void qt_fll_iso2usb_gui::closeEvent(QCloseEvent *event)
{
	// if is running/starting
	if ( fll_iso2usb->state() ){
		fll_iso2usb->terminate();
		if ( ! fll_iso2usb->waitForFinished ( 5000 ) )
			fll_iso2usb->kill();
	}

	event->accept();
}

int qt_fll_iso2usb_gui::start()
{

	textEdit_Cmd_Output->clear();
	
	QString cmd = "fll-iso2usb";
	if (set_cmd_options()){
		return 1;
	}
	
	pushButton_Close->setEnabled(false);
	pushButton_Start->setEnabled(false);
	pushButton_Show_Cmd->setEnabled(false);
	frame_Progressbar->setVisible(true);
	frame_Logo->setVisible(false);

	connect(fll_iso2usb, SIGNAL(error(QProcess::ProcessError )), this, SLOT(process_errored(QProcess::ProcessError)));
	connect(fll_iso2usb, SIGNAL(readyReadStandardOutput()), this, SLOT(print_cmd_output()));
	connect(fll_iso2usb, SIGNAL(readyReadStandardError()), this, SLOT(print_cmd_error()));
	connect(fll_iso2usb, SIGNAL(finished(int)), this, SLOT(process_finished(int)));
	fll_iso2usb->start(cmd, cmd_options);

	start_progressbar();
	
	cmd_options.clear();
	
	return 0;
}

void qt_fll_iso2usb_gui::process_finished(int i)
{
	stop_progressbar();
	
	// process_finished call comes from process_errored()
	if (i == 256){
		textEdit_Cmd_Output->setTextColor(Qt::red);
		textEdit_Cmd_Output->append( tr("Some error occured. Installation NOT successful.") );
	} else if ( i == 0 ){
		textEdit_Cmd_Output->setTextColor(Qt::blue);
		textEdit_Cmd_Output->append(tr("Installation finished successfully!"));
	} else {
		textEdit_Cmd_Output->setTextColor(Qt::red);
		textEdit_Cmd_Output->append(tr("Some error occured. Installation NOT successful. Exit code was:") + " " + QString::number(i)
);
	}
	
	pushButton_Close->setEnabled(true);
	pushButton_Start->setEnabled(true);
	pushButton_Show_Cmd->setEnabled(true);
	frame_Progressbar->setVisible(false);
	frame_Logo->setVisible(true);

}

void qt_fll_iso2usb_gui::process_errored(QProcess::ProcessError error){

	disconnect(fll_iso2usb, SIGNAL(finished(int)), this, SLOT(process_finished(int))); 

	if ( error == 0 ){
		textEdit_Cmd_Output->setTextColor(Qt::red);
		textEdit_Cmd_Output->append(tr("fll-iso2usb failed to start! Are you sure it is installed?") + "\n");
	} else if ( error == 1 ){
		textEdit_Cmd_Output->setTextColor(Qt::red);
		textEdit_Cmd_Output->append(tr("fll-iso2usb crashed!") + "\n");
	} else if ( error == 5 ) {
		textEdit_Cmd_Output->setTextColor(Qt::red);
		textEdit_Cmd_Output->append(tr("fll-iso2usb unknown error!") + "\n");
	}
	
	process_finished(256);
	
}

int qt_fll_iso2usb_gui::set_cmd_options()
{
	cmd_options.append("--device");
	cmd_options.append(comboBox_Device->currentText());
	cmd_options.append("--iso");
	if (lineEdit_ISO->text() == ""){
		QMessageBox::warning(this, "fll-iso2usb-gui", tr("You have not selected an ISO"));
		cmd_options.clear();
		return 1;
	} else {
		cmd_options.append(lineEdit_ISO->text());
	}
	
	if (checkBox_Persist->isChecked()){
		cmd_options.append("--persist");
		cmd_options.append("--percent");
		cmd_options.append(QString::number(spinBox_Persist->value()));
	}
	
	if (checkBox_Label->isChecked()){
		cmd_options.append("--label");
		cmd_options.append(lineEdit_Label->text());
	}
		
	if(lineEdit_Grub_Options->text() != ""){
		cmd_options.append("--");
		cmd_options.append(lineEdit_Grub_Options->text());
	}
	return 0;
}

void qt_fll_iso2usb_gui::list_devices()
{

	comboBox_Device->clear();
	
	QDir *device_dir = new QDir("/dev/disk/by-id/");
	QStringList device_list = device_dir->entryList();
	
	int i = 0;
	while ( i < device_list.length() ){
		if ( ! device_list.at(i).startsWith("usb-") || \
			device_list.at(i).contains("part") ){
				device_list.removeAt(i);
				continue;
			} else {
				device_list.replace(i, device_dir->path() + "/" + device_list.at(i));
				QString device_name = QFile::symLinkTarget(device_list.at(i));
				// Use this to remove USB cd-roms
				if ( device_name.contains(QRegExp ( "*[0-9]", Qt::CaseInsensitive, QRegExp::Wildcard ) ) ){
					device_list.removeAt(i);
					continue;
				}
				// Replace the name from /dev/disk/by-id with the actuall device name in /dev
				device_list.replace(i, device_name);
				i++;
			}
	}
	
	if ( device_list.isEmpty() ){
		QMessageBox::warning(this, appName, tr("No USB disks were detected! Please connect one and click the refresh button"));
	}
	
	comboBox_Device->addItems(device_list);
	
	delete device_dir;
}

void qt_fll_iso2usb_gui::browse_for_iso(){

	QFileDialog browse(this, tr("Choose an ISO"));
	// allow the user to select only an existing file
	browse.setFileMode(QFileDialog::ExistingFile);
	
	browse.setDirectory(QDir::currentPath());

	QStringList iso;

	browse.exec();
	iso = browse.selectedFiles();

	// only set lineEdit_Fonts if "Choose" was selected in the dialog
	if(browse.result()){
		lineEdit_ISO->setText(iso.first());
	}
}

/*
 * This function just displays the command that will be used to call
 * fll-iso2usb
 */
void qt_fll_iso2usb_gui::show_cmd()
{
	// set the options for the user to see the command
	if (!set_cmd_options()){
		QString cmd = "fll-iso2usb";
		for (int i = 0; i < cmd_options.length(); i++){
			cmd += " " + cmd_options.at(i);
		}
		QMessageBox::information(this, appName, cmd);
		// options will be set again with start
		cmd_options.clear();
	}
}

/*
 * =====================================================
 * Code to handle output from fll-iso2usb
 * =====================================================
 */
 
/*
 * This function prints all output form fll-iso2usb including errors
 * which are not printed to stderr by the script. Those start with "E:"
 */
void qt_fll_iso2usb_gui::print_cmd_output()
{
	cmd_output = fll_iso2usb->readAllStandardOutput();
	if (cmd_output.startsWith("E:")){
		textEdit_Cmd_Output->setTextColor(Qt::red);
		textEdit_Cmd_Output->append(cmd_output);
	// use 2nd if to avoid having fdisk etc print their output
	} else if (cmd_output.startsWith("O:")){
		textEdit_Cmd_Output->setTextColor(Qt::black);
		textEdit_Cmd_Output->append(cmd_output);
	}
}

/* 
 * This function will be used in case some command error occures during
 * fll-iso2usb execution (stderr). Errors with script options will 
 * be printed by print_cmd_output
 */
void qt_fll_iso2usb_gui::print_cmd_error()
{
	cmd_output = fll_iso2usb->readAllStandardError();
	textEdit_Cmd_Output->setTextColor(Qt::red);
	textEdit_Cmd_Output->append(cmd_output);
}

/*
 * ===========================================
 * Code to handle custom progressbar
 * ===========================================
 */
void qt_fll_iso2usb_gui::start_progressbar()
{

	frame_1->setAutoFillBackground (true);
	lastFrame_was = 1;
	connect(timer, SIGNAL(timeout()), this, SLOT(update_progressbar()));
	timer->start(250);
	
}

void qt_fll_iso2usb_gui::update_progressbar()
{

	if ( frame_1->autoFillBackground ()){
		frame_1->setAutoFillBackground (false);
		frame_2->setAutoFillBackground (true);
	} else if ( frame_2->autoFillBackground ()){
		frame_2->setAutoFillBackground (false);
		if ( lastFrame_was == 1 ){
			frame_3->setAutoFillBackground (true);
			lastFrame_was = 2;
		} else {
			frame_1->setAutoFillBackground (true);
			lastFrame_was = 1;
		}
	} else if ( frame_3->autoFillBackground ()){
		frame_3->setAutoFillBackground (false);
		if ( lastFrame_was == 2 ){
			frame_4->setAutoFillBackground (true);
			lastFrame_was = 3;
		} else {
			frame_2->setAutoFillBackground (true);
			lastFrame_was = 2;
		}
	} else if ( frame_4->autoFillBackground ()){
		frame_4->setAutoFillBackground (false);
		if ( lastFrame_was == 3 ){
			frame_5->setAutoFillBackground (true);
			lastFrame_was = 4;
		} else {
			frame_3->setAutoFillBackground (true);
			lastFrame_was = 3;
		}
	} else if ( frame_5->autoFillBackground ()){
		frame_5->setAutoFillBackground (false);
		frame_4->setAutoFillBackground (true);
	}
}

void qt_fll_iso2usb_gui::stop_progressbar()
{

	if ( frame_1->autoFillBackground ()){
		frame_1->setAutoFillBackground (false);
	} else if ( frame_2->autoFillBackground ()){
		frame_2->setAutoFillBackground (false);
	} else if ( frame_3->autoFillBackground ()){
		frame_3->setAutoFillBackground (false);
	} else if ( frame_4->autoFillBackground ()){
		frame_4->setAutoFillBackground (false);
	} else if ( frame_5->autoFillBackground ()){
		frame_5->setAutoFillBackground (false);
	}
	
	timer->stop();
}

void qt_fll_iso2usb_gui::call_cheatcodes(){

	help_window.setGeometry (0,0,800,600);

	help_page.setParent(&help_window);

	help_page.setGeometry(0,0,800,600);

	QFile man_page("/usr/share/sidux-manual/"+locale+"/cheatcodes-"+locale+".htm");
	
	QUrl url;
	
	// if manual exists localy use it. else go online
	if ( man_page.exists()){
		url.setUrl("/usr/share/sidux-manual/"+locale+"/cheatcodes-"+locale+".htm");
	}else{
		url.setUrl("http://manual.sidux.com/"+locale+"/cheatcodes-"+locale+".htm");
	}
	
	help_page.load(url);

	help_window.show();

}

void qt_fll_iso2usb_gui::about()
{
	QMessageBox::information(this, appName, about_msg);
}

void qt_fll_iso2usb_gui::set_about()
{
	about_msg = appName + " " + tr("Version") + ": " + version + "\n";
	
	about_msg += "\n" + tr("Developer") + ":\n";
	about_msg += "Nikolas Poniros <edhunter@sidux.com>\n";

     about_msg += "\n" + tr("License") + ": GPLv2";
}

void qt_fll_iso2usb_gui::help()
{
	QMessageBox::information(this, appName, help_msg);
}

void qt_fll_iso2usb_gui::set_help()
{
	help_msg = appName + " " + tr("is a graphical front-end for fll-iso2usb which allows you to easily create bootable USB-sticks") + "\n\n";

	help_msg += tr("The \"Show Command\" button shows you the command that will be used to run fll-iso2usb with") + "\n\n";
	
	help_msg += tr("For more information please have a look at man fll-iso2usb");
	
}

