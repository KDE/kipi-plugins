/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : IPTC subjects settings page.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "iptcsubjects.h"
#include "iptcsubjects.moc"
#include "metadataedit_global.h"

// Qt includes.

#include <QButtonGroup>
#include <QCheckBox>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QRadioButton>
#include <QValidator>
#include <QGridLayout>
// KDE includes.

#include <kcombobox.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kglobal.h>
#include <khbox.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klistwidget.h>
#include <klocale.h>
#include <kstandarddirs.h>

// LibKExiv2 includes.

#include <libkexiv2/kexiv2.h>

namespace KIPIMetadataEditPlugin
{

class IPTCSubjectsPriv
{
public:

    enum EditionMode
    {
        STANDARD = 0,
        CUSTOM
    };

    IPTCSubjectsPriv()
    {
        addSubjectButton = 0;
        delSubjectButton = 0;
        repSubjectButton = 0;
        subjectsBox      = 0;
        subjectsCheck    = 0;
        iprEdit          = 0;
        refEdit          = 0;
        nameEdit         = 0;
        matterEdit       = 0;
        detailEdit       = 0;
        iprLabel         = 0;
        refLabel         = 0;
        nameLabel        = 0;
        matterLabel      = 0;
        detailLabel      = 0;
        btnGroup         = 0;
        stdBtn           = 0;
        customBtn        = 0;
        refCB            = 0;
        optionsBox       = 0;
    }

    typedef QMap<QString, SubjectData>  SubjectCodesMap;

    SubjectCodesMap                     subMap;

    QStringList                         oldSubjects;

    QWidget                            *optionsBox;

    QPushButton                        *addSubjectButton;
    QPushButton                        *delSubjectButton;
    QPushButton                        *repSubjectButton;

    QCheckBox                          *subjectsCheck;

    QLabel                             *iprLabel;
    QLabel                             *refLabel;
    QLabel                             *nameLabel;
    QLabel                             *matterLabel;
    QLabel                             *detailLabel;

    QButtonGroup                       *btnGroup;

    QRadioButton                       *stdBtn;
    QRadioButton                       *customBtn;

    KComboBox                          *refCB;

    KLineEdit                          *iprEdit;
    KLineEdit                          *refEdit;
    KLineEdit                          *nameEdit;
    KLineEdit                          *matterEdit;
    KLineEdit                          *detailEdit;

