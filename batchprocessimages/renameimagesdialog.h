/* ============================================================
 *
 * Copyright (C) 2003-2005 Gilles CAULIER <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005 by Owen Hirst <n8rider@sbcglobal.net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
 *  
 * ============================================================ */

#ifndef RENAMEIMAGESDLG_H
#define RENAMEIMAGESDLG_H

#include <kdialogbase.h>
#include <kurl.h>

// Local includes

#include "kpaboutdata.h"

namespace KIPI
{
class Interface;
}

namespace KIPIBatchProcessImagesPlugin
{

class RenameImagesWidget;

class RenameImagesDialog : public KDialogBase
{
    Q_OBJECT

public:

    RenameImagesDialog(const KURL::List& images,
                    KIPI::Interface* interface,
                    QWidget* parent);
    ~RenameImagesDialog();

private:

    RenameImagesWidget* m_widget;

    KIPIPlugins::KPAboutData* m_about;

private slots:

    void slotHelp();
};

}

#endif /* RENAMEIMAGESDIALOG_H */
