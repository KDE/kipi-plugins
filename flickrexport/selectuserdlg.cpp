/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-16-05
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "selectuserdlg.moc"

// Qt includes

#include <QPushButton>
#include <QLabel>
#include <QProgressDialog>
#include <QPixmap>
#include <QCheckBox>
#include <QStringList>
#include <QSpinBox>
#include <QPointer>

// KDE includes

#include <kcombobox.h>
#include <klineedit.h>
#include <kmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <ktabwidget.h>
#include <krun.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kdeversion.h>
#include <kwallet.h>
#include <kpushbutton.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpprogresswidget.h"
#include "login.h"
#include "flickrtalker.h"
#include "flickritem.h"
#include "flickrlist.h"
#include "flickrnewphotosetdialog.h"
#include "flickrwidget.h"
#include "ui_flickralbumdialog.h"

namespace KIPIFlickrExportPlugin
{

SelectUserDlg::SelectUserDlg(QWidget* const parent,const QString& serviceName)
    : KPToolDialog(parent)
{
    m_serviceName = serviceName;
    setCaption(i18n("Flickr Account Selector"));
    setButtons(User1|Ok|Close);
    setButtonGuiItem(User1, KGuiItem(i18n("Add another account"), KIcon("network-workgroup")));
    setDefaultButton(Close);
    setModal(true);
    
    if (serviceName == QString("23"))
    {
        setWindowIcon(KIcon("kipi-hq"));
    }
    else if (serviceName == QString("Zooomr"))
    {
        setWindowIcon(KIcon("kipi-zooomr"));
    }
    else
    {
        setWindowIcon(KIcon("kipi-flickr"));
    }
    
    m_uname = QString();
    
    label = new QLabel(this);
    label->setText("Choose the "+serviceName+" account to use for exporting images: ");
    
    userComboBox = new KComboBox(this);
    
    setMainWidget(userComboBox);
    resize(300, 300);
    
}

SelectUserDlg::~SelectUserDlg()
{
    delete userComboBox;
    delete label;
}

void SelectUserDlg::reactivate()
{
    QString uname;
    KConfig config("kipirc");
    foreach ( const QString& group, config.groupList() ) 
    {
        if(!(group.contains(m_serviceName)))
	    continue;
        KConfigGroup grp = config.group(group);
	if(QString::compare(grp.readEntry("username"), QString(), Qt::CaseInsensitive)==0)
	    continue;
	userComboBox->addItem(grp.readEntry("username"));
    }
    exec();
}

void SelectUserDlg::slotButtonClicked(int button) 
{
    kDebug()<<"Button Clicked is "<<button;
    if(button == KDialog::Ok)
    {
        m_uname = userComboBox->currentText();
        accept();
    }
    else if(button == KDialog::User1)
    {
        m_uname = QString();
        KDialog::slotButtonClicked(KDialog::Close);
    }else
        KDialog::slotButtonClicked(button);
    
    userComboBox->clear();
}

QString SelectUserDlg::getUname()
{
    return m_uname;
}

SelectUserDlg* SelectUserDlg::getDlg()
{
    return this;
}

}