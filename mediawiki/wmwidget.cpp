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

#include "wmwidget.moc"

// Qt includes

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QScrollArea>
#include <QMap>

// KDE includes

#include <kdebug.h>
#include <kvbox.h>
#include <klocale.h>
#include <kdialog.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kconfiggroup.h>
#include <kcombobox.h>

// LibKIPI includes

#include <libkipi/uploadwidget.h>

// libKdcraw includes

#include <libkdcraw/rexpanderbox.h>
#include <libkdcraw/version.h>
#include <libkdcraw/squeezedcombobox.h>

// Local includes

#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "wmwidget.h"

namespace KIPIWikiMediaPlugin
{

WmWidget::WmWidget(QWidget* const parent)
    : QWidget(parent)
{
    setObjectName("WmWidget");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // -------------------------------------------------------------------

    m_imgList = new KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(i18n("This is the list of images to upload to your Wikimedia account."));

    // ------------------------------------------------------------------------

    QScrollArea* sv = new QScrollArea(this);
    KVBox* panel    = new KVBox(sv->viewport());
    panel->setAutoFillBackground(false);
    sv->setWidget(panel);
    sv->setWidgetResizable(true);
    sv->setAutoFillBackground(false);
    sv->viewport()->setAutoFillBackground(false);

    m_headerLbl = new QLabel(panel);
    m_headerLbl->setWhatsThis(i18n("This is a clickable link to open the Wikimedia home page in a web browser."));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    m_settingsExpander = new RExpanderBox(panel);
    m_settingsExpander->setObjectName("MediaWiki Settings Expander");

    // ------------------------------------------------------------------------

    m_userBox    = new KVBox(panel);
    m_loginBox   = new QWidget(m_userBox);
    m_loginBox->setWhatsThis(i18n("This is the login form to your Wikimedia account."));
    QGridLayout* loginBoxLayout = new QGridLayout(m_loginBox);

    m_wikiSelect = new KUrlComboRequester(m_loginBox);
    m_nameEdit   = new KLineEdit(m_loginBox);
    m_passwdEdit = new KLineEdit(m_loginBox);
    m_passwdEdit->setEchoMode(KLineEdit::Password);

    if(m_wikiSelect->button())
        m_wikiSelect->button()->hide();
    
    m_wikiSelect->comboBox()->setEditable(true);

    QLabel* wikiLabel     = new QLabel(m_loginBox);
    wikiLabel->setText(i18n("Wiki:"));

    QLabel* nameLabel     = new QLabel(m_loginBox);
    nameLabel->setText(i18n( "Login:" ));

    QLabel* passwdLabel   = new QLabel(m_loginBox);
    passwdLabel->setText(i18n("Password:"));
    
    QPushButton* loginBtn = new QPushButton(m_loginBox);
    loginBtn->setAutoDefault(true);
    loginBtn->setDefault(true);
    loginBtn->setText(i18n("&Log in"));

    loginBoxLayout->addWidget(wikiLabel,    0, 0, 1, 1);
    loginBoxLayout->addWidget(m_wikiSelect, 0, 1, 1, 1);
    loginBoxLayout->addWidget(nameLabel,    1, 0, 1, 1);
    loginBoxLayout->addWidget(m_nameEdit,   1, 1, 1, 1);
    loginBoxLayout->addWidget(m_passwdEdit, 2, 1, 1, 1);
    loginBoxLayout->addWidget(passwdLabel,  2, 0, 1, 1);
    loginBoxLayout->addWidget(loginBtn,     3, 0, 1, 1);
    loginBoxLayout->setObjectName("m_loginBoxLayout");

    m_accountBox         = new KHBox(m_userBox);
    m_accountBox->setWhatsThis(i18n("This is the Wikimedia account that is currently logged in."));

    QLabel* userNameLbl  = new QLabel(m_accountBox);
    userNameLbl->setText(i18nc("Wikimedia account settings", "Logged as: "));
    m_userNameDisplayLbl = new QLabel(m_accountBox);
    QLabel* space        = new QLabel(m_accountBox);
    m_changeUserBtn      = new KPushButton(KGuiItem(i18n("Change Account"), "system-switch-user",
                               i18n("Logout and change Wikimedia Account used for transfer")),
                               m_accountBox);
    m_accountBox->setStretchFactor(space, 10);
    m_accountBox->hide();

    m_settingsExpander->addItem(m_userBox, i18n("Account"), QString("account"), true);
    m_settingsExpander->setItemIcon(0, SmallIcon("user-properties"));

    // ------------------------------------------------------------------------

    m_textBox                  = new QWidget(panel);
    m_textBox->setWhatsThis(i18n("This is the login form to your Wikimedia account."));
    QGridLayout* textBoxLayout = new QGridLayout(m_textBox);

    QLabel* aut          = new QLabel(i18n("Author:"), m_textBox);
    m_authorEdit         = new KLineEdit(m_textBox);

    QLabel* licenceLabel = new QLabel(i18n("License:"), m_textBox);
    m_licenceComboBox    = new SqueezedComboBox(m_textBox);

    m_licenceComboBox->addSqueezedItem(i18n("Own work, multi-license with CC-BY-SA-3.0 and GFDL"), 
                                QString("{{self|cc-by-sa-3.0|GFDL|migration=redundant}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Own work, multi-license with CC-BY-SA-3.0 and older"), 
                                QString("{{self|cc-by-sa-3.0,2.5,2.0,1.0}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Creative Commons Attribution-Share Alike 3.0"),
                                QString("{{self|cc-by-sa-3.0}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Own work, Creative Commons Attribution 3.0"),
                                QString("{{self|cc-by-3.0}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Own work, release into public domain under the CC-Zero license"),
                                QString("{{self|cc-zero}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Author died more than 100 years ago"),
                                QString("{{PD-old}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Photo of a two-dimensional work whose author died more than 100 years ago"),
                                QString("{{PD-art}}"));
    m_licenceComboBox->addSqueezedItem(i18n("First published in the United States before 1923"),
                                QString("{{PD-US}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Work of a U.S. government agency"),
                                QString("{{PD-USGov}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Simple typefaces, individual words or geometric shapes"),
                                QString("{{PD-text}}"));
    m_licenceComboBox->addSqueezedItem(i18n("Logos with only simple typefaces, individual words or geometric shapes"),
                                QString("{{PD-textlogo}}"));

    textBoxLayout->addWidget(aut,               1, 0, 1, 1);
    textBoxLayout->addWidget(m_authorEdit,      1, 2, 1, 2);
    textBoxLayout->addWidget(licenceLabel,      3, 0, 1, 1);
    textBoxLayout->addWidget(m_licenceComboBox, 3, 2, 1, 2);
    textBoxLayout->setObjectName("m_textBoxLayout");

    m_settingsExpander->addItem(m_textBox, i18n("Information"), QString("information"), true);
    m_settingsExpander->setItemIcon(1, SmallIcon("document-properties"));

    //------------------------------------------------------------------------------------

    m_optionsBox                  = new QWidget(panel);
    m_optionsBox->setWhatsThis(i18n("These are options that will be applied to photos before upload."));
    QGridLayout* optionsBoxLayout = new QGridLayout(m_optionsBox);

    m_resizeChB = new QCheckBox(m_optionsBox);
    m_resizeChB->setText(i18n("Resize photos before uploading"));
    m_resizeChB->setChecked(false);

    m_dimensionSpB = new QSpinBox(m_optionsBox);
    m_dimensionSpB->setMinimum(0);
    m_dimensionSpB->setMaximum(5000);
    m_dimensionSpB->setSingleStep(10);
    m_dimensionSpB->setValue(600);
    m_dimensionSpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_dimensionSpB->setEnabled(false);
    QLabel* dimensionLbl = new QLabel(i18n("Maximum size:"), m_optionsBox);
  
    m_imageQualitySpB = new QSpinBox(m_optionsBox);
    m_imageQualitySpB->setMinimum(0);
    m_imageQualitySpB->setMaximum(100);
    m_imageQualitySpB->setSingleStep(1);
    m_imageQualitySpB->setValue(85);
    m_imageQualitySpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* imageQualityLbl = new QLabel(i18n("JPEG quality:"), m_optionsBox);

    optionsBoxLayout->addWidget(m_resizeChB,       0, 0, 1, 2);
    optionsBoxLayout->addWidget(imageQualityLbl,   1, 0, 1, 1);
    optionsBoxLayout->addWidget(m_imageQualitySpB, 1, 1, 1, 1);
    optionsBoxLayout->addWidget(dimensionLbl,      2, 0, 1, 1);
    optionsBoxLayout->addWidget(m_dimensionSpB,    2, 1, 1, 1);
    optionsBoxLayout->setRowStretch(3, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    m_settingsExpander->addItem(m_optionsBox, i18n("Options"), QString("options"), true);
    m_settingsExpander->setItemIcon(2, SmallIcon("system-run"));

    // ------------------------------------------------------------------------

    m_progressBar = new KPProgressWidget(panel);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    // ------------------------------------------------------------------------

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(sv);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    updateLabels();  // use empty labels until login

    // ------------------------------------------------------------------------

    connect(m_resizeChB, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

    connect(m_changeUserBtn, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    connect(loginBtn, SIGNAL(clicked()),
            this, SLOT(slotLoginClicked()));
}

WmWidget::~WmWidget()
{
}

void WmWidget::readSettings(KConfigGroup& group)
{
    kDebug() <<  "pass here";

#if KDCRAW_VERSION >= 0x020000
    m_settingsExpander->readSettings(group);
#else
    m_settingsExpander->readSettings();
#endif

    m_resizeChB->setChecked(group.readEntry("Resize",      false));
    m_dimensionSpB->setValue(group.readEntry("Dimension",  600));
    m_imageQualitySpB->setValue(group.readEntry("Quality", 85));
    slotResizeChecked();

    m_history = group.readEntry("Urls history", QStringList());

    foreach(KUrl url, m_history)
    {
        m_wikiSelect->comboBox()->addUrl(url);
    }
}

void WmWidget::saveSettings(KConfigGroup& group)
{
    kDebug() <<  "pass here";

#if KDCRAW_VERSION >= 0x020000
    m_settingsExpander->writeSettings(group);
#else
    m_settingsExpander->writeSettings();
#endif

    group.writeEntry("Resize",       m_resizeChB->isChecked());
    group.writeEntry("Dimension",    m_dimensionSpB->value());
    group.writeEntry("Quality",      m_imageQualitySpB->value());

    m_history.append(m_wikiSelect->url());
    group.writeEntry("Urls history", m_history.toStringList());
}

KPImagesList* WmWidget::imagesList() const
{
    return m_imgList;
}

KPProgressWidget* WmWidget::progressBar() const
{
    return m_progressBar;
}

void WmWidget::updateLabels(const QString& name, const QString& url)
{
    QString web("http://commons.wikimedia.org");

    if (!url.isEmpty())
        web = url;

    m_headerLbl->setText(QString("<b><h2><a href='%1'>"
                                 "<font color=\"#3B5998\">%2</font>"
                                 "</a></h2></b>").arg(web).arg(i18n("Wikimedia commons")));
    if (name.isEmpty())
    {
        m_userNameDisplayLbl->clear();
    }
    else
    {
        m_userNameDisplayLbl->setText(QString::fromLatin1("<b>%1</b>").arg(name));
    }
}

void WmWidget::invertAccountLoginBox()
{
    if(m_accountBox->isHidden())
    {
        m_loginBox->hide();
        m_accountBox->show();
    }
    else
    {
        m_loginBox->show();
        m_accountBox->hide();
    }
}

void WmWidget::slotResizeChecked()
{
    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

void WmWidget::slotChangeUserClicked()
{
    emit signalChangeUserRequest();
}

void WmWidget::slotLoginClicked()
{
    emit signalLoginRequest(m_nameEdit->text(), m_passwdEdit->text(), m_wikiSelect->url());
}

QString WmWidget::author() const
{
    kDebug() << "WmWidget::author()";
    return m_authorEdit->text();
}

QString WmWidget::licence() const
{
    kDebug() << "WmWidget::licence()";
    return m_licenceComboBox->itemData(m_licenceComboBox->currentIndex()).toString();
}

} // namespace KIPIWikimediaPlugin
