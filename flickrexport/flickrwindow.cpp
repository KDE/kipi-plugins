/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009 by Luka Renko <lure at kubuntu dot org>
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
#include "flickrwindow.moc"
#include "flickrnewphotosetdialog.h"

// Qt includes

#include <QProgressDialog>
#include <QPushButton>
#include <QPixmap>
#include <QCheckBox>
#include <QStringList>
#include <QSpinBox>

// KDE includes

#include <kcombobox.h>
#include <klineedit.h>
#include <khelpmenu.h>
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
#include <ktoolinvocation.h>

// Local includes

#include "imageslist.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "login.h"
#include "flickrtalker.h"
#include "flickritem.h"
#include "flickrlist.h"
//#include "flickrviewitem.h"
#include "flickrwidget.h"
#include "ui_flickralbumdialog.h"

namespace KIPIFlickrExportPlugin
{

FlickrWindow::FlickrWindow(KIPI::Interface* interface, const QString &tmpFolder, QWidget *parent, const QString& serviceName)
            : KDialog(parent)
{
    m_serviceName = serviceName;
    setWindowTitle(i18n("Export to %1 Web Service", m_serviceName));
    if (serviceName != "23")
        setWindowIcon(KIcon("flickr"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);

    m_tmp                    = tmpFolder;
    m_interface              = interface;
    m_uploadCount            = 0;
    m_uploadTotal            = 0;
//  m_wallet                 = 0;
    m_widget                 = new FlickrWidget(this, interface, serviceName);
    m_photoView              = m_widget->m_photoView;
    m_albumsListComboBox     = m_widget->m_albumsListComboBox;
    m_newAlbumBtn            = m_widget->m_newAlbumBtn;
    m_resizeCheckBox         = m_widget->m_resizeCheckBox;
    m_publicCheckBox         = m_widget->m_publicCheckBox;
    m_familyCheckBox         = m_widget->m_familyCheckBox;
    m_friendsCheckBox        = m_widget->m_friendsCheckBox;
    m_dimensionSpinBox       = m_widget->m_dimensionSpinBox;
    m_imageQualitySpinBox    = m_widget->m_imageQualitySpinBox;
    m_extendedButton         = m_widget->m_extendedButton;
    m_safetyLevelComboBox    = m_widget->m_safetyLevelComboBox;
    m_contentTypeComboBox    = m_widget->m_contentTypeComboBox;
    m_tagsLineEdit           = m_widget->m_tagsLineEdit;
    m_exportHostTagsCheckBox = m_widget->m_exportHostTagsCheckBox;
    m_stripSpaceTagsCheckBox = m_widget->m_stripSpaceTagsCheckBox;
    m_changeUserButton       = m_widget->m_changeUserButton;
    m_userNameDisplayLabel   = m_widget->m_userNameDisplayLabel;
    m_imglst                 = m_widget->m_imglst;

    setButtonGuiItem(User1, KGuiItem(i18n("Start Uploading"), KIcon("network-workgroup")));
    setMainWidget(m_widget);
    m_widget->setMinimumSize(600, 400);

    connect(m_imglst, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    //m_startUploadButton->setEnabled(false);

    // --------------------------------------------------------------------------
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Flickr/23 Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to export image collection to "
                                                     "Flickr / 23 web service."),
                                           ki18n( "(c) 2005-2008, Vardhman Jain\n"
                                           "(c) 2008, Gilles Caulier\n"
                                           "(c) 2009, Luka Renko" ));

    m_about->addAuthor(ki18n( "Vardhman Jain" ), ki18n("Author and maintainer"),
                       "Vardhman at gmail dot com");

    m_about->addAuthor(ki18n( "Gilles Caulier" ), ki18n("Developer"),
                       "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // --------------------------------------------------------------------------

    m_talker = new FlickrTalker(this, serviceName);

    connect(m_talker, SIGNAL( signalError( const QString& ) ),
            m_talker, SLOT( slotError( const QString& ) ));

    connect(m_talker, SIGNAL( signalBusy( bool ) ),
            this, SLOT( slotBusy( bool ) ));

    connect(m_talker, SIGNAL( signalAddPhotoSucceeded() ),
            this, SLOT( slotAddPhotoSucceeded() ));

    connect(m_talker, SIGNAL( signalAddPhotoFailed( const QString& ) ),
            this, SLOT( slotAddPhotoFailed( const QString& ) ));

    connect(m_talker, SIGNAL( signalAddPhotoSetSucceeded() ),
            this, SLOT( slotAddPhotoSetSucceeded() ));

    connect(m_newAlbumBtn, SIGNAL( clicked() ),
            this, SLOT( slotCreateNewPhotoSet() ));

    connect(m_talker, SIGNAL( signalListPhotoSetsSucceeded() ),
            this, SLOT( populatePhotoSetComboBox() ));

    connect(m_talker, SIGNAL( signalListPhotoSetsFailed(const QString&) ),
            this, SLOT( slotListPhotoSetsFailed(const QString&) ));

    //connect( m_talker, SIGNAL( signalAlbums( const QValueList<GAlbum>& ) ),
    //         SLOT( slotAlbums( const QValueList<GAlbum>& ) ) );

    //connect( m_talker, SIGNAL( signalPhotos( const QValueList<GPhoto>& ) ),
    //         SLOT( slotPhotos( const QValueList<GPhoto>& ) ) );

    // --------------------------------------------------------------------------

    m_progressDlg = new QProgressDialog(this);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);

    connect(m_progressDlg, SIGNAL( canceled() ),
            this, SLOT( slotAddPhotoCancel() ));

    connect(m_changeUserButton, SIGNAL( clicked() ),
            this, SLOT( slotUserChangeRequest() ));

    connect(m_talker, SIGNAL( signalTokenObtained(const QString&) ),
            this, SLOT( slotTokenObtained(const QString&) ));

    //connect( m_tagView, SIGNAL( selectionChanged() ),
    //         SLOT( slotTagSelected() ) );

    //connect( m_photoView->browserExtension(), SIGNAL( openURLRequest( const KUrl&, const KParts::URLArgs& ) ),
    //         SLOT( slotOpenPhoto( const KUrl& ) ) );

    //connect( m_newAlbumBtn, SIGNAL( clicked() ),
    //         SLOT( slotNewAlbum() ) );

    // --------------------------------------------------------------------------

    readSettings();

    m_authProgressDlg = new QProgressDialog(this);
    m_authProgressDlg->setModal(true);
    m_authProgressDlg->setAutoReset(true);
    m_authProgressDlg->setAutoClose(true);

    connect(m_authProgressDlg, SIGNAL( canceled() ),
            this, SLOT( slotAuthCancel() ));

    m_talker->m_authProgressDlg = m_authProgressDlg;
    m_widget->setEnabled(false);

    kDebug(51000) << "Calling auth methods" << endl;

    if(m_token.length()< 1)
        m_talker->getFrob();
    else
        m_talker->checkToken(m_token);

    // --------------------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotUser1()));
}

FlickrWindow::~FlickrWindow()
{
//   if (m_wallet)
//      delete m_wallet;

    delete m_progressDlg;
    delete m_authProgressDlg;
    delete m_talker;
    delete m_widget;
    delete m_about;
}

void FlickrWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group(QString("%1Export Settings").arg(m_serviceName));
    m_token = grp.readEntry("token");