    KListWidget                        *subjectsBox;
};

IPTCSubjects::IPTCSubjects(QWidget* parent)
            : QWidget(parent), d(new IPTCSubjectsPriv)
{
    // Load subject codes provided by IPTC/NAA as xml file.
    // See http://www.iptc.org/NewsCodes/nc_ts-table01.php for details.

    KGlobal::dirs()->addResourceDir("iptcschema", KStandardDirs::installPath("data") +
                                                  QString("kipiplugin_metadataedit/data"));
    QString path = KGlobal::dirs()->findResource("iptcschema", "topicset.iptc-subjectcode.xml");

    if (!loadSubjectCodesFromXML(KUrl(path)))
        kDebug( 51001 ) << "Cannot load IPTC/NAA subject codes XML database";

    // --------------------------------------------------------

    // Subject string only accept printable Ascii char excepted these one:
    // - '*' (\x2A)
    // - ':' (\x3A)
    // - '?' (\x3F)
    QRegExp subjectAsciiRx("[\x20-\x29\x2B-\x39\x3B-\x3E\x40-\x7F]+$");
    QValidator *subjectAsciiValidator = new QRegExpValidator(subjectAsciiRx, this);

    // Subject Reference Number only accept digit.
    QRegExp refDigitRx("^[0-9]{8}$");
    QValidator *refValidator = new QRegExpValidator(refDigitRx, this);

    d->subjectsCheck = new QCheckBox(i18n("Use structured definition of the subject matter:"), this);

    // --------------------------------------------------------

    d->optionsBox       = new QWidget;
    d->btnGroup         = new QButtonGroup;
    d->stdBtn           = new QRadioButton;
    d->customBtn        = new QRadioButton;
    d->refCB            = new KComboBox;

    QString codeLinkStr = i18n("Use standard <b><a href='http://www.iptc.org/NewsCodes'>reference code</a></b>");
    QLabel *codeLink    = new QLabel(codeLinkStr);
    codeLink->setOpenExternalLinks(true);
    codeLink->setWordWrap(false);

    QLabel *customLabel = new QLabel(i18n("Use custom definition"));

    d->btnGroup->addButton(d->stdBtn,    IPTCSubjectsPriv::STANDARD);
    d->btnGroup->addButton(d->customBtn, IPTCSubjectsPriv::CUSTOM);
    d->btnGroup->setExclusive(true);
    d->stdBtn->setChecked(true);


    for (IPTCSubjectsPriv::SubjectCodesMap::Iterator it = d->subMap.begin();
         it != d->subMap.end(); ++it)
        d->refCB->addItem(it.key());

    // --------------------------------------------------------

    d->iprEdit = new KLineEdit;
    d->iprEdit->setClearButtonShown(true);
    d->iprEdit->setValidator(subjectAsciiValidator);
    d->iprEdit->setMaxLength(32);
    d->iprEdit->setWhatsThis(i18n("Enter here the Informative Provider Reference. "
                                  "I.P.R is a name registered with the IPTC/NAA, identifying the "
                                  "provider that provides an indicator of the content. "
                                  "The default value for the I.P.R is \"IPTC\" if a standard Reference "
                                  "Code is used. This field is limited to 32 ASCII characters."));

    // --------------------------------------------------------

    d->refEdit = new KLineEdit;
    d->refEdit->setClearButtonShown(true);
    d->refEdit->setValidator(refValidator);
    d->refEdit->setMaxLength(8);
    d->refEdit->setWhatsThis(i18n("Enter here the Subject Reference Number. "
                                  "Provides a numeric code to indicate the Subject Name plus "
                                  "optional Subject Matter and Subject Detail Names in the "
                                  "language of the service. Subject Reference is a number "
                                  "from the range 01000000 to 17999999 and represent a "
                                  "language independent international reference to "
                                  "a Subject. A Subject is identified by its Reference Number "
                                  "and corresponding Names taken from a standard lists given "
                                  "by IPTC/NAA. If a standard reference code is used, these lists "
                                  "are the English language reference versions. "
                                  "This field is limited to 8 ASCII digit code."));

    // --------------------------------------------------------

    d->nameEdit = new KLineEdit;
    d->nameEdit->setClearButtonShown(true);
    d->nameEdit->setValidator(subjectAsciiValidator);
    d->nameEdit->setMaxLength(64);
    d->nameEdit->setWhatsThis(i18n("Enter here the Subject Name. English language is used "
                                   "if you selected a standard IPTC/NAA reference code. "
                                   "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->matterEdit = new KLineEdit;
    d->matterEdit->setClearButtonShown(true);
    d->matterEdit->setValidator(subjectAsciiValidator);
    d->matterEdit->setMaxLength(64);
    d->matterEdit->setWhatsThis(i18n("Enter here the Subject Matter Name. English language is used "
                                     "if you selected a standard IPTC/NAA reference code. "
                                     "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->detailEdit = new KLineEdit;
    d->detailEdit->setClearButtonShown(true);
    d->detailEdit->setValidator(subjectAsciiValidator);
    d->detailEdit->setMaxLength(64);
    d->detailEdit->setWhatsThis(i18n("Enter here the Subject Detail Name. English language is used "
                                     "if you selected a standard IPTC/NAA reference code. "
                                     "This field is limited to 64 ASCII characters."));

    // --------------------------------------------------------

    d->iprLabel    = new QLabel(i18n("I.P.R:"));
    d->refLabel    = new QLabel(i18n("Reference:"));
    d->nameLabel   = new QLabel(i18n("Name:"));
    d->matterLabel = new QLabel(i18n("Matter:"));
    d->detailLabel = new QLabel(i18n("Detail:"));

    // --------------------------------------------------------


    d->subjectsBox = new KListWidget;
    d->subjectsBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    d->addSubjectButton = new QPushButton( i18n("&Add"));
    d->delSubjectButton = new QPushButton( i18n("&Delete"));
    d->repSubjectButton = new QPushButton( i18n("&Replace"));
    d->addSubjectButton->setIcon(SmallIcon("list-add"));
    d->delSubjectButton->setIcon(SmallIcon("edit-delete"));
    d->repSubjectButton->setIcon(SmallIcon("view-refresh"));
    d->delSubjectButton->setEnabled(false);
    d->repSubjectButton->setEnabled(false);

    // --------------------------------------------------------

    QLabel *note = new QLabel(i18n("<b>Note: "
                 "<b><a href='http://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> "
                 "text tags only support the printable "
                 "<b><a href='http://en.wikipedia.org/wiki/Ascii'>ASCII</a></b> "
                 "characters set and limit strings size. "
                 "Use contextual help for details.</b>"));
    note->setMaximumWidth(150);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    QGridLayout *optionsBoxLayout = new QGridLayout;
    optionsBoxLayout->addWidget(d->stdBtn,      0, 0, 1, 1);
    optionsBoxLayout->addWidget(codeLink,       0, 1, 1, 2);
    optionsBoxLayout->addWidget(d->refCB,       0, 3, 1, 1);
    optionsBoxLayout->addWidget(d->customBtn,   1, 0, 1, 4);
    optionsBoxLayout->addWidget(customLabel,    1, 1, 1, 4);
    optionsBoxLayout->addWidget(d->iprLabel,    2, 0, 1, 1);
    optionsBoxLayout->addWidget(d->iprEdit,     2, 1, 1, 4);
    optionsBoxLayout->addWidget(d->refLabel,    3, 0, 1, 1);
    optionsBoxLayout->addWidget(d->refEdit,     3, 1, 1, 1);
    optionsBoxLayout->addWidget(d->nameLabel,   4, 0, 1, 1);
    optionsBoxLayout->addWidget(d->nameEdit,    4, 1, 1, 4);
    optionsBoxLayout->addWidget(d->matterLabel, 5, 0, 1, 1);
    optionsBoxLayout->addWidget(d->matterEdit,  5, 1, 1, 4);
    optionsBoxLayout->addWidget(d->detailLabel, 6, 0, 1, 1);
    optionsBoxLayout->addWidget(d->detailEdit,  6, 1, 1, 4);
    optionsBoxLayout->setColumnStretch(4, 10);
    optionsBoxLayout->setMargin(0);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    d->optionsBox->setLayout(optionsBoxLayout);

    // --------------------------------------------------------

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setAlignment( Qt::AlignTop );
    mainLayout->addWidget(d->subjectsCheck,    0, 0, 1, 4);
    mainLayout->addWidget(d->optionsBox,       1, 0, 1, 4);
    mainLayout->addWidget(d->subjectsBox,      2, 0, 5, 3);
    mainLayout->addWidget(d->addSubjectButton, 2, 3, 1, 1);
    mainLayout->addWidget(d->delSubjectButton, 3, 3, 1, 1);
    mainLayout->addWidget(d->repSubjectButton, 4, 3, 1, 1);
    mainLayout->addWidget(note, 5, 3, 1, 1);
    mainLayout->setRowStretch(6, 10);
    mainLayout->setColumnStretch(2, 1);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(KDialog::spacingHint());
    setLayout(mainLayout);

    // --------------------------------------------------------

    connect(d->subjectsBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSubjectSelectionChanged()));

    connect(d->addSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotAddSubject()));

    connect(d->delSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotDelSubject()));

    connect(d->repSubjectButton, SIGNAL(clicked()),
            this, SLOT(slotRepSubject()));

    connect(d->btnGroup, SIGNAL(buttonReleased(int)),
            this, SLOT(slotEditOptionChanged(int)));

    connect(d->refCB, SIGNAL(activated(int)),
            this, SLOT(slotRefChanged()));

    // --------------------------------------------------------

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotSubjectsToggled(bool)));

