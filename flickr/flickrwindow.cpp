/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
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

#include "flickrwindow.h"

// Qt includes

#include <QPushButton>
#include <QProgressDialog>
#include <QPixmap>
#include <QCheckBox>
#include <QStringList>
#include <QSpinBox>
#include <QPointer>
#include <QDebug>
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QWindow>

// KDE includes

#include <kconfig.h>
#include <kwindowconfig.h>

// LibKIPI includes

#include <KIPI/Interface>

// Local includes

#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpprogresswidget.h"
#include "login.h"
#include "flickrtalker.h"
#include "flickritem.h"
#include "flickrlist.h"
#include "flickrwidget.h"
#include "selectuserdlg.h"
#include "kipiplugins_debug.h"
#include "newalbum.h"

namespace KIPIFlickrPlugin
{

FlickrWindow::FlickrWindow(QWidget* const /*parent*/, const QString& serviceName, SelectUserDlg* const dlg)
    : KPToolDialog(0)
{
    m_serviceName = serviceName;
    setWindowTitle(i18n("Export to %1 Web Service", m_serviceName));
    setModal(false);

    if (serviceName == QLatin1String("23"))
    {
        setWindowIcon(QIcon::fromTheme(QString::fromLatin1("kipi-hq")));
    }
    else if (serviceName == QLatin1String("Zooomr"))
    {
        setWindowIcon(QIcon::fromTheme(QString::fromLatin1("kipi-zooomr")));
    }
    else
    {
        setWindowIcon(QIcon::fromTheme(QString::fromLatin1("kipi-flickr")));
    }

    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup grp = config.group(QString::fromLatin1("%1Export Settings").arg(m_serviceName));

    if(grp.exists())
    {
        qCDebug(KIPIPLUGINS_LOG) << QString::fromLatin1("%1Export Settings").arg(m_serviceName) << " exists, deleting it";
        grp.deleteGroup();
    }

    m_select                    = dlg;
    m_uploadCount               = 0;
    m_uploadTotal               = 0;
    m_widget                    = new FlickrWidget(this, iface(), serviceName);
    m_albumDlg                  = new NewAlbum(this,QString::fromLatin1("Flickr"));
    m_albumsListComboBox        = m_widget->getAlbumsCoB();
    m_newAlbumBtn               = m_widget->getNewAlbmBtn();
    //m_sendOriginalCheckBox      = m_widget->m_sendOriginalCheckBox;
    m_resizeCheckBox            = m_widget->getResizeCheckBox();
    m_publicCheckBox            = m_widget->m_publicCheckBox;
    m_familyCheckBox            = m_widget->m_familyCheckBox;
    m_friendsCheckBox           = m_widget->m_friendsCheckBox;
    m_dimensionSpinBox          = m_widget->getDimensionSpB();
    m_imageQualitySpinBox       = m_widget->getImgQualitySpB();
    m_extendedTagsButton        = m_widget->m_extendedTagsButton;
    m_addExtraTagsCheckBox      = m_widget->m_addExtraTagsCheckBox;
    m_extendedPublicationButton = m_widget->m_extendedPublicationButton;
    m_safetyLevelComboBox       = m_widget->m_safetyLevelComboBox;
    m_contentTypeComboBox       = m_widget->m_contentTypeComboBox;
    m_tagsLineEdit              = m_widget->m_tagsLineEdit;
    m_exportHostTagsCheckBox    = m_widget->m_exportHostTagsCheckBox;
    m_stripSpaceTagsCheckBox    = m_widget->m_stripSpaceTagsCheckBox;
    m_changeUserButton          = m_widget->getChangeUserBtn();
    m_removeAccount             = m_widget->m_removeAccount;
    m_userNameDisplayLabel      = m_widget->getUserNameLabel();
    m_imglst                    = m_widget->m_imglst;

    startButton()->setText(i18n("Start Uploading"));
    startButton()->setToolTip(QString());

    setMainWidget(m_widget);
    m_widget->setMinimumSize(800, 600);

    connect(m_imglst, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    //m_startUploadButton->setEnabled(false);

    // --------------------------------------------------------------------------
    // About data and help button.

    KPAboutData* const about = new KPAboutData(ki18n("Flickr/23/Zooomr Export"),
                                               ki18n("A tool to export an image collection to a "
                                                     "Flickr / 23 / Zooomr web service."),
                                               ki18n("(c) 2005-2008, Vardhman Jain\n"
                                                     "(c) 2008-2015, Gilles Caulier\n"
                                                     "(c) 2009, Luka Renko\n"
                                                     "(c) 2015, Shourya Singh Gupta"));

    about->addAuthor(i18n("Vardhman Jain"), i18n("Author and maintainer"),
                     QString::fromLatin1("Vardhman at gmail dot com"));

    about->addAuthor(i18n("Gilles Caulier"), i18n("Developer"),
                     QString::fromLatin1("caulier dot gilles at gmail dot com"));

    about->addAuthor(i18n("Shourya Singh Gupta"), i18n("Developer"),
                     QString::fromLatin1("shouryasgupta at gmail dot com"));

    about->setHandbookEntry(QString::fromLatin1("tool-flickrexport"));
    setAboutData(about);

    // --------------------------------------------------------------------------

    m_talker = new FlickrTalker(this, serviceName);

    connect(m_talker, SIGNAL(signalError(QString)),
            m_talker, SLOT(slotError(QString)));

    connect(m_talker, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(m_talker, SIGNAL(signalAddPhotoSucceeded()),
            this, SLOT(slotAddPhotoSucceeded()));

    connect(m_talker, SIGNAL(signalAddPhotoFailed(QString)),
            this, SLOT(slotAddPhotoFailed(QString)));

    connect(m_talker, SIGNAL(signalAddPhotoSetSucceeded()),
            this, SLOT(slotAddPhotoSetSucceeded()));

    connect(m_talker, SIGNAL(signalListPhotoSetsSucceeded()),
            this, SLOT(slotPopulatePhotoSetComboBox()));

    connect(m_talker, SIGNAL(signalListPhotoSetsFailed(QString)),
            this, SLOT(slotListPhotoSetsFailed(QString)));

    connect(m_talker, SIGNAL(signalTokenObtained(QString)),
            this, SLOT(slotTokenObtained(QString)));

    connect(m_widget->progressBar(), SIGNAL(signalProgressCanceled()),
            this, SLOT(slotAddPhotoCancelAndClose()));

    connect(m_widget->getReloadBtn(), SIGNAL(clicked()),
            this, SLOT(slotReloadPhotoSetRequest()));

    //connect( m_talker, SIGNAL(signalAlbums(QValueList<GAlbum>)),
    //         SLOT(slotAlbums(QValueList<GAlbum>)) );

    //connect( m_talker, SIGNAL(signalPhotos(QValueList<GPhoto>)),
    //         SLOT(slotPhotos(QValueList<GPhoto>)) );

    // --------------------------------------------------------------------------


    connect(m_changeUserButton, SIGNAL(clicked()),
            this, SLOT(slotUserChangeRequest()));

    connect(m_removeAccount, SIGNAL(clicked()),
            this, SLOT(slotRemoveAccount()));

    connect(m_newAlbumBtn, SIGNAL(clicked()),
            this, SLOT(slotCreateNewPhotoSet()));


    //connect( m_tagView, SIGNAL(selectionChanged()),
    //         SLOT(slotTagSelected()) );

    //connect( m_photoView->browserExtension(), SIGNAL(openURLRequest(KUrl,KParts::URLArgs)),
    //         SLOT(slotOpenPhoto(KUrl)) );

    // --------------------------------------------------------------------------

    m_authProgressDlg = new QProgressDialog(this);
    m_authProgressDlg->setModal(true);
    m_authProgressDlg->setAutoReset(true);
    m_authProgressDlg->setAutoClose(true);
    m_authProgressDlg->setMaximum(0);
    m_authProgressDlg->reset();

    connect(m_authProgressDlg, SIGNAL(canceled()),
            this, SLOT(slotAuthCancel()));

    m_talker->m_authProgressDlg = m_authProgressDlg;

    // --------------------------------------------------------------------------

    connect(this, &QDialog::finished,
            this, &FlickrWindow::slotFinished);

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancelClicked()));

    connect(startButton(), &QPushButton::clicked,
            this, &FlickrWindow::slotUser1);
}

FlickrWindow::~FlickrWindow()
{
    delete m_authProgressDlg;
    delete m_talker;
    delete m_widget;
}

void FlickrWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void FlickrWindow::slotFinished()
{
    writeSettings();
    m_imglst->listView()->clear();
}

void FlickrWindow::setUiInProgressState(bool inProgress)
{
    setRejectButtonMode(inProgress ? QDialogButtonBox::Cancel : QDialogButtonBox::Close);

    if (inProgress)
    {
        m_widget->progressBar()->show();
    }
    else
    {
        m_widget->progressBar()->hide();
        m_widget->progressBar()->progressCompleted();
    }
}

void FlickrWindow::slotCancelClicked()
{
    m_talker->cancel();
    m_uploadQueue.clear();
    setUiInProgressState(false);
}

void FlickrWindow::slotAddPhotoCancelAndClose()
{
    writeSettings();
    m_imglst->listView()->clear();
    m_uploadQueue.clear();
    m_widget->progressBar()->reset();
    setUiInProgressState(false);
    m_talker->cancel();
    reject();

    // refresh the thumbnails
    //slotTagSelected();
}

void FlickrWindow::reactivate()
{
    m_userNameDisplayLabel->setText(QString());
    readSettings(m_select->getUname());
    qCDebug(KIPIPLUGINS_LOG) << "Calling auth methods";

    if (m_token.length() < 1)
    {
        m_talker->getFrob();
    }
    else
    {
        m_talker->checkToken(m_token);
    }

    m_widget->m_imglst->loadImagesFromCurrentSelection();
    show();
}

void FlickrWindow::readSettings(QString uname)
{
    KConfig config(QString::fromLatin1("kipirc"));
    qCDebug(KIPIPLUGINS_LOG) << "Group name is : "<<QString::fromLatin1("%1%2Export Settings").arg(m_serviceName,uname);
    KConfigGroup grp = config.group(QString::fromLatin1("%1%2Export Settings").arg(m_serviceName,uname));
    m_token          = grp.readEntry("token");
    qCDebug(KIPIPLUGINS_LOG) << "Token is : "<<m_token;
    m_exportHostTagsCheckBox->setChecked(grp.readEntry("Export Host Tags",      false));
    m_extendedTagsButton->setChecked(grp.readEntry("Show Extended Tag Options", false));
    m_addExtraTagsCheckBox->setChecked(grp.readEntry("Add Extra Tags",          false));
    m_stripSpaceTagsCheckBox->setChecked(grp.readEntry("Strip Space From Tags", false));
    m_stripSpaceTagsCheckBox->setEnabled(m_exportHostTagsCheckBox->isChecked());

    if (!iface()->hasFeature(KIPI::HostSupportsTags))
    {
        m_exportHostTagsCheckBox->setEnabled(false);
        m_stripSpaceTagsCheckBox->setEnabled(false);
    }

    m_publicCheckBox->setChecked(grp.readEntry("Public Sharing",                               false));
    m_familyCheckBox->setChecked(grp.readEntry("Family Sharing",                               false));
    m_friendsCheckBox->setChecked(grp.readEntry("Friends Sharing",                             false));
    m_extendedPublicationButton->setChecked(grp.readEntry("Show Extended Publication Options", false));

    int safetyLevel = m_safetyLevelComboBox->findData(QVariant(grp.readEntry("Safety Level", 0)));

    if (safetyLevel == -1)
    {
        safetyLevel = 0;
    }

    m_safetyLevelComboBox->setCurrentIndex(safetyLevel);

    int contentType = m_contentTypeComboBox->findData(QVariant(grp.readEntry("Content Type", 0)));

    if (contentType == -1)
    {
        contentType = 0;
    }

    m_contentTypeComboBox->setCurrentIndex(contentType);

    if (grp.readEntry("Resize", false))
    {
        m_resizeCheckBox->setChecked(true);
        m_dimensionSpinBox->setEnabled(true);
        m_imageQualitySpinBox->setEnabled(true);
    }
    else
    {
        m_resizeCheckBox->setChecked(false);
        m_dimensionSpinBox->setEnabled(false);
        m_imageQualitySpinBox->setEnabled(false);
    }

    //m_sendOriginalCheckBox->setChecked(grp.readEntry("Send original", false));

    m_dimensionSpinBox->setValue(grp.readEntry("Maximum Width",       1600));
    m_imageQualitySpinBox->setValue(grp.readEntry("Image Quality",    85));

    winId();
    KConfigGroup dialogGroup = config.group(QString::fromLatin1("%1Export Dialog").arg(m_serviceName));
    KWindowConfig::restoreWindowSize(windowHandle(), dialogGroup);
    resize(windowHandle()->size());
}

void FlickrWindow::writeSettings()
{
    KConfig config(QString::fromLatin1("kipirc"));
    qCDebug(KIPIPLUGINS_LOG) << "Group name is : "<<QString::fromLatin1("%1%2Export Settings").arg(m_serviceName,m_username);

    if (QString::compare(QString::fromLatin1("%1Export Settings").arg(m_serviceName),
        QString::fromLatin1("%1%2Export Settings").arg(m_serviceName,m_username), Qt::CaseInsensitive) == 0)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Not writing entry of group " << QString::fromLatin1("%1%2Export Settings").arg(m_serviceName,m_username);
        return;
    }