    if (grp.readEntry("Resize", false))
    {
        m_resizeCheckBox->setChecked(true);
        m_dimensionSpinBox->setEnabled(true);
    }
    else
    {
        m_resizeCheckBox->setChecked(false);
        m_dimensionSpinBox->setEnabled(false);
    }

    m_dimensionSpinBox->setValue(grp.readEntry("Maximum Width", 1600));
    m_imageQualitySpinBox->setValue(grp.readEntry("Image Quality", 85));
    m_exportHostTagsCheckBox->setChecked(grp.readEntry("Export Host Tags", false));
    m_stripSpaceTagsCheckBox->setChecked(grp.readEntry("Strip Space Host Tags", false));
    m_stripSpaceTagsCheckBox->setEnabled(m_exportHostTagsCheckBox->isChecked());

    if(!m_interface->hasFeature(KIPI::HostSupportsTags))
    {
        m_exportHostTagsCheckBox->setEnabled(false);
        m_stripSpaceTagsCheckBox->setEnabled(false);
    }

    m_publicCheckBox->setChecked(grp.readEntry("Public Sharing", false));
    m_familyCheckBox->setChecked(grp.readEntry("Family Sharing", false));
    m_friendsCheckBox->setChecked(grp.readEntry("Friends Sharing", false));