    // --------------------------------------------------------

    connect(d->subjectsCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->delSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repSubjectButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
}

IPTCSubjects::~IPTCSubjects()
{
    delete d;
}

void IPTCSubjects::slotSubjectsToggled(bool b)
{
    d->optionsBox->setEnabled(b);
    d->subjectsBox->setEnabled(b);
    d->addSubjectButton->setEnabled(b);
    d->delSubjectButton->setEnabled(b);
    d->repSubjectButton->setEnabled(b);
    slotEditOptionChanged(d->btnGroup->id(d->btnGroup->checkedButton()));
}

void IPTCSubjects::slotEditOptionChanged(int b)
{
    if (b == IPTCSubjectsPriv::CUSTOM)
    {
        d->refCB->setEnabled(false);
        d->iprLabel->setEnabled(true);
        d->refLabel->setEnabled(true);
        d->nameLabel->setEnabled(true);
        d->matterLabel->setEnabled(true);
        d->detailLabel->setEnabled(true);
        d->iprEdit->setEnabled(true);
        d->refEdit->setEnabled(true);
        d->nameEdit->setEnabled(true);
        d->matterEdit->setEnabled(true);
        d->detailEdit->setEnabled(true);
    }
    else
    {
        d->refCB->setEnabled(true);
        d->iprLabel->setEnabled(false);
        d->refLabel->setEnabled(false);
        d->nameLabel->setEnabled(false);
        d->matterLabel->setEnabled(false);
        d->detailLabel->setEnabled(false);
        d->iprEdit->setEnabled(false);
        d->refEdit->setEnabled(false);
        d->nameEdit->setEnabled(false);
        d->matterEdit->setEnabled(false);
        d->detailEdit->setEnabled(false);
        slotRefChanged();
    }
}

void IPTCSubjects::slotRefChanged()
{
    QString key = d->refCB->currentText();
    QString name, matter, detail;

    for (IPTCSubjectsPriv::SubjectCodesMap::Iterator it = d->subMap.begin();
         it != d->subMap.end(); ++it)
    {
        if (key == it.key())
        {
            name   = it.value().name;
            matter = it.value().matter;
            detail = it.value().detail;
        }
    }

    d->iprEdit->setText("IPTC");
    d->refEdit->setText(key);
    d->nameEdit->setText(name);
    d->matterEdit->setText(matter);
    d->detailEdit->setText(detail);
}

QString IPTCSubjects::buildSubject() const
{
    QString subject = d->iprEdit->text();
    subject.append(":");
    subject.append(d->refEdit->text());
    subject.append(":");
    subject.append(d->nameEdit->text());
    subject.append(":");
    subject.append(d->matterEdit->text());
    subject.append(":");
    subject.append(d->detailEdit->text());
    return subject;
}

void IPTCSubjects::slotDelSubject()
{
    QListWidgetItem *item = d->subjectsBox->currentItem();
    if (!item) return;
    d->subjectsBox->takeItem(d->subjectsBox->row(item));
    delete item;
}

void IPTCSubjects::slotRepSubject()
{
    QString newSubject = buildSubject();
    if (newSubject.isEmpty()) return;

    if (!d->subjectsBox->selectedItems().isEmpty())
    {
        d->subjectsBox->selectedItems()[0]->setText(newSubject);
        d->iprEdit->clear();
        d->refEdit->clear();
        d->nameEdit->clear();
        d->matterEdit->clear();
        d->detailEdit->clear();
    }
}

void IPTCSubjects::slotSubjectSelectionChanged()
{
    if (!d->subjectsBox->selectedItems().isEmpty())
    {
        QString subject = d->subjectsBox->selectedItems()[0]->text();
        d->iprEdit->setText(subject.section(':', 0, 0));
        d->refEdit->setText(subject.section(':', 1, 1));
        d->nameEdit->setText(subject.section(':', 2, 2));
        d->matterEdit->setText(subject.section(':', 3, 3));
        d->detailEdit->setText(subject.section(':', 4, 4));
        d->delSubjectButton->setEnabled(true);
        d->repSubjectButton->setEnabled(true);
    }
    else
    {
        d->delSubjectButton->setEnabled(false);
        d->repSubjectButton->setEnabled(false);
    }
}

void IPTCSubjects::slotAddSubject()
{
    QString newSubject = buildSubject();
    if (newSubject.isEmpty()) return;

    bool found = false;
    for (int i = 0 ; i < d->subjectsBox->count(); i++)
    {
        QListWidgetItem *item = d->subjectsBox->item(i);
        if (newSubject == item->text())
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->subjectsBox->insertItem(d->subjectsBox->count(), newSubject);
        d->iprEdit->clear();
        d->refEdit->clear();
        d->nameEdit->clear();
        d->matterEdit->clear();
        d->detailEdit->clear();
    }
}

void IPTCSubjects::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    d->oldSubjects = exiv2Iface.getIptcSubjects();

