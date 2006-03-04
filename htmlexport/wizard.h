// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 Aurelien Gateau

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef WIZARD_H
#define WIZARD_H   

// KDE
#include <kwizard.h>

namespace KIPI {
class Interface;
}

namespace KIPIHTMLExport {

class GalleryInfo;

class Wizard : public KWizard {
Q_OBJECT
public:
	Wizard(QWidget* parent, KIPI::Interface* interface, GalleryInfo* info);
	~Wizard();

protected slots:
	virtual void accept();

private slots:
	void updateFinishButton();
	void updateAppearancePageNextButton();

private:
	struct Private;
	Private* d;
};


} // namespace

#endif /* WIZARD_H */
