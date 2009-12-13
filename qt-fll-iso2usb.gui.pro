###########################################################################################
##
##	Author    : Nikolas Poniros <edhunter@sidux.com>
##	Project   : qt-fll-iso2usb.gui
##	FileName  : qt-fll-iso2usb.gui.pro
##	Date      : 2009-11-08T09:47:09
##	License   : GPL2
##	Comment   : 
##	Home Page   : 
##
##	This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
##	WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
##
###########################################################################################

QT +=  webkit
TEMPLATE = app
TARGET	= qt-fll-iso2usb.gui
CONFIG	+= qt
BUILD_PATH =  ./build

RESOURCES = icons/qt-fll-iso2usb.gui.qrc

FORMS = src/qt_fll_iso2usb_gui.ui

HEADERS = src/qt_fll_iso2usb_gui.h

SOURCES	= src/main.cpp \
	src/qt_fll_iso2usb_gui.cpp