    d->subjectsBox->clear();
    d->subjectsCheck->setChecked(false);
    if (!d->oldSubjects.isEmpty())
    {
        d->subjectsBox->insertItems(0, d->oldSubjects);
        d->subjectsCheck->setChecked(true);
    }
    blockSignals(false);
    slotSubjectsToggled(d->subjectsCheck->isChecked());
}

void IPTCSubjects::applyMetadata(QByteArray& iptcData)
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.setIptc(iptcData);
    QStringList newSubjects;

    for (int i = 0 ; i < d->subjectsBox->count(); i++)
    {
        QListWidgetItem *item = d->subjectsBox->item(i);
        newSubjects.append(item->text());
    }

    if (d->subjectsCheck->isChecked())
        exiv2Iface.setIptcSubjects(d->oldSubjects, newSubjects);
    else
        exiv2Iface.setIptcSubjects(d->oldSubjects, QStringList());

    iptcData = exiv2Iface.getIptc();
}

bool IPTCSubjects::loadSubjectCodesFromXML(const KUrl& url)
{
    QFile xmlfile(url.path());

    if (!xmlfile.open(QIODevice::ReadOnly))
        return false;

    QDomDocument xmlDoc("NewsML");
    if (!xmlDoc.setContent(&xmlfile))
        return false;

    QDomElement xmlDocElem = xmlDoc.documentElement();
    if (xmlDocElem.tagName()!="NewsML")
        return false;

    for (QDomNode nbE1 = xmlDocElem.firstChild();
         !nbE1.isNull(); nbE1 = nbE1.nextSibling())
    {
        QDomElement newsItemElement = nbE1.toElement();
        if (newsItemElement.isNull()) continue;
        if (newsItemElement.tagName() != "NewsItem") continue;

        for (QDomNode nbE2 = newsItemElement.firstChild();
            !nbE2.isNull(); nbE2 = nbE2.nextSibling())
        {
            QDomElement topicSetElement = nbE2.toElement();
            if (topicSetElement.isNull()) continue;
            if (topicSetElement.tagName() != "TopicSet") continue;

            for (QDomNode nbE3 = topicSetElement.firstChild();
                !nbE3.isNull(); nbE3 = nbE3.nextSibling())
            {
                QDomElement topicElement = nbE3.toElement();
                if (topicElement.isNull()) continue;
                if (topicElement.tagName() != "Topic") continue;

                QString type, name, matter, detail, ref;
                for (QDomNode nbE4 = topicElement.firstChild();
                    !nbE4.isNull(); nbE4 = nbE4.nextSibling())
                {
                    QDomElement topicSubElement = nbE4.toElement();
                    if (topicSubElement.isNull()) continue;

                    if (topicSubElement.tagName() == "TopicType")
                        type = topicSubElement.attribute("FormalName");

                    if (topicSubElement.tagName() == "FormalName")
                        ref = topicSubElement.text();

                    if (topicSubElement.tagName() == "Description" &&
                        topicSubElement.attribute("Variant") == "Name")
                    {
                        if (type == "Subject")
                            name = topicSubElement.text();
                        else if (type == "SubjectMatter")
                            matter = topicSubElement.text();
                        else if (type == "SubjectDetail")
                            detail = topicSubElement.text();
                    }
                }

                d->subMap.insert(ref, SubjectData(name, matter, detail));
            }
        }
    }

    // Set the Subject Name everywhere on the map.

    for (IPTCSubjectsPriv::SubjectCodesMap::Iterator it = d->subMap.begin();
         it != d->subMap.end(); ++it)
    {
        QString name, keyPrefix;
        if (it.key().endsWith("00000"))
        {
            keyPrefix = it.key().left(3);
            name      = it.value().name;

            for (IPTCSubjectsPriv::SubjectCodesMap::Iterator it2 = d->subMap.begin();
                it2 != d->subMap.end(); ++it2)
            {
                if (it2.key().startsWith(keyPrefix) &&
                    !it2.key().endsWith("00000"))
                {
                    it2.value().name = name;
                }
            }
        }
    }

    // Set the Subject Matter Name everywhere on the map.

    for (IPTCSubjectsPriv::SubjectCodesMap::Iterator it = d->subMap.begin();
         it != d->subMap.end(); ++it)
    {
        QString matter, keyPrefix;
        if (it.key().endsWith("000"))
        {
            keyPrefix = it.key().left(5);
            matter    = it.value().matter;

            for (IPTCSubjectsPriv::SubjectCodesMap::Iterator it2 = d->subMap.begin();
                it2 != d->subMap.end(); ++it2)
            {
                if (it2.key().startsWith(keyPrefix) &&
                    !it2.key().endsWith("000"))
                {
                    it2.value().matter = matter;
                }
            }
        }
    }

    return true;
}

}  // namespace KIPIMetadataEditPlugin
