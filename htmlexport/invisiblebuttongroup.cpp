/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : A KIPI plugin to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free.fr>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// vim: set tabstop=4 shiftwidth=4 noexpandtab:

// Self
#include "invisiblebuttongroup.moc"

// Qt
#include <QAbstractButton>
#include <QButtonGroup>

// KDE
#include <kconfigdialogmanager.h>

// Local

namespace KIPIHTMLExport {


class InvisibleButtonGroupPrivate {
public:
	QButtonGroup* mGroup;
};


InvisibleButtonGroup::InvisibleButtonGroup(QWidget* parent)
: QWidget(parent)
, d(new InvisibleButtonGroupPrivate) {
	hide();
	d->mGroup = new QButtonGroup(this);
	d->mGroup->setExclusive(true);
	connect(d->mGroup, SIGNAL(buttonClicked(int)), SIGNAL(selectionChanged(int)) );
	const QString name = metaObject()->className();
	if (!KConfigDialogManager::propertyMap()->contains(name)) {
		KConfigDialogManager::propertyMap()->insert(name, "current");
		KConfigDialogManager::changedMap()->insert(name, SIGNAL(selectionChanged(int)));
	}
}


InvisibleButtonGroup::~InvisibleButtonGroup() {
	delete d;
}


int InvisibleButtonGroup::selected() const {
	return d->mGroup->checkedId();
}


void InvisibleButtonGroup::addButton(QAbstractButton* button, int id) {
	d->mGroup->addButton(button, id);
}


void InvisibleButtonGroup::setSelected(int id) {
	QAbstractButton* button = d->mGroup->button(id);
	if (button) {
		button->setChecked(true);
	}
}


} // namespace
