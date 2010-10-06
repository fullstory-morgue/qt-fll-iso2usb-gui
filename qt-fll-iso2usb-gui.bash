#!/bin/bash
# qt-fll-iso2usb-gui.bash
# Copyright: © 2007 Horst Tritremmel <peter_weber69@gmx.at>
# Copyright: © 2007-2009 Joaquim Boura <x-un-i@berlios.de>
# Copyright: © 2007-2008 Kel Modderman <kel@otaku42.de>
# Copyright: © 2008 Stefan Lippers-Hollmann <s.l-h@gmx.de>
# Copyright: © 2009-2010 Nikolas Poniros <edhunter@sidux.com>
# 
# install-usb-gui.bash is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
# 
# install-usb-gui.bash is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.

# exec code -----------------------------------------------------
INSTALL_USB_GUI="$(which qt-fll-iso2usb-gui)" || INSTALL_USB_GUI="/usr/bin/qt-fll-iso2usb-gui"
INSTALL_ISO2USB="$(which fll-iso2usb)"     || INSTALL_ISO2USB="/usr/sbin/fll-iso2usb"


# fll-iso2usb check ----------------------------------
if [ ! -x "$INSTALL_ISO2USB" ]; then
	printf "\n$INSTALL_ISO2USB not found\n\n"
	exit 1
fi


# distro source -------------------------------------------------
if [ -e "/etc/default/distro" ]; then
	. /etc/default/distro
else
	printf "\n/etc/default/distro not found\n\n"
	exit 1
fi

export FLL_DISTRO_MODE
export CHEATCODE_TORAM=$(grep -c toram /proc/cmdline) 

if [ `id -u` -ne 0 ]; then
	exec su-to-root -X -c $INSTALL_USB_GUI
else
	exec $INSTALL_USB_GUI
fi
