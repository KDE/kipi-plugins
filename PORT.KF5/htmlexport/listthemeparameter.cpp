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

#include "listthemeparameter.h"

// KDE includes

#include <kcombobox.h>
#include <kconfiggroup.h>

namespace KIPIHTMLExport {

static const char* ITEM_VALUE_KEY = "Value_";
static const char* ITEM_CAPTION_KEY = "Caption_";

struct ListThemeParameter::Private {
	QStringList mOrderedValueList;
	QMap<QString, QString> mContentMap;
};

ListThemeParameter::ListThemeParameter() {
	d = new Private;
}

ListThemeParameter::~ListThemeParameter() {
	delete d;
}

void ListThemeParameter::init(const QByteArray& internalName, const KConfigGroup* configGroup) {
	AbstractThemeParameter::init(internalName, configGroup);

	for (int pos=0;; ++pos) {
		QString valueKey = QString("%1%2").arg(ITEM_VALUE_KEY).arg(pos);
		QString captionKey = QString("%1%2").arg(ITEM_CAPTION_KEY).arg(pos);
		if (!configGroup->hasKey(valueKey) || !configGroup->hasKey(captionKey)) {
			break;
		}

		QString value = configGroup->readEntry(valueKey);
		QString caption = configGroup->readEntry(captionKey);

		d->mOrderedValueList << value;
		d->mContentMap[value] = caption;
	}
}

QWidget* ListThemeParameter::createWidget(QWidget* parent, const QString& widgetDefaultValue) const {
	KComboBox* comboBox = new KComboBox(parent);

	QStringList::ConstIterator
		it = d->mOrderedValueList.constBegin(),
		end = d->mOrderedValueList.constEnd();
	for (;it!=end; ++it) {
		QString value = *it;
		QString caption = d->mContentMap[value];
		comboBox->addItem(caption);
		if (value == widgetDefaultValue) {
			comboBox->setCurrentIndex(comboBox->count() - 1);
		}
	}

	return comboBox;
}

QString ListThemeParameter::valueFromWidget(QWidget* widget) const {
	Q_ASSERT(widget);
	KComboBox* comboBox = static_cast<KComboBox*>(widget);
	return d->mOrderedValueList[comboBox->currentIndex()];
}

} // namespace

