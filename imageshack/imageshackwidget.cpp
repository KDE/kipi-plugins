/* ============================================================
*
* This file is a part of kipi-plugins project
*
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
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

#include "imageshackwidget.h"

// Qt includes

#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QStringList>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QApplication>
#include <QPushButton>
#include <QMimeDatabase>
#include <QMimeType>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/UploadWidget>
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_debug.h"
#include "kpimageslist.h"
#include "imageshack.h"
#include "kpprogresswidget.h"

namespace KIPIImageshackPlugin
{

ImageshackWidget::ImageshackWidget(QWidget* const parent, Imageshack* const imageshack, KIPI::Interface* const iface, const QString& pluginName)
    : KPSettingsWidget(parent, iface, pluginName),
      m_imageshack(imageshack)
{

    m_imgList            = imagesList();
    m_headerLbl          = getHeaderLbl();
    m_accountNameLbl     = getUserNameLabel();
    m_chgRegCodeBtn      = getChangeUserBtn();
    m_reloadGalleriesBtn = getReloadBtn();
    m_galleriesCob       = getAlbumsCoB();
    m_progressBar        = progressBar();

    connect(m_reloadGalleriesBtn, SIGNAL(clicked()),
            this, SLOT(slotReloadGalleries()));

    QGroupBox* const tagsBox      = new QGroupBox(QString::fromLatin1(""), getSettingsBox());
    QGridLayout* const tagsLayout = new QGridLayout(tagsBox);

    m_privateImagesChb = new QCheckBox(tagsBox);
    m_privateImagesChb->setText(i18n("Make private"));
    m_privateImagesChb->setChecked(false);

    m_tagsFld             = new QLineEdit(tagsBox);
    QLabel* const tagsLbl = new QLabel(i18n("Tags (optional):"), tagsBox);

    m_remBarChb           = new QCheckBox(i18n("Remove information bar on thumbnails"));
    m_remBarChb->setChecked(false);

    tagsLayout->addWidget(m_privateImagesChb, 0, 0);
    tagsLayout->addWidget(tagsLbl,            1, 0);
    tagsLayout->addWidget(m_tagsFld,          1, 1);

    addWidgetToSettingsBox(tagsBox);

    getUploadBox()->hide();
    getSizeBox()->hide();

    updateLabels();
}

ImageshackWidget::~ImageshackWidget()
{
}

void ImageshackWidget::updateLabels(const QString& /*name*/, const QString& /*url*/)
{
    if (m_imageshack->loggedIn())
    {
        m_accountNameLbl->setText(m_imageshack->username());
        //m_accountEmailLbl->setText(m_imageshack->email());
    }
    else
    {
        m_accountNameLbl->clear();
        //m_accountEmailLbl->clear();
    }
}

void ImageshackWidget::slotGetGalleries(const QStringList &gTexts, const QStringList &gNames)
{
    m_galleriesCob->clear();

    m_galleriesCob->addItem(i18nc("@item:inlistbox", "Add to root folder"),
                            QString::fromLatin1("--add-to-root--"));

    m_galleriesCob->addItem(i18nc("@item:inlistbox", "Create new gallery"),
                            QString::fromLatin1("--new-gallery--"));

    // TODO check if the lists have the same size
    for (int i = 0; i < gTexts.size(); ++i)
    {
        qCDebug(KIPIPLUGINS_LOG) << "gTexts is "<<gTexts[i] << " gNames is "<<gNames[i];
        m_galleriesCob->addItem(gTexts[i], gNames[i]);
    }

//     slotEnableNewGalleryLE(m_galleriesCob->currentIndex());
}

void ImageshackWidget::slotReloadGalleries()
{
    Q_EMIT signalReloadGalleries();
}

}  // namespace KIPIImageshackPlugin

#include "moc_imageshackwidget.cpp"
