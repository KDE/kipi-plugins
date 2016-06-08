/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#include "imgurwidget.h"

// Qt includes

#include <QBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QStyle>
#include <QPointer>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"
#include "plugin_imgur.h"

namespace KIPIImgurPlugin
{

class ImgurWidget::Private
{
public:

    Private()
    {
        headerLbl      = 0;
        imagesList     = 0;
        progressBar    = 0;
        processedCount = 0;
        iface          = 0;

        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }

#ifdef OAUTH_ENABLED
        changeUserBtn  = 0;
#endif //OAUTH_ENABLED
    }

    QLabel*           headerLbl;
    ImgurImagesList*  imagesList;
    KPProgressWidget* progressBar;
    int               processedCount;
    Interface*        iface;

#ifdef OAUTH_ENABLED
    QPushButton*      changeUserBtn;
#endif //OAUTH_ENABLED
};

ImgurWidget::ImgurWidget(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QGridLayout* const mainLayout = new QGridLayout(this);
    d->imagesList                 = new ImgurImagesList(this);
    d->imagesList->loadImagesFromCurrentSelection();

    QWidget* const settingsBox           = new QWidget(this);
    QVBoxLayout* const settingsBoxLayout = new QVBoxLayout(settingsBox);

    d->headerLbl = new QLabel(settingsBox);
    d->headerLbl->setWhatsThis(i18n("This is a clickable link to open the Imgur home page in a web browser"));
    d->headerLbl->setText(QString::fromLatin1("<h2><a href='http://imgur.com'>imgur.com</a></h2>"));
    d->headerLbl->setOpenExternalLinks(true);
    d->headerLbl->setFocusPolicy(Qt::NoFocus);

/*
    d->textLbl = new QLabel(settingsBox);
    d->textLbl->setText(i18n("You can retrieve the\nimage URLs from the Xmp tags:\n"
                             "\"Imgur URL\" and \"Imgur Delete URL\". \n"));
    d->textLbl->setFocusPolicy(Qt::NoFocus);
    d->textLbl->setWordWrap(true);
*/

    d->progressBar = new KPProgressWidget(settingsBox);
    d->progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    d->progressBar->setVisible(false);

    // ------------------------------------------------------------------------

#ifdef OAUTH_ENABLED
    //d->dlg = new KPLoginDialog (this, Plugin_ImgurExport::name(), "Imgur authentication");

    QGroupBox* const accountBox         = new QGroupBox(i18n("Imgur authorization"), settingsBox);
    accountBox->setWhatsThis(i18n("Requesting Imgur for authorization to upload images with the KIPI Imgur Exporter plugin."));
    QGridLayout* const accountBoxLayout = new QGridLayout(accountBox);

    //QLabel* userNameLbl             = new QLabel(i18nc("imgur account settings", "Name:"), accountBox);
    //QLabel* userNameDisplayLbl    = new QLabel(d->loggedUser, accountBox);

    d->changeUserBtn = new QPushButton(i18n("Request authorization"), accountBox);
    d->changeUserBtn->setIcon(QIcon::fromTheme(QString::fromLatin1("system-switch-user")));
    d->changeUserBtn->setToolTip(i18n("Set permissions for the current application to upload images to Imgur."));

    //accountBoxLayout->addWidget(userNameLbl,            0, 0, 1, 2);
    //accountBoxLayout->addWidget(userNameDisplayLbl,     0, 2, 1, 2);
    accountBoxLayout->addWidget(d->changeUserBtn,        2, 0, 1, 2);

    accountBoxLayout->setContentsMargins(spacing, spacing, spacing, spacing);
    accountBoxLayout->setSpacing(spacing);
#endif //OAUTH_ENABLED

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(d->headerLbl);
    //settingsBoxLayout->addWidget(d->textLbl);

#ifdef OAUTH_ENABLED
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->setAlignment(accountBox, Qt::AlignTop);
#endif //OAUTH_ENABLED

    settingsBoxLayout->addWidget(d->progressBar);
    settingsBoxLayout->addStretch(10);
    //settingsBoxLayout->setAlignment(d->textLbl, Qt::AlignTop);
    settingsBoxLayout->setAlignment(d->progressBar, Qt::AlignBottom);
    settingsBoxLayout->setContentsMargins(spacing, spacing, spacing, spacing);
    settingsBoxLayout->setSpacing(spacing);

    // -------------------------------------------------------------------------

    mainLayout->addWidget(d->imagesList, 0, 0, 2, 1);
    mainLayout->addWidget(settingsBox, 0, 1, Qt::AlignTop);
    mainLayout->setContentsMargins(QMargins());
    mainLayout->setSpacing(spacing);

    connect(d->imagesList, SIGNAL(signalAddItems(QList<QUrl>)),
            this, SLOT(slotAddItems(QList<QUrl>)));

    connect(d->imagesList, SIGNAL(signalRemovedItems(QList<QUrl>)),
            this, SLOT(slotRemoveItems(QList<QUrl>)));

    connect(d->imagesList, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(this, SIGNAL(signalImageUploadSuccess(QUrl,ImgurSuccess)),
            d->imagesList, SLOT(slotUploadSuccess(QUrl,ImgurSuccess)));

    connect(this, SIGNAL(signalImageUploadError(QUrl,ImgurError)),
            d->imagesList, SLOT(slotUploadError(QUrl,ImgurError)));
/*
    connect(this, SIGNAL(signalImageUploadStart(QUrl)),
            d->imagesList, SLOT(processing(QUrl)));
*/
#ifdef OAUTH_ENABLED
    connect(d->changeUserBtn, SIGNAL(clicked()),
            this, SLOT(slotClickedChangeUser()));
/*
   connect(this, SIGNAL(signalEnableAuthentication(bool)),
           d->changeUserBtn , SLOT(setEnabled(bool)));
   connect(d->dlg, SIGNAL(accepted()),
           this, SLOT(slotCredentialsSet()));
   connect(d->dlg, SIGNAL(signalLoadedNamePass(QString,QString)),
           this, SLOT(slotLoadedNamePass(QString,QString)));
*/
#endif //OAUTH_ENABLED
}

ImgurWidget::~ImgurWidget()
{
    delete d;
}

void ImgurWidget::slotAddItems(const QList<QUrl>& list)
{
    emit signalAddItems(list);
}

void ImgurWidget::slotRemoveItems(const QList<QUrl>& list)
{
    emit signalRemoveItems(list);
}

void ImgurWidget::slotImageListChanged()
{
    emit signalImageListChanged();

    progressBar()->setMaximum(imagesList()->imageUrls().size());
}

void ImgurWidget::slotImageUploadStart(const QUrl& imgPath)
{
    d->processedCount++;
    qCDebug(KIPIPLUGINS_LOG) << "Processing" << imgPath;
    //imagesList()->clearProcessedStatus();

    imagesList()->processing(imgPath);

    if (!progressBar()->isVisible())
    {
        progressBar()->setVisible(true);
    }

    progressBar()->progressStatusChanged(i18n("Processing %1", imgPath.fileName()));
}

void ImgurWidget::slotImageUploadSuccess(const QUrl& imgPath, const ImgurSuccess& success)
{
    // we add tags to the image

    if (d->iface)
    {
        QPointer<MetadataProcessor> meta = d->iface->createMetadataProcessor();

        if (meta && meta->load(imgPath))
        {
            meta->setXmpTagString(QLatin1String("Xmp.kipi.ImgurId"),         success.image.id);
            meta->setXmpTagString(QLatin1String("Xmp.kipi.ImgurDeleteHash"), success.image.deletehash);
            bool saved = meta->applyChanges();
            qCDebug(KIPIPLUGINS_LOG) << "Metadata" << (saved ? "Saved" : "Not Saved") << "to" << imgPath;
        }
    }

    qCDebug(KIPIPLUGINS_LOG) << "URL" << ImgurConnection::pageURL(success.image.id);
    qCDebug(KIPIPLUGINS_LOG) << "Delete URL" << ImgurConnection::deleteURL(success.image.deletehash);

    imagesList()->processed(imgPath, true);

    progressBar()->setValue(d->processedCount);

    emit signalImageUploadSuccess(imgPath, success);
}

void ImgurWidget::slotImageUploadError(const QUrl& imgPath, const ImgurError& error)
{
    imagesList()->processed(imgPath, false);
    emit signalImageUploadError(imgPath, error);
}

ImgurImagesList* ImgurWidget::imagesList() const
{
    return d->imagesList;
}

KPProgressWidget* ImgurWidget::progressBar() const
{
    return d->progressBar;
}

void ImgurWidget::slotAuthenticated(bool authenticated, const QString& message)
{
    Q_UNUSED(message);

#ifdef OAUTH_ENABLED
    //qCDebug(KIPIPLUGINS_LOG) << "Disable the button.";
    if (authenticated)
    {
        d->changeUserBtn->setText(i18n("Authenticated"));
    }

    d->changeUserBtn->setEnabled(!authenticated);
    //emit signalEnableAuthentication(!authenticated);
#else //OAUTH_ENABLED
    Q_UNUSED(authenticated);
#endif //OAUTH_ENABLED
}

void ImgurWidget::slotClickedChangeUser()
{
    emit signalClickedChangeUser();
}
/*
void ImgurWidget::slotChangeUserDialog()
{
    //d->dlg->show();
}

void ImgurWidget::slotSetLoggedUser(bool ok)
{
    emit signalLoggedUserChanged(d->dlg->username());
}

void ImgurWidget::slotCredentialsSet()
{
//    emit signalCredentialsChanged(d->dlg->username(), d->dlg->password());
}

void ImgurWidget::slotLoadedNamePass(const QString& name, const QString& pass)
{
    emit signalCredentialsChanged(name, pass);
}
*/

} // namespace KIPIImgurPlugin
