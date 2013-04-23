/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : a plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#ifndef JALBUMCONFIG_H
#define JALBUMCONFIG_H

// Qt includes

#include <QLabel>
#include <QCheckBox>

// KDE includes

#include <kdialog.h>
#include <klineedit.h>

namespace KIPIJAlbumExportPlugin
{

class JAlbum;

class JAlbumEdit : public KDialog
{
    Q_OBJECT

public:

    JAlbumEdit(QWidget* const pParent, JAlbum* const pJAlbum, const QString& title);
    ~JAlbumEdit();

private Q_SLOTS:

    void slotShowAlbumDialogClicked(bool);
    void slotShowJarDialogClicked(bool);
    void slotAlbumsPathChanged(const QString&);
    void slotJarPathChanged(const QString&);
    void slotOk();

Q_SIGNALS:

//    void signalAlbumsPathChanged(const QString& albums);

private:

    void updateAlbumsPath();
    void updateJarPath();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIJAlbumExportPlugin

#endif /* JALBUMCONFIG_H */
