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

// Self
#include "theme.h"

// KDE
#include <kdebug.h>
#include <kdesktopfile.h>
#include <kstandarddirs.h>
#include <kurl.h>

// Local
#include "colorthemeparameter.h"
#include "listthemeparameter.h"
#include "stringthemeparameter.h"

namespace KIPIHTMLExport {


static const char* AUTHOR_GROUP = "X-HTMLExport Author";
static const char* PARAMETER_GROUP_PREFIX = "X-HTMLExport Parameter ";
static const char* PARAMETER_TYPE_KEY = "Type";

static const char* STRING_PARAMETER_TYPE = "string";
static const char* LIST_PARAMETER_TYPE = "list";
static const char* COLOR_PARAMETER_TYPE = "color";

static Theme::List sList;


struct Theme::Private {
	KDesktopFile* mDesktopFile;
	KURL mURL;
	ParameterList mParameterList;

	void init(const QString& desktopFileName) {
		mDesktopFile=new KDesktopFile(desktopFileName, true /*read only*/);
		mURL.setPath(desktopFileName);

		readParameters();
	}

	void readParameters() {
		QStringList list = mDesktopFile->groupList();
		QStringList::Iterator it=list.begin(), end=list.end();
		for (;it!=end; ++it) {
			QString group = *it;
			if (!group.startsWith(PARAMETER_GROUP_PREFIX)) {
				continue;
			}

			QCString internalName = group.mid(qstrlen(PARAMETER_GROUP_PREFIX) ).utf8();

			KConfigGroupSaver saver(mDesktopFile, group);
			QString type = mDesktopFile->readEntry(PARAMETER_TYPE_KEY);
			AbstractThemeParameter* parameter;
			if (type == STRING_PARAMETER_TYPE) {
				parameter = new StringThemeParameter();
			} else if (type == LIST_PARAMETER_TYPE) {
				parameter = new ListThemeParameter();
			} else if (type == COLOR_PARAMETER_TYPE) {
				parameter = new ColorThemeParameter();
			} else {
				kdWarning() << "Parameter '" << internalName << "' has unknown type '" << type << "'. Falling back to string type\n";
				parameter = new StringThemeParameter();
			}
			parameter->init(internalName, mDesktopFile);
			mParameterList << parameter;
		}
	}
};


Theme::Theme() {
	d=new Private;
}


Theme::~Theme() {
	delete d->mDesktopFile;
	delete d;
}


const Theme::List& Theme::getList() {
	if (sList.isEmpty()) {
		QStringList internalNameList;
		QStringList list=KGlobal::instance()->dirs()->findAllResources("data", "kipiplugin_htmlexport/themes/*/*.desktop");
		QStringList::Iterator it=list.begin(), end=list.end();
		for (;it!=end; ++it) {
			Theme* theme=new Theme;
			theme->d->init(*it);
			QString internalName = theme->internalName();
			if (!internalNameList.contains(internalName)) {
				sList << Theme::Ptr(theme);
				internalNameList << internalName;
			}
		}
	}
	return sList;
}


Theme::Ptr Theme::findByInternalName(const QString& internalName) {
	const Theme::List& lst=getList();
	Theme::List::ConstIterator it=lst.begin(), end=lst.end();
	for (; it!=end; ++it) {
		Theme::Ptr theme = *it;
		if (theme->internalName() == internalName) {
			return theme;
		}
	}
	return 0;
}


QString Theme::internalName() const {
	KURL url = d->mURL;
	url.setFileName("");
	return url.fileName();
}


QString Theme::name() const {
	return d->mDesktopFile->readName();
}


QString Theme::comment() const {
	return d->mDesktopFile->readComment();
}


QString Theme::directory() const {
	return d->mURL.directory();
}


QString Theme::authorName() const {
	KConfigGroupSaver saver(d->mDesktopFile, AUTHOR_GROUP);
	return d->mDesktopFile->readEntry("Name");
}


QString Theme::authorUrl() const {
	KConfigGroupSaver saver(d->mDesktopFile, AUTHOR_GROUP);
	return d->mDesktopFile->readEntry("Url");
}


Theme::ParameterList Theme::parameterList() const {
	return d->mParameterList;
}


} // namespace
