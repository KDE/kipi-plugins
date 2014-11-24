/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-09-28
 * Description : a tool to export image to a KIO accessible
 *               location
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "KioExportWidget.moc"

// Qt includes

#include <QVBoxLayout>
#include <QLabel>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kdebug.h>
#include <khbox.h>
#include <ktoolinvocation.h>
#include <kcombobox.h>

// Local includes

#include "kpimageslist.h"

namespace KIPIKioExportPlugin
{

KioExportWidget::KioExportWidget(QWidget* const parent)
    : QWidget(parent)
{
    // setup kio target selection

    KHBox* const hbox   = new KHBox(this);
    QLabel* const label = new QLabel(hbox);
    m_targetLabel       = new KUrlComboRequester(hbox);
    m_targetDialog      = 0;

    if(m_targetLabel->button())
        m_targetLabel->button()->hide();

    m_targetLabel->comboBox()->setEditable(true);

    label->setText(i18n("Target location: "));
    m_targetLabel->setWhatsThis(i18n(
                    "Sets the target address to upload the images to. "
                    "This can be any address as used in Dolphin or Konqueror, "
                    "e.g. ftp://my.server.org/sub/folder."));

    m_targetSearchButton = new KPushButton(i18n("Select target location..."), this);
    m_targetSearchButton->setIcon(KIcon("folder-remote"));

    // setup image list
    m_imageList = new KPImagesList(this);
    m_imageList->setAllowRAW(true);
    m_imageList->listView()->setWhatsThis(i18n("This is the list of images to upload "
                                               "to the specified target."));
    m_imageList->loadImagesFromCurrentSelection();

    // layout dialog
    QVBoxLayout* const layout = new QVBoxLayout(this);

    layout->addWidget(hbox);
    layout->addWidget(m_targetSearchButton);
    layout->addWidget(m_imageList);
    layout->setSpacing(KDialog::spacingHint());
    layout->setMargin(0);

    // ------------------------------------------------------------------------

    connect(m_targetSearchButton, SIGNAL(clicked(bool)),
            this, SLOT(slotShowTargetDialogClicked(bool)));

    connect(m_targetLabel, SIGNAL(textChanged(QString)),
            this, SLOT(slotLabelUrlChanged()));

    // ------------------------------------------------------------------------

    updateTargetLabel();
}

KioExportWidget::~KioExportWidget()
{
}

KUrl KioExportWidget::targetUrl() const
{
    return m_targetUrl;
}

KUrl::List KioExportWidget::history() const
{
    KUrl::List urls;

    for (int i=0; i <= m_targetLabel->comboBox()->count(); i++)
        urls << m_targetLabel->comboBox()->itemText(i);

    return urls;
}

void KioExportWidget::setHistory(const KUrl::List& urls)
{
    m_targetLabel->comboBox()->clear();

    foreach (KUrl url, urls)
        m_targetLabel->comboBox()->addUrl(url);
}

void KioExportWidget::setTargetUrl(const KUrl& url)
{
    m_targetUrl = url;
    updateTargetLabel();
}

void KioExportWidget::slotShowTargetDialogClicked(bool checked)
{
    Q_UNUSED(checked);

    m_targetDialog = new KFileDialog(KUrl(), "*", this);
    m_targetDialog->setMode(KFile::Directory);
    m_targetDialog->setWindowTitle(i18n("Select target..."));
    m_targetDialog->setOperationMode(KFileDialog::Other);
    m_targetDialog->setUrl(m_targetUrl); // should this check if m_targetUrl is non-null?

    if (m_targetDialog->exec() == KFileDialog::Accepted)
    {
        m_targetUrl = m_targetDialog->selectedUrl();
        updateTargetLabel();
        emit signalTargetUrlChanged(m_targetUrl);
    }

    delete m_targetDialog;
}

void KioExportWidget::updateTargetLabel()
{
    kDebug() << "Call for url "
             << m_targetUrl.prettyUrl() << ", valid = "
             << m_targetUrl.isValid();

    QString urlString = '<' + i18n("not selected") + '>';

    if (m_targetUrl.isValid())
    {
        urlString = m_targetUrl.prettyUrl();
        m_targetLabel->setUrl(urlString);
    }
}

KPImagesList* KioExportWidget::imagesList() const
{
    return m_imageList;
}

void KioExportWidget::slotLabelUrlChanged()
{
    m_targetUrl = m_targetLabel->url();
    emit signalTargetUrlChanged(m_targetUrl);
}

} // namespace KIPIKioExportPlugin
