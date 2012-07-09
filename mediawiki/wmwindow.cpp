/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to wikimedia commons
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
 * Copyright (C) 2012      by Peter Potrowl <peter dot potrowl at gmail dot com>
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

#include "wmwindow.moc"

// Qt includes

#include <QLayout>
#include <QCloseEvent>
#include <QFileInfo>

// KDE includes

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmenu.h>
#include <kurl.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kmessagebox.h>

// Mediawiki includes

#include <libmediawiki/version.h>

// KIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "wmwidget.h"
#include "wikimediajob.h"

namespace KIPIWikiMediaPlugin
{

WMWindow::WMWindow(const QString& tmpFolder, QWidget* const /*parent*/)
    : KPToolDialog(0)
{
    m_tmpPath.clear();
    m_tmpDir    = tmpFolder;
    m_widget    = new WmWidget(this);
    m_uploadJob = 0;
    m_login     = QString();
    m_pass      = QString();

    setMainWidget(m_widget);
    setWindowIcon(KIcon("wikimedia"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);
    setWindowTitle(i18n("Export to Wikimedia Commons"));
    setButtonGuiItem(User1, KGuiItem(i18n("Start Upload"), "network-workgroup",
                                     i18n("Start upload to Wikimedia Commons")));
    enableButton(User1, false);
    m_widget->setMinimumSize(700, 500);
    m_widget->installEventFilter(this);

    KPAboutData* about = new KPAboutData(ki18n("Wikimedia Commons Export"), 0,
                                         KAboutData::License_GPL,
                                         ki18n("A Kipi plugin to export image collection "
                                               "to Wikimedia Commons.\n"
                                               "Using libmediawiki version %1").subs(QString(mediawiki_version)),
                                         ki18n("(c) 2011, Alexandre Mendes"));

    about->addAuthor(ki18n("Alexandre Mendes"), ki18n("Author"),
                     "alex dot mendes1988 at gmail dot com");

    about->addAuthor(ki18n("Guillaume Hormiere"), ki18n("Developer"),
                     "hormiere dot guillaume at gmail dot com");

    about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Peter Potrowl"), ki18n("Developer"),
                     "peter dot potrowl at gmail dot com");

    about->setHandbookEntry("wikimedia");
    setAboutData(about);

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(m_widget, SIGNAL(signalChangeUserRequest()),
            this, SLOT(slotChangeUserClicked()));

    connect(m_widget, SIGNAL(signalLoginRequest(QString, QString, QUrl)),
            this, SLOT(slotDoLogin(QString, QString, QUrl)));

    connect(m_widget->progressBar(), SIGNAL(signalProgressCanceled()),
            this, SLOT(slotClose()));

    readSettings();
    reactivate();
}

WMWindow::~WMWindow()
{
}

void WMWindow::closeEvent(QCloseEvent* e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void WMWindow::reactivate()
{
    m_widget->imagesList()->listView()->clear();
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    m_widget->loadImageInfoFirstLoad();

    show();
}

void WMWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Wikimedia Commons settings"));

    m_widget->readSettings(group);

    KConfigGroup group2 = config.group(QString("Wikimedia Commons dialog"));
    restoreDialogSize(group2);
}

void WMWindow::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Wikimedia Commons settings"));

    m_widget->saveSettings(group);

    KConfigGroup group2 = config.group(QString("Wikimedia Commons dialog"));
    saveDialogSize(group2);
    config.sync();
}

void WMWindow::slotClose()
{
    m_widget->clearImagesDesc();
    m_widget->progressBar()->progressCompleted();
    saveSettings();
    done(Close);
}

QString WMWindow::getImageCaption(const QString& fileName)
{
    KPImageInfo info(fileName);
    QStringList descriptions = QStringList() << info.title() << info.description();
    descriptions.removeAll("");
    return descriptions.join("\n\n");
}

