/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to wikimedia commons
 *
 * Copyright (C) 2011 by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
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
#include <QProgressBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>

// KDE includes

#include <kdebug.h>
#include <KLocale>
#include <KDialog>
#include <KComboBox>
#include <KPushButton>
#include <KLineEdit>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes

#include "imageslist.h"

namespace KIPIWikiMediaPlugin
{

WmWidget::WmWidget(QWidget* parent, KIPI::Interface* iface)
    : QWidget(parent)
{
    setObjectName("WmWidget");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // -------------------------------------------------------------------

    m_imgList = new KIPIPlugins::ImagesList(iface, this);
    m_imgList->setControlButtonsPlacement(KIPIPlugins::ImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(i18n("This is the list of images to upload to your Wikimedia account."));

    m_tabBar                       = new QTabWidget(this);
    QWidget* tab1Box               = new QWidget(this);
    QWidget* tab2Box               = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(tab1Box);
    m_tabBar->addTab(tab1Box,"Account");
    m_tabBar->addTab(tab2Box,"Options");

    m_headerLbl = new QLabel(tab1Box);
    m_headerLbl->setWhatsThis(i18n("This is a clickable link to open the Wikimedia home page in a web browser."));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    // ------------------------------------------------------------------------

    m_loginBox = new QGroupBox(i18n("Login"), tab1Box);
    m_loginBox->setWhatsThis(i18n("This is the login form to your Wikimedia account."));
    QGridLayout* loginBoxLayout = new QGridLayout(m_loginBox);

    m_nameEdit   = new KLineEdit(m_loginBox);
    m_passwdEdit = new KLineEdit(m_loginBox);
    m_wikiSelect = new QComboBox(m_loginBox);
    m_passwdEdit->setEchoMode(KLineEdit::Password);

    m_wikiSelect->addItem(QString("test wikipedia"), QUrl("http://test.wikipedia.org/w/api.php"));
    m_wikiSelect->addItem(QString("en wikipedia"),   QUrl("http://en.wikipedia.org/w/api.php"));
    m_wikiSelect->addItem(QString("fr wikipedia"),   QUrl("http://fr.wikipedia.org/w/api.php"));

    QLabel* nameLabel = new QLabel(m_loginBox);
    nameLabel->setText(i18n( "Wiki Login:" ));

    QLabel* passwdLabel = new QLabel(m_loginBox);
    passwdLabel->setText(i18n("Password:"));

    QLabel* wikiLabel = new QLabel(m_loginBox);
    wikiLabel->setText(i18n("Wiki:"));

    QPushButton *loginBtn     = new QPushButton(m_loginBox);
    loginBtn->setAutoDefault(true);
    loginBtn->setDefault(true);
    loginBtn->setText(i18n("&OK"));

    loginBoxLayout->addWidget(m_nameEdit,   0, 1);
    loginBoxLayout->addWidget(m_passwdEdit, 1, 1);
    loginBoxLayout->addWidget(m_wikiSelect, 2, 1);
    loginBoxLayout->addWidget(nameLabel,    0, 0);
    loginBoxLayout->addWidget(passwdLabel,  1, 0);
    loginBoxLayout->addWidget(wikiLabel,    2, 0);
    loginBoxLayout->addWidget(loginBtn, 3, 0);
    loginBoxLayout->setObjectName("m_loginBoxLayout");

    // ------------------------------------------------------------------------

    m_accountBox                  = new QGroupBox(i18n("Account"), tab1Box);
    m_accountBox->setWhatsThis(i18n("This is the Wikimedia account that is currently logged in."));
    QGridLayout* accountBoxLayout = new QGridLayout(m_accountBox);

    QLabel *userNameLbl  = new QLabel(i18nc("Wikimedia account settings", "Account:"), m_accountBox);
    m_userNameDisplayLbl = new QLabel(m_accountBox);
    m_changeUserBtn      = new KPushButton(KGuiItem(i18n("Change Account"), "system-switch-user",
                               i18n("Logout and change Wikimedia Account used for transfer")),
                               m_accountBox);

    accountBoxLayout->addWidget(userNameLbl,            0, 0, 1, 2);
    accountBoxLayout->addWidget(m_userNameDisplayLbl,   0, 2, 1, 2);
    accountBoxLayout->addWidget(m_changeUserBtn,        2, 0, 1, 2);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    m_textBox                  = new QGroupBox(i18n("Text"), tab1Box);
    m_textBox->setWhatsThis(i18n("This is the login form to your Wikimedia account."));
    QGridLayout* textBoxLayout = new QGridLayout(m_textBox);

    QLabel * desc     = new QLabel(i18nc("Description:","Description:"), m_textBox);
    m_descriptionEdit = new QTextEdit(m_textBox);

    QLabel * aut = new QLabel(i18nc("Author:","Author:"), m_textBox);
    m_authorEdit = new KLineEdit(m_textBox);

    QLabel * licenceLabel = new QLabel(i18nc("Licence:","Licence:"), m_textBox);
    m_licenceComboBox     = new QComboBox(m_textBox);

    m_licenceComboBox->addItem(QString("Own work, multi-license with CC-BY-SA-3.0 and GFDL"), QString("{{self|cc-by-sa-3.0|GFDL|migration=redundant}}"));
    m_licenceComboBox->addItem(QString("Own work, multi-license with CC-BY-SA-3.0 and older"), QString("{{self|cc-by-sa-3.0,2.5,2.0,1.0}}"));
    m_licenceComboBox->addItem(QString("Creative Commons Attribution-Share Alike 3.0"), QString("{{self|cc-by-sa-3.0}}"));
    m_licenceComboBox->addItem(QString("Own work, Creative Commons Attribution 3.0"), QString("{{self|cc-by-3.0}}"));
    m_licenceComboBox->addItem(QString("Own work, release into public domain under the CC-Zero license"), QString("{{self|cc-zero}}"));
    m_licenceComboBox->addItem(QString("Author died more than 100 years ago"), QString("{{PD-old}}"));
    m_licenceComboBox->addItem(QString("Photo of a two-dimensional work whose author died more than 100 years ago"), QString("{{PD-art}}"));
    m_licenceComboBox->addItem(QString("First published in the United States before 1923"), QString("{{PD-US}}"));
    m_licenceComboBox->addItem(QString("Work of a U.S. government agency"), QString("{{PD-USGov}}"));
    m_licenceComboBox->addItem(QString("Simple typefaces, individual words or geometric shapes"), QString("{{PD-text}}"));
    m_licenceComboBox->addItem(QString("Logos with only simple typefaces, individual words or geometric shapes"), QString("{{PD-textlogo}}"));

    accountBoxLayout->addWidget(userNameLbl,            0, 0, 1, 2);
    accountBoxLayout->addWidget(m_userNameDisplayLbl,   0, 2, 1, 2);
    accountBoxLayout->addWidget(m_changeUserBtn,        2, 0, 1, 2);
    textBoxLayout->addWidget(desc,                      0, 0, 1, 1);
    textBoxLayout->addWidget(m_descriptionEdit,         0, 2, 1, 2);
    textBoxLayout->addWidget(aut,                       1, 0, 1, 1);
    textBoxLayout->addWidget(m_authorEdit,              1, 2, 1, 2);
    textBoxLayout->addWidget(licenceLabel,              3, 0, 1, 1);
    textBoxLayout->addWidget(m_licenceComboBox,         3, 2, 1, 2);
    textBoxLayout->setObjectName("m_textBoxLayout");
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    //------------------------------------------------------------------------------------

    QGroupBox* optionsBox         = new QGroupBox(i18n("Options"), tab2Box);
    optionsBox->setWhatsThis(i18n("These are options that will be applied to photos before upload."));
    QGridLayout* optionsBoxLayout = new QGridLayout(optionsBox);

    m_resizeChB = new QCheckBox(optionsBox);
    m_resizeChB->setText(i18n("Resize photos before uploading"));
    m_resizeChB->setChecked(false);

    m_dimensionSpB = new QSpinBox(optionsBox);
    m_dimensionSpB->setMinimum(0);
    m_dimensionSpB->setMaximum(5000);
    m_dimensionSpB->setSingleStep(10);
    m_dimensionSpB->setValue(600);
    m_dimensionSpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_dimensionSpB->setEnabled(false);
    QLabel* dimensionLbl = new QLabel(i18n("Maximum dimension:"), optionsBox);

    m_imageQualitySpB = new QSpinBox(optionsBox);
    m_imageQualitySpB->setMinimum(0);
    m_imageQualitySpB->setMaximum(100);
    m_imageQualitySpB->setSingleStep(1);
    m_imageQualitySpB->setValue(85);
    m_imageQualitySpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* imageQualityLbl = new QLabel(i18n("JPEG quality:"), optionsBox);

    optionsBoxLayout->addWidget(m_resizeChB,        0, 0, 1, 5);
    optionsBoxLayout->addWidget(imageQualityLbl,    1, 1, 1, 1);
    optionsBoxLayout->addWidget(m_imageQualitySpB,  1, 2, 1, 1);
    optionsBoxLayout->addWidget(dimensionLbl,       2, 1, 1, 1);
    optionsBoxLayout->addWidget(m_dimensionSpB,     2, 2, 1, 1);
    optionsBoxLayout->setRowStretch(3, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    m_progressBar = new QProgressBar(tab1Box);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(m_loginBox);
    settingsBoxLayout->addWidget(m_accountBox);
    settingsBoxLayout->addWidget(m_textBox);
    settingsBoxLayout->addWidget(m_progressBar);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    m_accountBox->hide();

    // ------------------------------------------------------------------------

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(m_tabBar);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    updateLabels();  // use empty labels until login

    // ------------------------------------------------------------------------

    connect(m_resizeChB, SIGNAL( clicked() ),
            this, SLOT( slotResizeChecked() ));

    connect(m_changeUserBtn, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    connect(loginBtn, SIGNAL(clicked()),
            this, SLOT(slotLoginClicked()));
}

WmWidget::~WmWidget()
{
}

KIPIPlugins::ImagesList* WmWidget::imagesList() const
{
    return m_imgList;
}

QProgressBar* WmWidget::progressBar() const
{
    return m_progressBar;
}

void WmWidget::updateLabels(const QString& name, const QString& url)
{
    QString web("http://commons.wikimedia.org");
    if (!url.isEmpty())
        web = url;

    m_headerLbl->setText(QString("<b><h2><a href='%1'>"
                                 "<font color=\"#3B5998\">Wikimedia commons</font>"
                                 "</a></h2></b>").arg(web));
    if (name.isEmpty())
    {
        m_userNameDisplayLbl->clear();
    }
    else
    {
        m_userNameDisplayLbl->setText(QString("<b>%1</b>").arg(name));
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
    emit signalLoginRequest(m_nameEdit->text(), m_passwdEdit->text(),
                            m_wikiSelect->itemData(m_wikiSelect->currentIndex()).toUrl());
}

QString WmWidget::author()
{
    kDebug() << "WmWidget::author()";
    return this->m_authorEdit->text();
}

QString WmWidget::description()
{
    kDebug() << "WmWidget::description()";
    return this->m_descriptionEdit->toPlainText();
}
QString WmWidget::licence()
{
    kDebug() << "WmWidget::licence()";
    return m_licenceComboBox->itemData(m_licenceComboBox->currentIndex()).toString();
}

} // namespace KIPIWikimediaPlugin