    m_extendedButton->setChecked(grp.readEntry("Show Extended Options", false));
    int safetyLevel = m_safetyLevelComboBox->findData(
                          QVariant(grp.readEntry("Safety Level", 0)));
    if (safetyLevel == -1) safetyLevel = 0;
    m_safetyLevelComboBox->setCurrentIndex(safetyLevel);
    int contentType = m_contentTypeComboBox->findData(
                          QVariant(grp.readEntry("Content Type", 0)));
    if (contentType == -1) contentType = 0;
    m_contentTypeComboBox->setCurrentIndex(contentType);

    KConfigGroup dialogGroup = config.group(QString("%1Export Dialog").arg(m_serviceName));
    restoreDialogSize(dialogGroup);
}

void FlickrWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group(QString("%1Export Settings").arg(m_serviceName));
    grp.writeEntry("token", m_token);
    grp.writeEntry("Resize", m_resizeCheckBox->isChecked());
    grp.writeEntry("Maximum Width",  m_dimensionSpinBox->value());
    grp.writeEntry("Image Quality",  m_imageQualitySpinBox->value());
    grp.writeEntry("Export Host Tags", m_exportHostTagsCheckBox->isChecked());
    grp.writeEntry("Strip Space Host Tags", m_stripSpaceTagsCheckBox->isChecked());
    grp.writeEntry("Public Sharing", m_publicCheckBox->isChecked());
    grp.writeEntry("Family Sharing", m_familyCheckBox->isChecked());
    grp.writeEntry("Friends Sharing", m_friendsCheckBox->isChecked());
    grp.writeEntry("Show Extended Options", m_extendedButton->isChecked());
    int safetyLevel = m_safetyLevelComboBox->itemData(m_safetyLevelComboBox->currentIndex()).toInt();
    grp.writeEntry("Safety Level", safetyLevel);
    int contentType = m_contentTypeComboBox->itemData(m_contentTypeComboBox->currentIndex()).toInt();
    grp.writeEntry("Content Type", contentType);
    KConfigGroup dialogGroup = config.group(QString("%1Export Dialog").arg(m_serviceName));
    saveDialogSize(dialogGroup );
    config.sync();
}

void FlickrWindow::slotHelp()
{
    KToolInvocation::invokeHelp("flickrexport", "kipi-plugins");
}

void FlickrWindow::slotDoLogin()
{
}

void FlickrWindow::slotClose()
{
    writeSettings();
    done(Close);
}

void FlickrWindow::slotTokenObtained(const QString& token)
{
    m_token    = token;
    m_username = m_talker->getUserName();
    m_userId   = m_talker->getUserId();
    kDebug(51000) << "SlotTokenObtained invoked setting user Display name to " << m_username << endl;
    m_userNameDisplayLabel->setText(QString("<b>%1</b>").arg(m_username));
    m_talker->listPhotoSets();
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
    KMessageBox::error( this, msg );
}

void FlickrWindow::slotUserChangeRequest()
{
    kDebug(51000) << "Slot Change User Request " << endl;
    m_talker->getFrob();
//  m_addPhotoButton->setEnabled(m_selectImagesButton->isChecked());
}