bool WMWindow::prepareImageForUpload(const QString& imgPath, QString& caption)
{
    QImage image;
    image.load(imgPath);

    if (image.isNull())
    {
        return false;
    }

    // get temporary file name
    m_tmpPath = m_tmpDir + QFileInfo(imgPath).baseName().trimmed() + ".jpg";

    // rescale image if requested
    int maxDim = m_widget->dimension();

    if (image.width() > maxDim || image.height() > maxDim)
    {
        kDebug() << "Resizing to " << maxDim;
        image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);
    }

    kDebug() << "Saving to temp file: " << m_tmpPath;
    image.save(m_tmpPath, "JPEG", m_widget->quality());

    // copy meta data to temporary image
    KPMetadata meta;

    if (meta.load(imgPath))
    {
        caption = getImageCaption(imgPath);
        meta.setImageDimensions(image.size());
        meta.save(m_tmpPath);
    }
    else
    {
        caption.clear();
    }

    return true;
}

void WMWindow::slotStartTransfer()
{
    saveSettings();
    KUrl::List urls = iface()->currentSelection().images();
    QMap <QString,QMap <QString,QString> > imagesDesc=m_widget->allImagesDesc();

    for (int i = 0; i < urls.size(); ++i)
    {
        QString caption;
        QString url;
        if(m_widget->resize())
        {
            prepareImageForUpload(urls.at(i).path(), caption);
            imagesDesc.insert(m_tmpPath, imagesDesc.take(urls.at(i).path()));
        }
    }

    m_uploadJob->setImageMap(imagesDesc);

    m_widget->progressBar()->setRange(0, 100);
    m_widget->progressBar()->setValue(0);

    connect(m_uploadJob, SIGNAL(uploadProgress(int)),
            m_widget->progressBar(), SLOT(setValue(int)));

    connect(m_uploadJob, SIGNAL(endUpload()),
            this, SLOT(slotEndUpload()));

    m_widget->progressBar()->show();
    m_widget->progressBar()->progressScheduled(i18n("Wiki Export"), true, true);
    m_widget->progressBar()->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));
    m_uploadJob->begin();
}

void WMWindow::slotChangeUserClicked()
{
    enableButton(User1, false);
    m_widget->invertAccountLoginBox();
}

void WMWindow::slotDoLogin(const QString& login, const QString& pass, const QUrl& wiki)
{
    m_login         = login;
    m_pass          = pass;
    m_wiki          = wiki;
    m_mediawiki     = new MediaWiki(wiki);
    Login* loginJob = new Login(*m_mediawiki, login, pass);

    connect(loginJob, SIGNAL(result(KJob*)), 
            this, SLOT(slotLoginHandle(KJob*)));

    loginJob->start();
}

int WMWindow::slotLoginHandle(KJob* loginJob)
{
    kDebug() << loginJob->error();

    if(loginJob->error())
    {
        m_login.clear();
        m_pass.clear();
        m_uploadJob = 0;
        //TODO Message d'erreur de login
        KMessageBox::error(this, i18n("Login error\nPlease check your credentials and try again."));
    }
    else
    {
        m_uploadJob = new WikiMediaJob(iface(), m_mediawiki, this);
        enableButton(User1, true);
        m_widget->invertAccountLoginBox();
        m_widget->updateLabels(m_login, m_wiki.toString());
    }

    return loginJob->error();
}

void WMWindow::slotEndUpload()
{
    disconnect(m_uploadJob, SIGNAL(uploadProgress(int)),
               m_widget->progressBar(),SLOT(setValue(int)));

    disconnect(m_uploadJob, SIGNAL(endUpload()),
               this, SLOT(slotEndUpload()));

    KMessageBox::information(this, i18n("Upload finished with no errors."));
    m_widget->progressBar()->hide();
    m_widget->progressBar()->progressCompleted();
    hide();
}

bool WMWindow::eventFilter(QObject* /*obj*/, QEvent* event)
{
    if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* c = dynamic_cast<QKeyEvent *>(event);
        if(c && c->key() == Qt::Key_Return)
        {
            event->ignore();
            kDebug() << "Key event pass";
            return false;

        }
    }
    return true;
}

} // namespace KIPIWikiMediaPlugin