    KConfigGroup grp = config.group(QString::fromLatin1("%1%2Export Settings").arg(m_serviceName,m_username));
    grp.writeEntry("username",m_username);
    qCDebug(KIPIPLUGINS_LOG) << "Token written of group "<<QString::fromLatin1("%1%2Export Settings").arg(m_serviceName,m_username)<<" is "<<m_token;
    grp.writeEntry("token", m_token);
    grp.writeEntry("Export Host Tags",                  m_exportHostTagsCheckBox->isChecked());
    grp.writeEntry("Show Extended Tag Options",         m_extendedTagsButton->isChecked());
    grp.writeEntry("Add Extra Tags",                    m_addExtraTagsCheckBox->isChecked());
    grp.writeEntry("Strip Space From Tags",             m_stripSpaceTagsCheckBox->isChecked());
    grp.writeEntry("Public Sharing",                    m_publicCheckBox->isChecked());
    grp.writeEntry("Family Sharing",                    m_familyCheckBox->isChecked());
    grp.writeEntry("Friends Sharing",                   m_friendsCheckBox->isChecked());
    grp.writeEntry("Show Extended Publication Options", m_extendedPublicationButton->isChecked());
    int safetyLevel = m_safetyLevelComboBox->itemData(m_safetyLevelComboBox->currentIndex()).toInt();
    grp.writeEntry("Safety Level",                      safetyLevel);
    int contentType = m_contentTypeComboBox->itemData(m_contentTypeComboBox->currentIndex()).toInt();
    grp.writeEntry("Content Type",                      contentType);
    grp.writeEntry("Resize",                            m_resizeCheckBox->isChecked());
    //grp.writeEntry("Send original",                     m_sendOriginalCheckBox->isChecked());
    grp.writeEntry("Maximum Width",                     m_dimensionSpinBox->value());
    grp.writeEntry("Image Quality",                     m_imageQualitySpinBox->value());
    KConfigGroup dialogGroup = config.group(QString::fromLatin1("%1Export Dialog").arg(m_serviceName));
    KWindowConfig::saveWindowSize(windowHandle(), dialogGroup);
    config.sync();
}

