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

#ifndef INVISIBLEBUTTONGROUP_H
#define INVISIBLEBUTTONGROUP_H

// Qt
#include <QWidget>

// KDE

// Local

class QAbstractButton;

namespace KIPIHTMLExport {


class InvisibleButtonGroupPrivate;
class InvisibleButtonGroup : public QWidget {
	Q_OBJECT
	Q_PROPERTY(int current READ selected WRITE setSelected)
public:
	explicit InvisibleButtonGroup(QWidget* parent = 0);
	~InvisibleButtonGroup();

	int selected() const;

	void addButton(QAbstractButton* button, int id);

public Q_SLOTS:
	void setSelected(int id);

Q_SIGNALS:
	void selectionChanged(int id);

private:
	InvisibleButtonGroupPrivate* const d;
};


} // namespace

#endif /* INVISIBLEBUTTONGROUP_H */
