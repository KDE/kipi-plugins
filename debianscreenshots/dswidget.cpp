/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#include "dswidget.h"
#include "dswidget.moc"

// Qt includes

#include <QApplication>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QProgressBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QStandardItemModel>
#include <QCompleter>

// KDE includes

#include <KLocale>
#include <KDialog>
#include <KComboBox>
#include <KPushButton>
#include <KLineEdit>
#include <KCompletion>
#include <KCompletionBox>
#include <kdebug.h>
#include <KIO/AccessManager>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// QJSON includes

#include <qjson/parser.h>

// Local includes

#include "imageslist.h"
#include "packagedelegate.h"
#include "debshots.h"
#include "kclickableimagelabel.h"

namespace KIPIDebianScreenshotsPlugin
{

DsWidget::DsWidget(QWidget* parent, KIPI::Interface* iface)
    : QWidget(parent),
      m_lastTip( QString() ),
      m_lastQueryUrl( QUrl() ),
      m_httpManager( new KIO::AccessManager(this) ),
      m_jsonManager( new KIO::AccessManager(this) )
{
    setObjectName("DsWidget");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // -------------------------------------------------------------------

    m_imgList  = new KIPIPlugins::ImagesList(iface, this);
    m_imgList->setControlButtonsPlacement(KIPIPlugins::ImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis( i18n("This is the list of images to upload to Debian Screenshots.") );

    QWidget* settingsBox = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

//    m_headerLabel = new QLabel(settingsBox);
//    m_headerLabel->setText(QString("<b><h2><a href='%1'>"
//                                 "<font color=\"#BF1238\">Debian Screenshots</font>"
//                                 "</a></h2></b>").arg(KIPIDebianScreenshotsPlugin::debshotsUrl));
    m_headerLabel = new KClickableImageLabel(settingsBox);
    QPixmap sdnLogoPixmap(":/kipi_debianscreenshots/sdnlogo.png");
    m_headerLabel->setPixmap(sdnLogoPixmap);
    m_headerLabel->setUrl(KIPIDebianScreenshotsPlugin::debshotsUrl);
    m_headerLabel->setWhatsThis( i18n("This is a clickable link to open the Debian Screenshots home page in a web browser.") );
    m_headerLabel->setOpenExternalLinks(true);
    m_headerLabel->setFocusPolicy(Qt::NoFocus);

    QGroupBox* pkgGroupBox   = new QGroupBox(settingsBox);
    pkgGroupBox->setTitle(i18n("Package"));
    pkgGroupBox->setWhatsThis(i18n("This is the Debian Screenshots package to which selected photos will be uploaded."));

    QGridLayout* sdnLayout   = new QGridLayout(pkgGroupBox);

    QLabel* pkgLabel         = new QLabel(i18n("Package:"), pkgGroupBox);

    m_pkgLineEdit            = new KLineEdit(pkgGroupBox);
    QCompleter* pkgCompleter = new QCompleter(this);
    pkgCompleter->setCompletionMode(QCompleter::PopupCompletion);
    pkgCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_pkgLineEdit->setCompleter(pkgCompleter);

    QListView* listView = new QListView;
    pkgCompleter->setPopup(listView);
    listView->setItemDelegateForColumn(0, new PackageDelegate);

    connect(m_pkgLineEdit, SIGNAL(textEdited(QString)), 
            this, SLOT(slotCompletePackageName(QString)));

    connect(m_httpManager, SIGNAL(finished(QNetworkReply*)), 
            this, SLOT(slotCompletePackageNameFinished(QNetworkReply*)));

    connect(pkgCompleter, SIGNAL(activated(QString)),
            this, SLOT(slotFindVersionsForPackage(QString)));

    connect(m_jsonManager, SIGNAL(finished(QNetworkReply*)), 
            this, SLOT(slotFindVersionsForPackageFinished(QNetworkReply*)));

    QLabel* versionLabel = new QLabel(i18n("Software version:"), pkgGroupBox);
    m_versionsComboBox   = new KComboBox(pkgGroupBox);
    m_versionsComboBox->setEditable(false);
    m_versionsComboBox->setEnabled(false); // Disable until we have a package name
    m_versionsComboBox->setMinimumContentsLength(40);
    connect(m_versionsComboBox, SIGNAL(activated(int)),
            this, SLOT(slotEnableUpload()));

    QLabel* descriptionLabel  = new QLabel(i18n("Screenshot description:"), pkgGroupBox);
    m_descriptionLineEdit = new KLineEdit(pkgGroupBox);
    m_descriptionLineEdit->setMaxLength(40); // 40 is taken from screenshots.debian.net/upload page source
    m_descriptionLineEdit->setEnabled(false);

    sdnLayout->addWidget(pkgLabel,              1, 0, 1, 1);
    sdnLayout->addWidget(m_pkgLineEdit,         1, 1, 1, 4);
    sdnLayout->addWidget(versionLabel,          2, 0, 1, 1);
    sdnLayout->addWidget(m_versionsComboBox,    2, 1, 1, 4);
    sdnLayout->addWidget(descriptionLabel,      3, 0, 1, 1);
    sdnLayout->addWidget(m_descriptionLineEdit, 3, 1, 1, 4);

    m_progressBar = new QProgressBar(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    settingsBoxLayout->addWidget(m_headerLabel);
    settingsBoxLayout->addWidget(pkgGroupBox);
    settingsBoxLayout->addWidget(m_progressBar);

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);
}

DsWidget::~DsWidget()
{
}

KIPIPlugins::ImagesList* DsWidget::imagesList() const
{
    return m_imgList;
}

QProgressBar* DsWidget::progressBar() const
{
    return m_progressBar;
}

QString DsWidget::getDestinationPath() const
{
    return m_uploadWidget->selectedImageCollection().uploadPath().toLocalFile();
}

void DsWidget::slotCompletePackageName(const QString& tip)
{
    if((!tip.isEmpty()) && (QString::compare(tip, m_lastTip, Qt::CaseInsensitive) != 0) )
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        m_versionsComboBox->clear();
        m_versionsComboBox->setEnabled(false);
        m_descriptionLineEdit->setEnabled(false);
        emit requiredPackageInfoAvailable(false);

        QUrl sdnUrl(KIPIDebianScreenshotsPlugin::debshotsUrl + "/packages/ajax_autocomplete_packages"); // DOES NOT RETURN JSON
        sdnUrl.addQueryItem("q", tip );
        sdnUrl.addQueryItem("limit", "30"); // No matter what 'limit' we use, s.d.n will always return 30 results

        QNetworkRequest request(sdnUrl);
        m_httpManager->get(request);
        m_lastQueryUrl = sdnUrl;
    }

    m_lastTip = tip;
}

void DsWidget::slotCompletePackageNameFinished(QNetworkReply* reply)
{
    QUrl replyUrl = reply->url();

    QApplication::restoreOverrideCursor();

    // Check if this is the reply for the last request, or a delayed reply we are receiving just now
    if( QString::compare(replyUrl.toString(), m_lastQueryUrl.toString(), Qt::CaseInsensitive) != 0 )
    {
        kDebug() << "Received a delayed reply, discarding it";
        return; // It was a delayed reply, discard it
    }

    if ( reply->error() )
    {
        kDebug() << "Didn't receive a reply for request " << replyUrl.toEncoded().constData() << " - " <<  qPrintable(reply->errorString());
    }
    else
    {
        QByteArray ba = reply->readAll();

        if( ba.isEmpty() )
        {
            kDebug() << "No completion data received for request " << replyUrl.toEncoded().constData() << "(probably no package matches that pattern)";
            return;
        }

        QList<QByteArray> pkgSuggestions = ba.split('\n');

        QStandardItemModel *m = new QStandardItemModel(pkgSuggestions.count(), 2, m_pkgLineEdit->completer());

        for( int i = 0; i < pkgSuggestions.count(); ++i)
        {
            QModelIndex pkgIdx = m->index(i, 0);
            QModelIndex descIdx = m->index(i, 1);
            QList<QByteArray> pkgDescSplit = pkgSuggestions.at(i).split('|');
            QString pkg = pkgDescSplit.at(0);
            QString desc =pkgDescSplit.at(1);
            m->setData(pkgIdx, pkg);
            m->setData(descIdx, desc);
        }
        m_pkgLineEdit->completer()->setModel(m);
    }

    m_pkgLineEdit->completer()->complete();

    reply->deleteLater();
}

void DsWidget::slotFindVersionsForPackage(const QString& package)
{
    QUrl sdnVersionUrl(KIPIDebianScreenshotsPlugin::debshotsUrl + "/packages/ajax_get_version_for_package"); // DOES RETURN JSON
    sdnVersionUrl.addEncodedQueryItem(QByteArray("q"), QUrl::toPercentEncoding(package));
    sdnVersionUrl.addQueryItem("limit", "30");
    QNetworkRequest request(sdnVersionUrl);
    m_jsonManager->get(request);
}

void DsWidget::slotFindVersionsForPackageFinished(QNetworkReply* reply)
{
    QUrl replyUrl = reply->url();

    if (reply->error())
    {
        kWarning() << "Download of " << replyUrl.toEncoded().constData() << "failed: " <<  qPrintable(reply->errorString());
    }
    else
    {
        QByteArray ba = reply->readAll();

        bool ok;
        QJson::Parser jsonParser;
        QVariant versionSuggestions = jsonParser.parse(ba, &ok);

        if (ok)
        {
            kDebug() << "Query " << replyUrl.toEncoded().constData() << "succeeded";

            QMap<QString, QVariant> versions = versionSuggestions.toMap();

            QMap<QString, QVariant>::const_iterator i = versions.constBegin();
            while (i != versions.constEnd())
            {
                m_versionsComboBox->addItem(i.value().toString());
                ++i;
            }
            m_versionsComboBox->setEnabled(true);

            if( versions.size() == 1 )
            {
                m_descriptionLineEdit->setEnabled(true);
                slotEnableUpload();
            }

        }
        else
        {
            kDebug() << "Query " << replyUrl.toEncoded().constData() << "failed";
        }
    }
    reply->deleteLater();
}

void DsWidget::slotEnableUpload()
{
    if(!m_imgList->imageUrls().isEmpty())
    {
        emit requiredPackageInfoAvailable(true);
    }
}

} // namespace KIPIDebianScreenshotsPlugin
