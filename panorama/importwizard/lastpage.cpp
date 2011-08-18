/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "lastpage.moc"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCheckBox>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>
#include <klineedit.h>
#include <kdebug.h>

// Local includes

#include "manager.h"
#include "actionthread.h"

namespace KIPIPanoramaPlugin
{

struct LastPage::LastPagePriv
{
    LastPagePriv() : title(0), saveSettingsGroupBox(0), fileTemplateKLineEdit(0), savePtoCheckBox(0), mngr(0) {}

    QLabel*    title;

    QGroupBox* saveSettingsGroupBox;
    KLineEdit* fileTemplateKLineEdit;
    QCheckBox* savePtoCheckBox;

    Manager*   mngr;
};

LastPage::LastPage(Manager* mngr, KAssistantDialog* dlg)
        : KIPIPlugins::WizardPage(dlg, i18n("Panorama Stitched")),
          d(new LastPagePriv)
{
    KConfig config("kipirc");
    KConfigGroup group        = config.group(QString("Panorama Settings"));

    d->mngr                   = mngr;
    KVBox *vbox               = new KVBox(this);
    d->title                  = new QLabel(vbox);
    d->title->setOpenExternalLinks(true);
    d->title->setWordWrap(true);

    QLabel* space             = new QLabel(vbox);

    QVBoxLayout *formatVBox   = new QVBoxLayout();
    d->saveSettingsGroupBox   = new QGroupBox(i18n("Save Settings"), vbox);
    d->saveSettingsGroupBox->setLayout(formatVBox);
    formatVBox->addStretch(1);

    QLabel *fileTemplateLabel = new QLabel(i18n("File Name Template: "), d->saveSettingsGroupBox);
    formatVBox->addWidget(fileTemplateLabel);
    // TODO: change the default name to something similar to what is done within hugin
    d->fileTemplateKLineEdit  = new KLineEdit("panorama", d->saveSettingsGroupBox);
    d->fileTemplateKLineEdit->setToolTip(i18n("Name of the panorama file (without its extension)."));
    d->fileTemplateKLineEdit->setWhatsThis(i18n("<b>File Name Template</b>: Set here the base name of the files that "
                                                "will be saved. For example, if your template is <i>panorama</i> and if "
                                                "you chose a JPEG output, then your panorama will be saved with the "
                                                "name <i>panorama.jpg</i>. If you choose to save also the project file, "
                                                "it will have the name <i>panorama.pto</i>."));
    formatVBox->addWidget(d->fileTemplateKLineEdit);
    d->savePtoCheckBox        = new QCheckBox(i18n("Save Project File"), d->saveSettingsGroupBox);
    d->savePtoCheckBox->setChecked(group.readEntry("Save PTO", false));
    d->savePtoCheckBox->setToolTip(i18n("Save the project file for further processing within Hugin GUI."));
    d->savePtoCheckBox->setWhatsThis(i18n("<b>Save Project File</b>: You can keep the project file generated to stitch "
                                          "your panorama for further tweaking within "
                                          "<a href=\"http://hugin.sourceforge.net/\">Hugin</a> by checking this. "
                                          "This is useful if you want a different projection, modify the horizon or "
                                          "the center of the panorama, or modify the control points to get better results."));
    formatVBox->addWidget(d->savePtoCheckBox);
    vbox->setStretchFactor(space, 2);

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_panorama/pics/assistant-hugin.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->fileTemplateKLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotTemplateChanged(QString)));

    connect(d->mngr->thread(), SIGNAL(starting(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));
}

LastPage::~LastPage()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Panorama Settings"));
    group.writeEntry("Save PTO", d->savePtoCheckBox->isChecked());
    config.sync();

    delete d;
}

void LastPage::resetTitle()
{
    slotTemplateChanged(d->fileTemplateKLineEdit->text());
}

void LastPage::copyFiles()
{
    connect(d->mngr->thread(), SIGNAL(finished(KIPIPanoramaPlugin::ActionData)),
            this, SLOT(slotAction(KIPIPanoramaPlugin::ActionData)));

    KUrl panoUrl(d->mngr->preProcessedMap().begin().key());
    panoUrl.setFileName(panoFileName(d->fileTemplateKLineEdit->text()));
    d->mngr->thread()->copyFiles(d->mngr->autoOptimiseUrl(),
                                 d->mngr->panoUrl(),
                                 panoUrl.toLocalFile(),
                                 d->mngr->preProcessedMap(),
                                 d->savePtoCheckBox->isChecked()
                                );
    if (!d->mngr->thread()->isRunning())
        d->mngr->thread()->start();
}

void LastPage::slotAction(const KIPIPanoramaPlugin::ActionData& ad)
{
    if (!ad.starting)           // Something is complete...
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case COPY:
                {
                    //TODO
                    break;
                }
                default:
                {
                    kWarning() << "Unknown action " << ad.action;
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case COPY:
                {
                    emit signalCopyFinished();
                    break;
                }
                default:
                {
                    kWarning() << "Unknown action " << ad.action;
                    break;
                }
            }
        }
    }

}

void LastPage::slotTemplateChanged(const QString& fileTemplate)
{
    d->title->setText(i18n("<qt>"
                           "<p><h1><b>Panorama Stitching is Done</b></h1></p>"
                           "<p>Congratulations. Your images are stitched into a panorama.</p>"
                           "<p>Your panorama will be created to the directory</p>"
                           "<p><b>%1</b></p>"
                           "<p>once you press the <i>Finish</i> button, with the name set below.</p>"
                           "<p>If you choose to save the project file, and "
                           "if your images were raw images then the converted images used during "
                           "the stitching process will be copied at the same time (those are "
                           "TIFF files that can be big).</p>"
                           "</qt>",
                           d->mngr->preProcessedMap().begin().key().directory(),
                           panoFileName(fileTemplate),
                           fileTemplate + ".pto"
                          ));
}

QString LastPage::panoFileName(const QString& fileTemplate) const
{
    switch (d->mngr->format())
    {
        case ActionThread::JPEG:
            return fileTemplate + ".jpg";
        case ActionThread::TIFF:
            return fileTemplate + ".tif";
        // FIXME: this doesn't compile with GCC 4.6.1 (-Werror=return-type)
    }
}

}   // namespace KIPIPanoramaPlugin
