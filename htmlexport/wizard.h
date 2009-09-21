// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 by Aurelien Gateau <aurelien dot gateau at free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.

*/
#ifndef WIZARD_H
#define WIZARD_H

// KDE
#include <kassistantdialog.h>

namespace KIPI {
class Interface;
}

namespace KIPIHTMLExport {

class GalleryInfo;

/**
 * The wizard used by the user to select the various settings.
 */
class Wizard : public KAssistantDialog {
Q_OBJECT
public:
	Wizard(QWidget* parent, GalleryInfo* info, KIPI::Interface*);
	~Wizard();

protected Q_SLOTS:
	virtual void accept();

private Q_SLOTS:
	void updateCollectionSelectorPageValidity();
	void updateFinishPageValidity();
	void slotThemeSelectionChanged();
    void slotHelp();

private:
	struct Private;
	Private* d;
};


} // namespace

#endif /* WIZARD_H */