void FlickrWindow::slotDoLogin()
{
}

void FlickrWindow::slotTokenObtained(const QString& token)
{
    qCDebug(KIPIPLUGINS_LOG) << "Token Obtained is : "<<token;
    m_username = m_talker->getUserName();
    m_userId   = m_talker->getUserId();
    qCDebug(KIPIPLUGINS_LOG) << "SlotTokenObtained invoked setting user Display name to " << m_username;
    m_userNameDisplayLabel->setText(QString::fromLatin1("<b>%1</b>").arg(m_username));

    KConfig config(QString::fromLatin1("kipirc"));

    foreach ( const QString& group, config.groupList() )
    {
        if(!(group.contains(m_serviceName)))
            continue;

        KConfigGroup grp = config.group(group);

        if(group.contains(m_username))
        {
            readSettings(m_username);
            break;
        }
    }

    m_token    = token;
    writeSettings();

    // Mutable photosets are not supported by Zooomr (Zooomr only has smart
    // folder-type photosets).
    if (m_serviceName != QString::fromLatin1("Zooomr"))
    {
        m_talker->listPhotoSets();
    }
}

void FlickrWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        //      m_newAlbumBtn->setEnabled( false );
        //      m_addPhotoButton->setEnabled( false );
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        //      m_newAlbumBtn->setEnabled( loggedIn );
        //      m_addPhotoButton->setEnabled( loggedIn && m_albumView->selectedItem() );
    }
}

