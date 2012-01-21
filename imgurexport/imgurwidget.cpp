/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
 * Copyright (C) 2010 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QProgressBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>

namespace KIPIImgurExportPlugin
{

ImgurWidget::ImgurWidget(Interface* iface, bool import)
{
    setObjectName("ImgurWidget");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);


    // ------------------------------------------------------------------------

    QGroupBox* uploadBox    = new QGroupBox(i18n("Destination"), settingsBox);
    uploadBox->setWhatsThis(
        i18n("This is the location where Imgur images will be downloaded."));
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget = iface->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

    m_progressBar = new QProgressBar(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    // ------------------------------------------------------------------------


    settingsBoxLayout->addWidget(uploadBox);
    settingsBoxLayout->addWidget(m_progressBar);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

//    //mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

//    updateLabels();  // use empty labels until login

//    // ------------------------------------------------------------------------

////    connect(m_changeUserBtn, SIGNAL(clicked()),
////            this, SLOT(slotChangeUserClicked()));

//    connect(m_resizeChB, SIGNAL(clicked()),
//            this, SLOT(slotResizeChecked()));

}

ImgurWidget::~ImgurWidget()
{
}

//ImagesList* ImgurWidget::imagesList() const
//{
//    return m_imgList;
//}

QProgressBar* ImgurWidget::progressBar() const
{
    return m_progressBar;
}

void ImgurWidget::slotChangeUserClicked()
{
//    emit signalUserChangeRequest(false);
}

void ImgurWidget::slotResizeChecked()
{
//    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
//    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

} // namespace KIPIImgurExportPlugin
