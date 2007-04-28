/* ============================================================
 * File  : svedialog.h
 * Author: Joern Ahrens <joern.ahrens@kdemail.net>
 * Date  : 2006-01-06
 * Description :
 *
 * Copyright 20066 by Joern Ahrens <joern.ahrens@kdemail.net>
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
 * ============================================================ */

#ifndef FIRSTRUNDLG_H
#define FIRSTRUNDLG_H

// Include files for KDE

#include <kdialogbase.h>

// Include files for KIPI

#include <libkipi/interface.h>

class KURLRequester;
class QString;

namespace KIPISimpleViewerExportPlugin
{
    
/**
 * To avoid licensing problems with some distributions, the SimpleViewer
 * Flash cannot be shipped with the plugin. During the first run of the
 * plugin, the user has to download SimpleViewer from its homepage and point
 * the plugin to that archive to install it. This is done by this dialog.
 */   

class FirstRunDlg : public KDialogBase
{
    Q_OBJECT

public:
 
    FirstRunDlg(QWidget *parent=0);
    ~FirstRunDlg();

    /**
     * Returns the URL, where the SimpleViewer package is stored
     */
    QString getURL();
    
private:
        
    KURLRequester       *m_urlRequester;
    QString              m_url;
    
private slots:
        
    /**
     * Opens the browser with the SimpleViewer download page
     */
    void slotDownload(const QString &url);
    
    /**
     * Starts the installation of SimpleViewer
     */
    void slotURLSelected(const QString &url);
};

}

#endif /* FIRSTRUNDLG_H */