void FlickrWindow::slotError(const QString& msg)
{
    //m_talker->slotError(msg);
    QMessageBox::critical(this, i18n("Error"), msg);
}

void FlickrWindow::slotUserChangeRequest()
{
    writeSettings();
    qCDebug(KIPIPLUGINS_LOG) << "Slot Change User Request ";
    m_select->reactivate();
    readSettings(m_select->getUname());
    if (m_token.length() < 1)
    {
        m_talker->getFrob();
    }
    else
    {
        m_talker->checkToken(m_token);
    }
    //m_talker->getFrob();
    //  m_addPhotoButton->setEnabled(m_selectImagesButton->isChecked());
}

void FlickrWindow::slotRemoveAccount()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup grp = config.group(QString::fromLatin1("%1%2Export Settings").arg(m_serviceName).arg(m_username));

    if(grp.exists())
    {
        qCDebug(KIPIPLUGINS_LOG) << "Removing Account having group"<<QString::fromLatin1("%1%2Export Settings").arg(m_serviceName);
        grp.deleteGroup();
    }

    m_username = QString();
    qCDebug(KIPIPLUGINS_LOG) << "SlotTokenObtained invoked setting user Display name to " << m_username;
    m_userNameDisplayLabel->setText(QString::fromLatin1("<b>%1</b>").arg(m_username));
}