void FlickrWindow::slotCreateNewPhotoSet()
{
   /* This method is called when the photo set creation button is pressed. It
    * summons a creation dialog for user input. When that is closed, it
    * creates a new photo set in the local list. The id gets the form of
    * UNDEFINED_ followed by a number, to indicate that it doesn't exist on
    * Flickr yet. */

   // Call the dialog
   FlickrNewPhotoSetDialog *dlg = new FlickrNewPhotoSetDialog(kapp->activeWindow());
   int resp = dlg->exec();
   if ((resp == QDialog::Accepted) && (dlg->titleEdit->text() != ""))
   {
      // Create a new photoset with title and description from the dialog.
      FPhotoSet fps;
      fps.title       = dlg->titleEdit->text();
      fps.description = dlg->descriptionEdit->toPlainText();

      // Lets find an UNDEFINED_ style id that isn't taken yet.s
      QString id;
      int i = 0;
      id = "UNDEFINED_" + QString::number(i);
      QLinkedList<FPhotoSet>::iterator it = m_talker->m_photoSetsList->begin();
      while(it != m_talker->m_photoSetsList->end()) {
        FPhotoSet fps = *it;
        if (fps.id == id) {
          id = "UNDEFINED_" + QString::number(++i);
          it = m_talker->m_photoSetsList->begin();
        }
        it++;
      }
      fps.id = id;

      kDebug() << "Created new photoset with temporary id " << id << endl;
      // Append the new photoset to the list.
      m_talker->m_photoSetsList->prepend(fps);
      m_talker->m_selectedPhotoSet = fps;

      // Repopulate the photo sets combo box.
      populatePhotoSetComboBox();
   }
   else
   {
     kDebug(51000) << "New Photoset creation aborted " << endl;
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
void FlickrWindow::slotOpenPhoto( const KUrl& url )
{
    new KRun(url);
}
*/

void FlickrWindow::populatePhotoSetComboBox()
{
    kDebug(51000) << "populatePhotoSetComboBox invoked" << endl;
    if (m_talker && m_talker->m_photoSetsList)
    {
        QLinkedList <FPhotoSet> *list = m_talker->m_photoSetsList;
        m_albumsListComboBox->clear();
        m_albumsListComboBox->insertItem(0, i18n("<Photostream Only>"));
        m_albumsListComboBox->insertSeparator(1);
        QLinkedList<FPhotoSet>::iterator it = list->begin();
        int index = 2, curr_index = 0;
        while(it != list->end())
        {
            FPhotoSet photoSet=*it;
            QString name = photoSet.title;
            // Store the id as user data, because the title is not unique.
            QVariant id = QVariant(photoSet.id);
            if (id == m_talker->m_selectedPhotoSet.id)
                curr_index = index;
            m_albumsListComboBox->insertItem(index++, name, id);
            it++;
        }
        m_albumsListComboBox->setCurrentIndex(curr_index);
    }
    m_widget->setEnabled(true);
}


/** This slot is call when 'Start Uploading' button is pressed.
*/
void FlickrWindow::slotUser1()
{
    kDebug(51000) << "SlotUploadImages invoked" << endl;

    m_widget->m_tab->setCurrentIndex(FlickrWidget::FILELIST);

    if (m_imglst->imageUrls().isEmpty())
        return;

    typedef QPair<KUrl, FPhotoInfo> Pair;

    m_uploadQueue.clear();

    for (int i = 0; i < m_imglst->listView()->topLevelItemCount(); ++i)
    {
        FlickrListViewItem *lvItem = dynamic_cast<FlickrListViewItem*>
                                     (m_imglst->listView()->topLevelItem(i));

        KIPI::ImageInfo info = m_interface->info(lvItem->url());
        kDebug(51000) << "Adding images to the list" << endl;
        FPhotoInfo temp;

        temp.title                 = info.title();
        temp.description           = info.description();
        temp.is_public             = lvItem->isPublic()  ? 1 : 0;
        temp.is_family             = lvItem->isFamily()  ? 1 : 0;
        temp.is_friend             = lvItem->isFriends() ? 1 : 0;
        temp.safety_level          = lvItem->safetyLevel();
        temp.content_type          = lvItem->contentType();
        QStringList tagsFromDialog = m_tagsLineEdit->text().split(" ", QString::SkipEmptyParts);

        QStringList           allTags;
        QStringList::Iterator itTags;

        // Tags from the dialog
        itTags = tagsFromDialog.begin();
        while(itTags != tagsFromDialog.end())
        {
            allTags.append(*itTags);
            ++itTags;
        }

        // Tags from the database
        QMap <QString, QVariant> attribs = info.attributes();
        QStringList tagsFromDatabase;

        if(m_exportHostTagsCheckBox->isChecked())
        {
            tagsFromDatabase = attribs["tags"].toStringList();
            if (m_stripSpaceTagsCheckBox->isChecked())
            {
                for (QStringList::iterator it = tagsFromDatabase.begin(); it != tagsFromDatabase.end() ; ++it)
                    *it = (*it).trimmed().remove(" ");
            }
        }

        itTags = tagsFromDatabase.begin();

        while(itTags != tagsFromDatabase.end())
        {
            allTags.append(*itTags);
            ++itTags;
        }

        itTags = allTags.begin();

        while(itTags != allTags.end())
        {
            kDebug(51000) << "Tags list: " << (*itTags) << endl;
            ++itTags;
        }

        temp.tags = allTags;
        m_uploadQueue.append(Pair(lvItem->url(), temp));
    }

    m_uploadTotal = m_uploadQueue.count();
    m_uploadCount = 0;
    m_progressDlg->reset();
    slotAddPhotoNext();
    kDebug(51000) << "SlotUploadImages done" << endl;
}

void FlickrWindow::slotAddPhotoNext()
{
    if (m_uploadQueue.isEmpty())
    {
        m_progressDlg->reset();
        m_progressDlg->hide();
        //slotAlbumSelected();
        return;
    }

    typedef QPair<KUrl, FPhotoInfo> Pair;
    Pair pathComments = m_uploadQueue.first();
    FPhotoInfo info   = pathComments.second;

    // Find out the selected photo set.
    QString selectedPhotoSetId = m_albumsListComboBox->itemData(m_albumsListComboBox->currentIndex()).toString();
    if (selectedPhotoSetId == "") {
       m_talker->m_selectedPhotoSet = FPhotoSet();
    } else {
       QLinkedList<FPhotoSet>::iterator it = m_talker->m_photoSetsList->begin();
       while(it != m_talker->m_photoSetsList->end()) {
          if (it->id == selectedPhotoSetId) {
             m_talker->m_selectedPhotoSet = *it;
             break;
          }
          it++;
       }
    }

    bool res          = m_talker->addPhoto(pathComments.first.path(), //the file path
                                           info,
                                           m_resizeCheckBox->isChecked(),
                                           m_dimensionSpinBox->value(),
                                           m_imageQualitySpinBox->value()
                       );
    if (!res)
    {
        slotAddPhotoFailed("");
        return;
    }

    m_progressDlg->setLabelText(i18n("Uploading file %1",pathComments.first.fileName()));

    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void FlickrWindow::slotAddPhotoSucceeded()
{
    // Remove photo uploaded from the list
    m_imglst->removeItemByUrl(m_uploadQueue.first().first);
    m_uploadQueue.pop_front();
    m_uploadCount++;
    m_progressDlg->setMaximum(m_uploadTotal);
    m_progressDlg->setValue(m_uploadCount);
    slotAddPhotoNext();
}

void FlickrWindow::slotListPhotoSetsFailed(const QString& msg) {
    KMessageBox::error(this,
                 i18n("Failed to Fetch Photoset information from %1. %2\n", m_serviceName, msg));
}
void FlickrWindow::slotAddPhotoFailed(const QString& msg)
{
    if (KMessageBox::warningContinueCancel(this,
                     i18n("Failed to upload photo into %1. %2\nDo you want to continue?", m_serviceName, msg))
                     != KMessageBox::Continue)
    {
        m_uploadQueue.clear();
        m_progressDlg->reset();
        m_progressDlg->hide();
        // refresh the thumbnails
        //slotTagSelected();
    }
    else
    {
        m_uploadQueue.pop_front();
        m_uploadTotal--;
        m_progressDlg->setMaximum(m_uploadTotal);
        m_progressDlg->setValue(m_uploadCount);
        slotAddPhotoNext();
    }
}

void FlickrWindow::slotAddPhotoSetSucceeded()
{
  /* Method called when a photo set has been succesfully created on Flickr.
   * It functions to restart the normal flow after a photo set has been created
   * on Flickr. */
  populatePhotoSetComboBox();
  slotAddPhotoSucceeded();
}

void FlickrWindow::slotAddPhotoCancel()
{
    m_uploadQueue.clear();
    m_progressDlg->reset();
    m_progressDlg->hide();

    m_talker->cancel();

    // refresh the thumbnails
    //slotTagSelected();
}

void FlickrWindow::slotImageListChanged()
{
    enableButton(User1, !(m_widget->m_imglst->imageUrls().isEmpty()));
}

} // namespace KIPIFlickrExportPlugin