/**
 * Try to guess a sensible set name from the urls given.
 * Currently, it extracs the last path name component, and returns the most
 * frequently seen. The function could be expanded to, for example, only
 * accept the path if it occurs at least 50% of the time. It could also look
 * further up in the path name.
 */
QString FlickrWindow::guessSensibleSetName(const QList<QUrl>& urlList)
{
    QMap<QString,int> nrFolderOccurences;

    // Extract last component of directory
    foreach(const QUrl& url, urlList)
    {
        QString dir      = url.adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).toLocalFile();
        QStringList list = dir.split(QLatin1Char('/'));

        if (list.isEmpty())
            continue;

        nrFolderOccurences[list.last()]++;
    }

    int maxCount   = 0;
    int totalCount = 0;
    QString name;

    for(QMap<QString,int>::const_iterator it=nrFolderOccurences.constBegin();
        it!=nrFolderOccurences.constEnd(); ++it)
    {
        totalCount += it.value();

        if (it.value() > maxCount)
        {
            maxCount = it.value();
            name     = it.key();
        }
    }

    // If there is only one entry or one name appears at least twice, return the suggestion
    if (totalCount == 1 || maxCount > 1)
        return name;

    return QString();
}

/** This method is called when the photo set creation button is pressed. It
 * summons a creation dialog for user input. When that is closed, it
 * creates a new photo set in the local list. The id gets the form of
 * UNDEFINED_ followed by a number, to indicate that it doesn't exist on
 * Flickr yet.
 */
void FlickrWindow::slotCreateNewPhotoSet()
{
    if (m_albumDlg->exec() == QDialog::Accepted)
    {
        FPhotoSet fps;
        m_albumDlg->getFolderProperties(fps);
        qCDebug(KIPIPLUGINS_LOG) << "in slotCreateNewPhotoSet() " << fps.title;

        // Lets find an UNDEFINED_ style id that isn't taken yet.s
        QString id;
        int i                               = 0;
        id                                  = QString::fromLatin1("UNDEFINED_") + QString::number(i);
        QLinkedList<FPhotoSet>::iterator it = m_talker->m_photoSetsList->begin();

        while (it != m_talker->m_photoSetsList->end())
        {
            FPhotoSet fps = *it;

            if (fps.id == id)
            {
                id = QString::fromLatin1("UNDEFINED_") + QString::number(++i);
                it = m_talker->m_photoSetsList->begin();
            }

            ++it;
        }

        fps.id = id;

        qCDebug(KIPIPLUGINS_LOG) << "Created new photoset with temporary id " << id;
        // Append the new photoset to the list.
        m_talker->m_photoSetsList->prepend(fps);
        m_talker->m_selectedPhotoSet = fps;

        // Re-populate the photo sets combo box.
        slotPopulatePhotoSetComboBox();
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "New Photoset creation aborted ";
    }
}

void FlickrWindow::slotAuthCancel()
{
    m_talker->cancel();
    m_authProgressDlg->hide();
}

/*
void FlickrWindow::slotPhotos( const QValueList<GPhoto>& photoList)
{
    // TODO
}

void FlickrWindow::slotTagSelected()
{
    // TODO
}
void FlickrWindow::slotOpenPhoto( const QUrl &url )
{
    new KRun(url);
}
*/

void FlickrWindow::slotPopulatePhotoSetComboBox()
{
    qCDebug(KIPIPLUGINS_LOG) << "slotPopulatePhotoSetComboBox invoked";

    if (m_talker && m_talker->m_photoSetsList)
    {
        QLinkedList <FPhotoSet>* const list = m_talker->m_photoSetsList;
        m_albumsListComboBox->clear();
        m_albumsListComboBox->insertItem(0, i18n("&lt;Photostream Only&gt;"));
        m_albumsListComboBox->insertSeparator(1);
        QLinkedList<FPhotoSet>::iterator it = list->begin();
        int index = 2, curr_index = 0;

        while (it != list->end())
        {
            FPhotoSet photoSet = *it;
            QString name       = photoSet.title;
            // Store the id as user data, because the title is not unique.
            QVariant id        = QVariant(photoSet.id);

            if (id == m_talker->m_selectedPhotoSet.id)
            {
                curr_index = index;
            }

            m_albumsListComboBox->insertItem(index++, name, id);
            ++it;
        }

        m_albumsListComboBox->setCurrentIndex(curr_index);
    }
}

/** This slot is call when 'Start Uploading' button is pressed.
*/
void FlickrWindow::slotUser1()
{
    qCDebug(KIPIPLUGINS_LOG) << "SlotUploadImages invoked";

    //m_widget->m_tab->setCurrentIndex(FlickrWidget::FILELIST);

    if (m_imglst->imageUrls().isEmpty())
    {
        return;
    }

    typedef QPair<QUrl, FPhotoInfo> Pair;

    m_uploadQueue.clear();

    for (int i = 0; i < m_imglst->listView()->topLevelItemCount(); ++i)
    {
        FlickrListViewItem* const lvItem = dynamic_cast<FlickrListViewItem*>(m_imglst->listView()->topLevelItem(i));

        if (lvItem)
        {
            KPImageInfo info(lvItem->url());
            qCDebug(KIPIPLUGINS_LOG) << "Adding images"<<lvItem->url()<<" to the list";
            FPhotoInfo temp;

            temp.title                 = info.title();
            temp.description           = info.description();
            temp.size                  = info.fileSize();
            temp.is_public             = lvItem->isPublic()  ? 1 : 0;
            temp.is_family             = lvItem->isFamily()  ? 1 : 0;
            temp.is_friend             = lvItem->isFriends() ? 1 : 0;
            temp.safety_level          = lvItem->safetyLevel();
            temp.content_type          = lvItem->contentType();
            QStringList tagsFromDialog = m_tagsLineEdit->text().split(QLatin1Char(','), QString::SkipEmptyParts);
            QStringList tagsFromList   = lvItem->extraTags();

            QStringList           allTags;
            QStringList::Iterator itTags;

            // Tags from the dialog
            itTags = tagsFromDialog.begin();

            while (itTags != tagsFromDialog.end())
            {
                allTags.append(*itTags);
                ++itTags;
            }

            // Tags from the database
            if (m_exportHostTagsCheckBox->isChecked())
            {
                QStringList tagsFromDatabase;

                tagsFromDatabase = info.keywords();
                itTags           = tagsFromDatabase.begin();

                while (itTags != tagsFromDatabase.end())
                {
                    allTags.append(*itTags);
                    ++itTags;
                }
            }

            // Tags from the list view.
            itTags = tagsFromList.begin();

            while (itTags != tagsFromList.end())
            {
                allTags.append(*itTags);
                ++itTags;
            }

            // Remove spaces if the user doesn't like them.
            if (m_stripSpaceTagsCheckBox->isChecked())
            {
                for (QStringList::iterator it = allTags.begin();
                    it != allTags.end();
                    ++it)
                {
                    *it = (*it).trimmed().remove(QLatin1Char(' '));
                }
            }

            // Debug the tag list.
            itTags = allTags.begin();

            while (itTags != allTags.end())
            {
                qCDebug(KIPIPLUGINS_LOG) << "Tags list: " << (*itTags);
                ++itTags;
            }

            temp.tags = allTags;
            m_uploadQueue.append(Pair(lvItem->url(), temp));
        }
    }

    m_uploadTotal = m_uploadQueue.count();
    m_uploadCount = 0;
    m_widget->progressBar()->reset();
    slotAddPhotoNext();
    qCDebug(KIPIPLUGINS_LOG) << "SlotUploadImages done";
}

void FlickrWindow::slotAddPhotoNext()
{
    if (m_uploadQueue.isEmpty())
    {
        m_widget->progressBar()->reset();
        setUiInProgressState(false);
        //slotAlbumSelected();
        return;
    }

    typedef QPair<QUrl, FPhotoInfo> Pair;
    Pair pathComments = m_uploadQueue.first();
    FPhotoInfo info   = pathComments.second;

    // Find out the selected photo set.
    if (m_serviceName != QString::fromLatin1("Zooomr"))
    {
        // mutable photosets are not supported by Zooomr (Zooomr only has smart folder-type photosets)
        QString selectedPhotoSetId = m_albumsListComboBox->itemData(m_albumsListComboBox->currentIndex()).toString();

        if (selectedPhotoSetId.isEmpty())
        {
            m_talker->m_selectedPhotoSet = FPhotoSet();
        }
        else
        {
            QLinkedList<FPhotoSet>::iterator it = m_talker->m_photoSetsList->begin();

            while (it != m_talker->m_photoSetsList->end())
            {
                if (it->id == selectedPhotoSetId)
                {
                    m_talker->m_selectedPhotoSet = *it;
                    break;
                }

                ++it;
            }
        }
    }

    qCDebug(KIPIPLUGINS_LOG) << "Max allowed file size is : "<<((m_talker->getMaxAllowedFileSize()).toLongLong())<<"File Size is "<<info.size;

    bool res = m_talker->addPhoto(pathComments.first.toLocalFile(), //the file path
                                  info,
                                  m_resizeCheckBox->isChecked(),
                                  m_dimensionSpinBox->value(),
                                  m_imageQualitySpinBox->value());

    if (!res)
    {
        slotAddPhotoFailed(QString::fromLatin1(""));
        return;
    }

    if (m_widget->progressBar()->isHidden())
    {
        setUiInProgressState(true);
        m_widget->progressBar()->progressScheduled(i18n("Flickr Export"), true, true);
        m_widget->progressBar()->progressThumbnailChanged(QIcon(QLatin1String(":/icons/kipi-icon.svg")).pixmap(22, 22));
    }
}

void FlickrWindow::slotAddPhotoSucceeded()
{
    // Remove photo uploaded from the list
    m_imglst->removeItemByUrl(m_uploadQueue.first().first);
    m_uploadQueue.pop_front();
    m_uploadCount++;
    m_widget->progressBar()->setMaximum(m_uploadTotal);
    m_widget->progressBar()->setValue(m_uploadCount);
    slotAddPhotoNext();
}

void FlickrWindow::slotListPhotoSetsFailed(const QString& msg)
{
    QMessageBox::critical(this, QString::fromLatin1("Error"), i18n("Failed to Fetch Photoset information from %1. %2\n", m_serviceName, msg));
}

void FlickrWindow::slotAddPhotoFailed(const QString& msg)
{
    QMessageBox warn(QMessageBox::Warning,
                     i18n("Warning"),
                     i18n("Failed to upload photo into %1. %2\nDo you want to continue?", m_serviceName, msg),
                     QMessageBox::Yes | QMessageBox::No);

    (warn.button(QMessageBox::Yes))->setText(i18n("Continue"));
    (warn.button(QMessageBox::No))->setText(i18n("Cancel"));

    if (warn.exec() != QMessageBox::Yes)
    {
        m_uploadQueue.clear();
        m_widget->progressBar()->reset();
        setUiInProgressState(false);
        // refresh the thumbnails
        //slotTagSelected();
    }
    else
    {
        m_uploadQueue.pop_front();
        m_uploadTotal--;
        m_widget->progressBar()->setMaximum(m_uploadTotal);
        m_widget->progressBar()->setValue(m_uploadCount);
        slotAddPhotoNext();
    }
}

void FlickrWindow::slotAddPhotoSetSucceeded()
{
    /* Method called when a photo set has been successfully created on Flickr.
     * It functions to restart the normal flow after a photo set has been created
     * on Flickr. */
    slotPopulatePhotoSetComboBox();
    slotAddPhotoSucceeded();
}

void FlickrWindow::slotImageListChanged()
{
    startButton()->setEnabled(!(m_widget->m_imglst->imageUrls().isEmpty()));
}

void FlickrWindow::slotReloadPhotoSetRequest()
{
    m_talker->listPhotoSets();
}

} // namespace KIPIFlickrPlugin
