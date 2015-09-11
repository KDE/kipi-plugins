/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
 * Copyright (C) 2012      by Nathan Damie <nathan dot damie at gmail dot com>
 * Copyright (C) 2012      by Iliya Ivanov <ilko2002 at abv dot bg>
 * Copyright (C) 2012-2013 by Peter Potrowl <peter dot potrowl at gmail dot com>
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

#include "wmwidget.h"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMap>
#include <QProgressBar>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QApplication>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>

// Libkipi includes

#include <KIPI/UploadWidget>
#include <KIPI/Interface>

// libKdcraw includes

#include <KDCRAW/RWidgetUtils>
#include <KDCRAW/RExpanderBox>
#include <KDCRAW/SqueezedComboBox>
#include <libkdcraw_version.h>

// Local includes

#include "kpimageinfo.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "kipiplugins_debug.h"

using namespace KIPI;
using namespace KDcrawIface;

namespace KIPIWikiMediaPlugin
{

class WmWidget::Private
{
public:

    Private()
    {
        fileBox            = 0;
        titleEdit          = 0;
        descEdit           = 0;
        dateEdit           = 0;
        longitudeEdit      = 0;
        latitudeEdit       = 0;
        categoryEdit       = 0;
        userBox            = 0;
        loginBox           = 0;
        loginHeaderLbl     = 0;
        nameEdit           = 0;
        passwdEdit         = 0;
        newWikiSv          = 0;
        newWikiNameEdit    = 0;
        newWikiUrlEdit     = 0;
        wikiSelect         = 0;
        textBox            = 0;
        authorEdit         = 0;
        sourceEdit         = 0;
        genCatEdit         = 0;
        genTxtEdit         = 0;
        genComEdit         = 0;
        accountBox         = 0;
        headerLbl          = 0;
        wikiNameDisplayLbl = 0;
        userNameDisplayLbl = 0;
        changeUserBtn      = 0;
        optionsBox         = 0;
        resizeChB          = 0;
        dimensionSpB       = 0;
        imageQualitySpB    = 0;
        removeMetaChB      = 0;
        removeGeoChB       = 0;
        licenseComboBox    = 0;
        progressBar        = 0;
        settingsExpander   = 0;
        imgList            = 0;
        uploadWidget       = 0;
        defaultMessage     = i18n("Select an image");
    }

    QWidget*                                 fileBox;
    QLineEdit*                               titleEdit;
    QTextEdit*                               descEdit;
    QLineEdit*                               dateEdit;
    QLineEdit*                               longitudeEdit;
    QLineEdit*                               latitudeEdit;
    QTextEdit*                               categoryEdit;

    RVBox*                                   userBox;
    QWidget*                                 loginBox;
    QLabel*                                  loginHeaderLbl;
    QLineEdit*                               nameEdit;
    QLineEdit*                               passwdEdit;
    QScrollArea*                             newWikiSv;
    QLineEdit*                               newWikiNameEdit;
    QLineEdit*                               newWikiUrlEdit;
    QComboBox*                               wikiSelect;

    QWidget*                                 textBox;

    QLineEdit*                               authorEdit;
    QLineEdit*                               sourceEdit;

    QTextEdit*                               genCatEdit;
    QTextEdit*                               genTxtEdit;
    QTextEdit*                               genComEdit;

    QWidget*                                 accountBox;
    QLabel*                                  headerLbl;
    QLabel*                                  wikiNameDisplayLbl;
    QLabel*                                  userNameDisplayLbl;
    QPushButton*                             changeUserBtn;

    QWidget*                                 optionsBox;
    QCheckBox*                               resizeChB;
    QSpinBox*                                dimensionSpB;
    QSpinBox*                                imageQualitySpB;
    QCheckBox*                               removeMetaChB;
    QCheckBox*                               removeGeoChB;
    SqueezedComboBox*                        licenseComboBox;

    KPProgressWidget*                        progressBar;

    RExpanderBox*                            settingsExpander;
    KPImagesList*                            imgList;
    UploadWidget*                            uploadWidget;

    QStringList                              WikisHistory;
    QStringList                              UrlsHistory;

    QString                                  defaultMessage;

    QMap <QString, QMap <QString, QString> > imagesDescInfo;
};

WmWidget::WmWidget(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    setObjectName(QLatin1String("WmWidget"));

    QVBoxLayout* const mainLayout = new QVBoxLayout(this);

    // -------------------------------------------------------------------

    d->headerLbl = new QLabel(this);
    d->headerLbl->setWhatsThis(i18n("This is a clickable link to open the MediaWiki home page in a web browser."));
    d->headerLbl->setOpenExternalLinks(true);
    d->headerLbl->setFocusPolicy(Qt::NoFocus);

    d->imgList   = new KPImagesList(this);
    d->imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    d->imgList->setAllowRAW(true);
    d->imgList->loadImagesFromCurrentSelection();
    d->imgList->listView()->setWhatsThis(i18n("This is the list of images to upload to the wiki."));

    // --------------------- Upload tab ----------------------------------

    QScrollArea* const wrapperScroll = new QScrollArea(this);
    RVBox* const wrapperPan          = new RVBox(wrapperScroll->viewport());
    wrapperScroll->setWidget(wrapperPan);
    wrapperScroll->setWidgetResizable(true);
    wrapperScroll->setVisible(false);

    QWidget* const wrapper           = new QWidget(wrapperPan);
    QHBoxLayout* const wrapperLayout = new QHBoxLayout(wrapper);

    QScrollArea* const upload = new QScrollArea(wrapper);
    RVBox* const pan          = new RVBox(upload->viewport());
    pan->setAutoFillBackground(true);

    upload->setWidget(pan);
    upload->setWidgetResizable(true);

    RVBox* const uploadBox             = new RVBox(pan);
    QWidget* const uploadPanel         = new QWidget(uploadBox);
    QVBoxLayout* const uploadBoxLayout = new QVBoxLayout(uploadPanel);

    d->fileBox = new QWidget(uploadBox);
    d->fileBox->setWhatsThis(i18n("This is the login form to your account on the chosen wiki."));
    QGridLayout* const fileBoxLayout = new QGridLayout(d->fileBox);

    loadImageInfoFirstLoad();

    d->titleEdit    = new QLineEdit(d->defaultMessage, d->fileBox);
    d->dateEdit     = new QLineEdit(d->defaultMessage, d->fileBox);

    d->descEdit     = new QTextEdit(d->fileBox);
    d->descEdit->setPlainText(d->defaultMessage);
    d->descEdit->setTabChangesFocus(1);
    d->descEdit->setAcceptRichText(false);
    d->categoryEdit = new QTextEdit(d->fileBox);
    d->categoryEdit->setPlainText(d->defaultMessage);
    d->categoryEdit->setTabChangesFocus(1);
    d->categoryEdit->setAcceptRichText(false);

    d->latitudeEdit  = new QLineEdit(d->defaultMessage, d->fileBox);
    d->longitudeEdit = new QLineEdit(d->defaultMessage, d->fileBox);

    QLabel* const titleLabel     = new QLabel(d->fileBox);
    titleLabel->setText(i18n("Title:"));
    QLabel* const dateLabel      = new QLabel(d->fileBox);
    dateLabel->setText(i18n("Date:"));
    QLabel* const descLabel      = new QLabel(d->fileBox);
    descLabel->setText(i18n("Description:"));
    QLabel* const categoryLabel  = new QLabel(d->fileBox);
    categoryLabel->setText(i18n("Categories:"));
    QLabel* const latitudeLabel  = new QLabel(d->fileBox);
    latitudeLabel->setText(i18n("Latitude:"));
    QLabel* const longitudeLabel = new QLabel(d->fileBox);
    longitudeLabel->setText(i18n("Longitude:"));

    uploadBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    uploadBoxLayout->addWidget(d->fileBox, 0, Qt::AlignTop);

    fileBoxLayout->addWidget(titleLabel,       1, 0, 1, 1);
    fileBoxLayout->addWidget(dateLabel,        2, 0, 1, 1);
    fileBoxLayout->addWidget(descLabel,        3, 0, 1, 1);
    fileBoxLayout->addWidget(categoryLabel,    4, 0, 1, 1);
    fileBoxLayout->addWidget(latitudeLabel,    5, 0, 1, 1);
    fileBoxLayout->addWidget(longitudeLabel,   6, 0, 1, 1);
    fileBoxLayout->addWidget(d->titleEdit,     1, 1, 1, 3);
    fileBoxLayout->addWidget(d->dateEdit,      2, 1, 1, 3);
    fileBoxLayout->addWidget(d->descEdit,      3, 1, 1, 3);
    fileBoxLayout->addWidget(d->categoryEdit,  4, 1, 1, 3);
    fileBoxLayout->addWidget(d->latitudeEdit,  5, 1, 1, 3);
    fileBoxLayout->addWidget(d->longitudeEdit, 6, 1, 1, 3);

    // --------------------- Config tab ----------------------------------

    QScrollArea* const config = new QScrollArea(wrapper);
    RVBox* const panel2       = new RVBox(config->viewport());
    config->setWidget(panel2);
    config->setWidgetResizable(true);

    d->settingsExpander = new RExpanderBox(panel2);
    d->settingsExpander->setObjectName(QLatin1String("MediaWiki Settings Expander"));

    d->userBox    = new RVBox(panel2);
    d->loginBox   = new QWidget(d->userBox);
    d->loginBox->setWhatsThis(i18n("This is the login form to your MediaWiki account."));
    QGridLayout* const loginBoxLayout = new QGridLayout(d->loginBox);

    d->wikiSelect                 = new QComboBox(d->loginBox);
    QPushButton* const newWikiBtn = new QPushButton(QIcon::fromTheme(QLatin1String("list-add")), i18n("New"), d->loginBox);
    newWikiBtn->setToolTip(i18n("Add a wiki to this list"));
    d->nameEdit                   = new QLineEdit(d->loginBox);
    d->passwdEdit                 = new QLineEdit(d->loginBox);
    d->passwdEdit->setEchoMode(QLineEdit::Password);

    d->wikiSelect->addItem(i18n("Wikimedia Commons"),  QLatin1String("http://commons.wikimedia.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikimedia Meta"),     QLatin1String("http://meta.wikimedia.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikipedia"),          QLatin1String("http://en.wikipedia.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikibooks"),          QLatin1String("http://en.wikibooks.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikinews"),           QLatin1String("http://en.wikinews.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikiquote"),          QLatin1String("http://en.wikiquote.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikisource"),         QLatin1String("http://en.wikinews.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wiktionary"),         QLatin1String("http://en.wiktionary.org/w/api.php"));
    d->wikiSelect->addItem(i18n("MediaWiki"),          QLatin1String("http://www.mediawiki.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikia Foto"),         QLatin1String("http://foto.wikia.com/api.php"));
    d->wikiSelect->addItem(i18n("Wikia Uncyclopedia"), QLatin1String("http://uncyclopedia.wikia.com/api.php"));

    d->wikiSelect->setEditable(false);

    QLabel* const wikiLabel = new QLabel(d->loginBox);
    wikiLabel->setText(i18n("Wiki:"));

    // --------------------- New wiki area ----------------------------------

    d->newWikiSv              = new QScrollArea(this);
    RVBox* const newWikiPanel = new RVBox(d->newWikiSv->viewport());
    d->newWikiSv->setWidget(newWikiPanel);
    d->newWikiSv->setWidgetResizable(true);
    d->newWikiSv->setVisible(false);

    QWidget* const newWikiBox        = new QWidget(newWikiPanel);
    newWikiBox->setWhatsThis(i18n("These are options for adding a Wiki."));

    QGridLayout* const newWikiLayout = new QGridLayout(newWikiBox);

    QLabel* const newWikiNameLabel   = new QLabel(newWikiPanel);
    newWikiNameLabel->setText(i18n("Name:"));


    QLabel* const newWikiUrlLabel    = new QLabel(newWikiPanel);
    newWikiUrlLabel->setText(i18n("API URL:"));

    d->newWikiNameEdit            = new QLineEdit(newWikiPanel);
    d->newWikiUrlEdit             = new QLineEdit(newWikiPanel);

    QPushButton* const addWikiBtn = new QPushButton(QIcon::fromTheme(QLatin1String("list-add")), i18n("Add"), newWikiPanel);
    addWikiBtn->setToolTip(i18n("Add a new wiki"));

    newWikiLayout->addWidget(newWikiNameLabel,   0, 0, 1, 1);
    newWikiLayout->addWidget(d->newWikiNameEdit, 0, 1, 1, 1);
    newWikiLayout->addWidget(newWikiUrlLabel,    1, 0, 1, 1);
    newWikiLayout->addWidget(d->newWikiUrlEdit,  1, 1, 1, 1);
    newWikiLayout->addWidget(addWikiBtn,         2, 1, 1, 1);

    QLabel* const nameLabel     = new QLabel(d->loginBox);
    nameLabel->setText(i18n( "Login:" ));

    QLabel* const passwdLabel   = new QLabel(d->loginBox);
    passwdLabel->setText(i18n("Password:"));

    QPushButton* const loginBtn = new QPushButton(d->loginBox);
    loginBtn->setAutoDefault(true);
    loginBtn->setDefault(true);
    loginBtn->setText(i18n("&Log in"));

    loginBoxLayout->addWidget(wikiLabel,     0, 0, 1, 1);
    loginBoxLayout->addWidget(d->wikiSelect, 0, 1, 1, 1);
    loginBoxLayout->addWidget(newWikiBtn,    0, 2, 1, 1);
    loginBoxLayout->addWidget(d->newWikiSv,  1, 1, 3, 3);
    loginBoxLayout->addWidget(nameLabel,     4, 0, 1, 1);
    loginBoxLayout->addWidget(d->nameEdit,   4, 1, 1, 1);
    loginBoxLayout->addWidget(passwdLabel,   5, 0, 1, 1);
    loginBoxLayout->addWidget(d->passwdEdit, 5, 1, 1, 1);
    loginBoxLayout->addWidget(loginBtn,      6, 0, 1, 1);
    loginBoxLayout->setObjectName(QLatin1String("loginBoxLayout"));

    d->accountBox                       = new QWidget(d->userBox);
    QGridLayout* const accountBoxLayout = new QGridLayout(d->accountBox);

    QLabel* const wikiNameLbl = new QLabel(d->accountBox);
    wikiNameLbl->setText(i18nc("Name of the wiki the user is currently logged on", "Logged on: "));
    d->wikiNameDisplayLbl     = new QLabel(d->accountBox);

    QLabel* const userNameLbl = new QLabel(d->accountBox);
    userNameLbl->setText(i18nc("Username which is used to connect to the wiki", "Logged as: "));
    d->userNameDisplayLbl     = new QLabel(d->accountBox);

    d->changeUserBtn          = new QPushButton(QIcon::fromTheme(QLatin1String("system-switch-user")), i18n("Change Account"), d->accountBox);
    d->changeUserBtn->setToolTip(i18n("Logout and change the account used for transfer"));

    accountBoxLayout->addWidget(wikiNameLbl,           0, 0, 1, 1);
    accountBoxLayout->addWidget(d->wikiNameDisplayLbl, 0, 1, 1, 1);
    accountBoxLayout->addWidget(userNameLbl,           1, 0, 1, 1);
    accountBoxLayout->addWidget(d->userNameDisplayLbl, 1, 1, 1, 1);
    accountBoxLayout->addWidget(d->changeUserBtn,      2, 0, 1, 2);
    d->accountBox->hide();

    d->settingsExpander->addItem(d->userBox, i18n("Account"), QLatin1String("account"), true);
    d->settingsExpander->setItemIcon(0, QIcon::fromTheme(QLatin1String("user-properties")));

    // --------------------- Login area ----------------------------------

    d->textBox                       = new QWidget(panel2);
    d->textBox->setWhatsThis(i18n("This is the login form to your account on the chosen wiki."));
    QGridLayout* const textBoxLayout = new QGridLayout(d->textBox);

    QLabel* const authorLbl  = new QLabel(i18n("Author:"), d->textBox);
    d->authorEdit            = new QLineEdit(d->textBox);

    QLabel* const sourceLbl  = new QLabel(i18n("Source:"), d->textBox);
    d->sourceEdit            = new QLineEdit(d->textBox);

    QLabel* const licenseLbl = new QLabel(i18n("License:"), d->textBox);
    d->licenseComboBox       = new SqueezedComboBox(d->textBox);

    d->licenseComboBox->addSqueezedItem(i18n("Own work, multi-license with CC-BY-SA-3.0 and GFDL"), 
                                       QLatin1String("{{self|cc-by-sa-3.0|GFDL|migration=redundant}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Own work, multi-license with CC-BY-SA-3.0 and older"), 
                                       QLatin1String("{{self|cc-by-sa-3.0,2.5,2.0,1.0}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Creative Commons Attribution-Share Alike 3.0"),
                                       QLatin1String("{{self|cc-by-sa-3.0}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Own work, Creative Commons Attribution 3.0"),
                                       QLatin1String("{{self|cc-by-3.0}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Own work, release into public domain under the CC-Zero license"),
                                       QLatin1String("{{self|cc-zero}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Author died more than 100 years ago"),
                                       QLatin1String("{{PD-old}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Photo of a two-dimensional work whose author died more than 100 years ago"),
                                       QLatin1String("{{PD-art}}"));
    d->licenseComboBox->addSqueezedItem(i18n("First published in the United States before 1923"),
                                       QLatin1String("{{PD-US}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Work of a U.S. government agency"),
                                       QLatin1String("{{PD-USGov}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Simple typefaces, individual words or geometric shapes"),
                                       QLatin1String("{{PD-text}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Logos with only simple typefaces, individual words or geometric shapes"),
                                       QLatin1String("{{PD-textlogo}}"));
    d->licenseComboBox->addSqueezedItem(i18n("No license specified (not recommended for public wiki sites)"),
                                       QLatin1String(""));

    QLabel* const genCatLbl = new QLabel(i18n("Generic categories:"), d->textBox);
    d->genCatEdit           = new QTextEdit(d->textBox);
    d->genCatEdit->setTabChangesFocus(1);
    d->genCatEdit->setWhatsThis(i18n("This is a place to enter categories that will be added to all the files."));
    d->genCatEdit->setAcceptRichText(false);

    QLabel* const genTxtLbl = new QLabel(i18n("Generic text:"), d->textBox);
    d->genTxtEdit           = new QTextEdit(d->textBox);
    d->genTxtEdit->setTabChangesFocus(1);
    d->genTxtEdit->setWhatsThis(i18n("This is a place to enter text that will be added to all the files, "
                                     "below the Information template."));
    d->genTxtEdit->setAcceptRichText(false);

    QLabel* const genComLbl = new QLabel(i18n("Upload comments:"), d->textBox);
    d->genComEdit           = new QTextEdit(d->textBox);
    d->genComEdit->setTabChangesFocus(1);
    d->genComEdit->setWhatsThis(i18n("This is a place to enter text that will be used as upload comments. "
                                     "The default of 'Uploaded via KIPI uploader' will be used if empty."));
    d->genComEdit->setAcceptRichText(false);

    textBoxLayout->addWidget(authorLbl,          1, 0, 1, 1);
    textBoxLayout->addWidget(sourceLbl,          2, 0, 1, 1);
    textBoxLayout->addWidget(licenseLbl,         3, 0, 1, 1);
    textBoxLayout->addWidget(genCatLbl,          4, 0, 1, 1);
    textBoxLayout->addWidget(genTxtLbl,          5, 0, 1, 1);
    textBoxLayout->addWidget(genComLbl,          6, 0, 1, 1);

    textBoxLayout->addWidget(d->authorEdit,      1, 2, 1, 2);
    textBoxLayout->addWidget(d->sourceEdit,      2, 2, 1, 2);
    textBoxLayout->addWidget(d->licenseComboBox, 3, 2, 1, 2);
    textBoxLayout->addWidget(d->genCatEdit,      4, 2, 1, 2);
    textBoxLayout->addWidget(d->genTxtEdit,      5, 2, 1, 2);
    textBoxLayout->addWidget(d->genComEdit,      6, 2, 1, 2);
    textBoxLayout->setObjectName(QLatin1String("textBoxLayout"));

    d->settingsExpander->addItem(d->textBox, i18n("Information"), QLatin1String("information"), true);
    d->settingsExpander->setItemIcon(1, QIcon::fromTheme(QLatin1String("document-properties")));

    // --------------------- Options area ----------------------------------

    d->optionsBox                       = new QWidget(panel2);
    d->optionsBox->setWhatsThis(i18n("These are options that will be applied to photos before upload."));
    QGridLayout* const optionsBoxLayout = new QGridLayout(d->optionsBox);

    d->resizeChB = new QCheckBox(d->optionsBox);
    d->resizeChB->setText(i18n("Resize photos before uploading"));
    d->resizeChB->setChecked(false);

    d->dimensionSpB            = new QSpinBox(d->optionsBox);
    d->dimensionSpB->setMinimum(0);
    d->dimensionSpB->setMaximum(10000);
    d->dimensionSpB->setSingleStep(10);
    d->dimensionSpB->setValue(1600);
    d->dimensionSpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->dimensionSpB->setEnabled(false);
    QLabel* const dimensionLbl = new QLabel(i18n("Maximum size:"), d->optionsBox);

    d->imageQualitySpB            = new QSpinBox(d->optionsBox);
    d->imageQualitySpB->setMinimum(0);
    d->imageQualitySpB->setMaximum(100);
    d->imageQualitySpB->setSingleStep(1);
    d->imageQualitySpB->setValue(85);
    d->imageQualitySpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* const imageQualityLbl = new QLabel(i18n("JPEG quality:"), d->optionsBox);

    d->removeMetaChB = new QCheckBox(d->optionsBox);
    d->removeMetaChB->setText(i18n("Remove metadata from file"));
    d->removeMetaChB->setChecked(false);

    d->removeGeoChB = new QCheckBox(d->optionsBox);
    d->removeGeoChB->setText(i18n("Remove coordinates from file"));
    d->removeGeoChB->setChecked(false);

    optionsBoxLayout->addWidget(d->resizeChB,       0, 0, 1, 2);
    optionsBoxLayout->addWidget(dimensionLbl,       1, 0, 1, 1);
    optionsBoxLayout->addWidget(imageQualityLbl,    2, 0, 1, 1);
    optionsBoxLayout->addWidget(d->dimensionSpB,    1, 1, 1, 1);
    optionsBoxLayout->addWidget(d->imageQualitySpB, 2, 1, 1, 1);
    optionsBoxLayout->addWidget(d->removeMetaChB,   3, 0, 1, 2);
    optionsBoxLayout->addWidget(d->removeGeoChB,    4, 0, 1, 2);
    optionsBoxLayout->setRowStretch(3, 10);
    optionsBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    optionsBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    d->settingsExpander->addItem(d->optionsBox, i18n("Options"), QLatin1String("options"), true);
    d->settingsExpander->setItemIcon(2, QIcon::fromTheme(QLatin1String("system-run")));

    // ------------------------------------------------------------------------

    QTabWidget* const tabWidget = new QTabWidget;
    tabWidget->addTab(upload, i18n("Items Properties"));
    tabWidget->addTab(config, i18n("Upload Settings"));
    tabWidget->setMinimumWidth(350);

    // ------------------------------------------------------------------------

    d->progressBar = new KPProgressWidget(this);
    d->progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    d->progressBar->hide();

    // ------------------------------------------------------------------------

    wrapperLayout->addWidget(d->imgList);
    wrapperLayout->addWidget(tabWidget);
    wrapperLayout->setStretch(0, 10);
    wrapperLayout->setStretch(1, 5);

    mainLayout->addWidget(d->headerLbl);
    mainLayout->addWidget(wrapper);
    mainLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    mainLayout->addWidget(d->progressBar);
    mainLayout->setMargin(0);

    updateLabels();  // use empty labels until login

    // --------------------- Slots definition ----------------------------------

    connect(d->resizeChB, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

    connect(d->removeMetaChB, SIGNAL(clicked()),
            this, SLOT(slotRemoveMetaChecked()));

    connect(d->changeUserBtn, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    connect(loginBtn, SIGNAL(clicked()),
            this, SLOT(slotLoginClicked()));

    connect(newWikiBtn, SIGNAL(clicked()),
            this, SLOT(slotNewWikiClicked()));

    connect(addWikiBtn, SIGNAL(clicked()),
            this, SLOT(slotAddWikiClicked()));

    connect(d->titleEdit, SIGNAL(editingFinished()),
            this, SLOT(slotRestoreExtension()));

    connect(d->titleEdit, SIGNAL(textEdited(QString)),
            this, SLOT(slotApplyTitle()));

    connect(d->dateEdit, SIGNAL(textEdited(QString)),
            this, SLOT(slotApplyDate()));

    // Problem: textChanged() is also called when the text is changed by setText()
    // textEdited() would be better, but QTextEdit does not have this.
    // Solution for the moment: we do not setText() when the selection changes
    connect(d->categoryEdit, SIGNAL(textChanged()),
            this, SLOT(slotApplyCategories()));

    // Problem: textChanged() is also called when the text is changed by setText()
    // textEdited() would be better, but QTextEdit does not have this.
    // Solution for the moment: we do not setText() when the selection changes
    connect(d->descEdit, SIGNAL(textChanged()),
            this, SLOT(slotApplyDescription()));

    connect(d->latitudeEdit, SIGNAL(textEdited(QString)),
            this, SLOT(slotApplyLatitude()));

    connect(d->longitudeEdit, SIGNAL(textEdited(QString)),
            this, SLOT(slotApplyLongitude()));

    connect(d->imgList, SIGNAL(signalItemClicked(QTreeWidgetItem*)),
            this, SLOT(slotLoadImagesDesc(QTreeWidgetItem*)));

    connect(d->imgList, SIGNAL(signalRemovedItems(QList<QUrl>)),
            this, SLOT(slotRemoveImagesDesc(QList<QUrl>)));
}

WmWidget::~WmWidget()
{
    delete d;
}

void WmWidget::readSettings(KConfigGroup& group)
{
    qCDebug(KIPIPLUGINS_LOG) <<  "Read settings from" << group.name();

#if KDCRAW_VERSION >= 0x020000
    d->settingsExpander->readSettings(group);
#else
    d->settingsExpander->readSettings();
#endif

    d->authorEdit->setText(group.readEntry("Author",           ""));
    d->sourceEdit->setText(group.readEntry("Source",           "{{own}}"));

    d->genCatEdit->setText(group.readEntry("genCategories",    "Uploaded with KIPI uploader"));
    d->genTxtEdit->setText(group.readEntry("genText",          ""));

    d->genComEdit->setText(group.readEntry("Comments",         "Uploaded with KIPI uploader"));
    d->resizeChB->setChecked(group.readEntry("Resize",         false));
    d->dimensionSpB->setValue(group.readEntry("Dimension",     1600));
    d->imageQualitySpB->setValue(group.readEntry("Quality",    85));
    d->removeMetaChB->setChecked(group.readEntry("RemoveMeta", false));
    d->removeGeoChB->setChecked(group.readEntry("RemoveGeo",   false));
    slotResizeChecked();
    slotRemoveMetaChecked();

    d->WikisHistory = group.readEntry("Wikis history",         QStringList());
    d->UrlsHistory  = group.readEntry("Urls history",          QStringList());

    qCDebug(KIPIPLUGINS_LOG) << "UrlHistory.size: " << d->UrlsHistory.size() << "; WikisHistory.size:" << d->WikisHistory.size();

    for(int i = 0 ; i < d->UrlsHistory.size() && i < d->WikisHistory.size() ; i++)
    {
        d->wikiSelect->addItem(d->WikisHistory.at(i), d->UrlsHistory.at(i));
    }
}

void WmWidget::saveSettings(KConfigGroup& group)
{
    qCDebug(KIPIPLUGINS_LOG) << "Save settings to" << group.name();

#if KDCRAW_VERSION >= 0x020000
    d->settingsExpander->writeSettings(group);
#else
    d->settingsExpander->writeSettings();
#endif

    group.writeEntry("Author",        d->authorEdit->text());
    group.writeEntry("Source",        d->sourceEdit->text());

    group.writeEntry("genCategories", d->genCatEdit->toPlainText());
    group.writeEntry("genText",       d->genTxtEdit->toPlainText());
    group.writeEntry("Comments",      d->genComEdit->toPlainText());

    group.writeEntry("Resize",        d->resizeChB->isChecked());
    group.writeEntry("Dimension",     d->dimensionSpB->value());
    group.writeEntry("Quality",       d->imageQualitySpB->value());

    group.writeEntry("RemoveMeta",    d->removeMetaChB->isChecked());
    group.writeEntry("RemoveGeo",     d->removeGeoChB->isChecked());
}

KPImagesList* WmWidget::imagesList() const
{
    return d->imgList;
}

KPProgressWidget* WmWidget::progressBar() const
{
    return d->progressBar;
}

void WmWidget::updateLabels(const QString& userName, const QString& wikiName, const QString& url)
{
    QString web = QLatin1String("http://www.mediawiki.org");

    if (url.isEmpty())
    {
        d->wikiNameDisplayLbl->clear();
    }
    else
    {
        web = url;
        d->wikiNameDisplayLbl->setText(QString::fromLatin1("<b>%1</b>").arg(wikiName));
    }

    d->headerLbl->setText(QString::fromUtf8("<h2><b><a href='%1'><font color=\"#3B5998\">%2</font></a></b></h2>").arg(web).arg(wikiName));

    if (userName.isEmpty())
    {
        d->userNameDisplayLbl->clear();
    }
    else
    {
        d->userNameDisplayLbl->setText(QString::fromLatin1("<b>%1</b>").arg(userName));
    }
}

void WmWidget::invertAccountLoginBox()
{
    if (d->accountBox->isHidden())
    {
        d->loginBox->hide();
        d->accountBox->show();
    }
    else
    {
        d->loginBox->show();
        d->accountBox->hide();
    }
}

void WmWidget::slotResizeChecked()
{
    d->dimensionSpB->setEnabled(d->resizeChB->isChecked());
    d->imageQualitySpB->setEnabled(d->resizeChB->isChecked());
}

void WmWidget::slotRemoveMetaChecked()
{
    d->removeGeoChB->setEnabled(!d->removeMetaChB->isChecked());
    d->removeGeoChB->setChecked(d->removeMetaChB->isChecked());
}

void WmWidget::slotChangeUserClicked()
{
    emit signalChangeUserRequest();
}

void WmWidget::slotLoginClicked()
{
     emit signalLoginRequest(d->nameEdit->text(), d->passwdEdit->text(),
                             d->wikiSelect->itemText(d->wikiSelect->currentIndex()),
                             d->wikiSelect->itemData(d->wikiSelect->currentIndex()).toUrl());
}

void WmWidget::slotNewWikiClicked()
{
    if (d->newWikiSv->isVisible())
    {
        d->newWikiSv->setVisible(false);
    }
    else
    {
        d->newWikiSv->setVisible(true);
    }
}

void WmWidget::slotAddWikiClicked()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group(QLatin1String("MediaWiki export settings"));

    d->UrlsHistory << d->newWikiUrlEdit->text();
    group.writeEntry(QLatin1String("Urls history"), d->UrlsHistory);

    d->WikisHistory << d->newWikiNameEdit->text();
    group.writeEntry(QLatin1String("Wikis history"), d->WikisHistory);

    d->wikiSelect->addItem(d->newWikiNameEdit->text(), d->newWikiUrlEdit->text());
    d->wikiSelect->setCurrentIndex(d->wikiSelect->count()-1);

    slotNewWikiClicked();
}

void WmWidget::loadImageInfoFirstLoad()
{
    QList<QUrl> urls = d->imgList->imageUrls(false);

    d->imagesDescInfo.clear();

    for (int j = 0; j < urls.size(); j++)
    {
        loadImageInfo(urls.at(j));
    }
}

void WmWidget::loadImageInfo(const QUrl& url)
{
    KPImageInfo info(url);
    QStringList keywar        = info.keywords();
    QString date              = info.date().toString(Qt::ISODate).replace(QLatin1String("T"), QLatin1String(" "), Qt::CaseSensitive);
    QString title             = info.name();
    QString description       = info.title();
    QString currentCategories;
    QString latitude;
    QString longitude;

    for (int i = 0; i < keywar.size(); i++) 
    {
        if (i == keywar.size() - 1)
        {
            currentCategories.append(keywar.at(i));
        }
        else
        {
            currentCategories.append(keywar.at(i)).append(QLatin1String("\n"));
        }
    }

    if (info.hasLatitude())
    {
        latitude = QString::number(info.latitude(), 'f', 9);
    }

    if (info.hasLongitude())
    {
        longitude = QString::number(info.longitude(), 'f', 9);
    }

    QMap<QString, QString> imageMetaData;
    imageMetaData[QLatin1String("title")]       = title;
    imageMetaData[QLatin1String("date")]        = date;
    imageMetaData[QLatin1String("categories")]  = currentCategories;
    imageMetaData[QLatin1String("description")] = description;
    imageMetaData[QLatin1String("latitude")]    = latitude;
    imageMetaData[QLatin1String("longitude")]   = longitude;
    d->imagesDescInfo.insert(url.path(), imageMetaData);
}

void WmWidget::clearEditFields()
{
    d->titleEdit->setText(d->defaultMessage);
    d->dateEdit->setText(d->defaultMessage);
    d->descEdit->setText(d->defaultMessage);
    d->categoryEdit->setText(d->defaultMessage);
    d->latitudeEdit->setText(d->defaultMessage);
    d->longitudeEdit->setText(d->defaultMessage);
}

void WmWidget::slotLoadImagesDesc(QTreeWidgetItem* item)
{
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();
    KPImagesListViewItem* const l_item    = dynamic_cast<KPImagesListViewItem*>(item);

    if (!l_item)
        return;

    QMap<QString, QString> imageMetaData;

    if (!d->imagesDescInfo.contains(l_item->url().path())) 
    {
        loadImageInfo(l_item->url());
    }

    imageMetaData = d->imagesDescInfo[l_item->url().path()];

    d->titleEdit->setText(imageMetaData[QLatin1String("title")]);
    d->dateEdit->setText(imageMetaData[QLatin1String("date")].replace(QLatin1String("T"), QLatin1String(" "), Qt::CaseSensitive));
    d->latitudeEdit->setText(imageMetaData[QLatin1String("latitude")]);
    d->longitudeEdit->setText(imageMetaData[QLatin1String("longitude")]);

    if (selectedItems.size() == 1)
    {
        d->categoryEdit->setText(imageMetaData[QLatin1String("categories")]);
        d->descEdit->setText(imageMetaData[QLatin1String("description")]);
    }
}

void WmWidget::slotRemoveImagesDesc(const QList<QUrl> urls)
{
    for (QList<QUrl>::const_iterator it = urls.begin(); it != urls.end(); ++it)
    {
        QString path = (*it).path();
        d->imagesDescInfo.remove(path);
        qCDebug(KIPIPLUGINS_LOG) << "Remove" << path << "; new length:" << d->imagesDescInfo.size();
    }
}

void WmWidget::slotRestoreExtension()
{
    qCDebug(KIPIPLUGINS_LOG) << "RestoreExtension";

    QString imageTitle;
    QString originalExtension;
    QString currentExtension;
    QList<QUrl> urls;
    QMap<QString, QString> imageMetaData;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    // Build the list of items to rename
    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* const l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));

        if (l_item)
            urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        imageMetaData = d->imagesDescInfo[urls.at(i).path()];
        imageTitle    = imageMetaData[QLatin1String("title")];

        // Add original extension if removed
        currentExtension  = imageTitle.split(QLatin1Char('.')).last();
        originalExtension = urls.at(i).path().split(QLatin1Char('.')).last();

        if (QString::compare(currentExtension, originalExtension, Qt::CaseInsensitive) != 0)
        {
            imageTitle.append(QLatin1String(".")).append(originalExtension);
            d->titleEdit->setText(imageTitle);
        }

        qCDebug(KIPIPLUGINS_LOG) << urls.at(i).path() << "renamed to" << imageTitle;
        imageMetaData[QLatin1String("title")] = imageTitle;
        d->imagesDescInfo[urls.at(i).path()]  = imageMetaData;
    }
}

void WmWidget::slotApplyTitle()
{
    qCDebug(KIPIPLUGINS_LOG) << "ApplyTitle";

    QString givenTitle = title();
    QString imageTitle;
    QString number;
    QList<QUrl> urls;
    QMap<QString, QString> imageMetaData;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();
    QStringList parts;

    const int minLength = givenTitle.count(QLatin1String("#"));

    // Build the list of items to rename
    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* const l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));

        if (l_item)
            urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        imageMetaData = d->imagesDescInfo[urls.at(i).path()];
        imageTitle    = givenTitle;

        // If there is at least one #, replace it the correct number
        if (minLength > 0)
        {
            parts      = imageTitle.split(QLatin1String("#"), QString::KeepEmptyParts);
            imageTitle = parts.first().append(QLatin1String("#")).append(parts.last());
            number     = QString::number(i + 1);

            while (number.length() < minLength)
            {
                number.prepend(QLatin1String("0"));
            }

            imageTitle.replace(imageTitle.indexOf(QLatin1String("#")), 1, number);
        }

        qCDebug(KIPIPLUGINS_LOG) << urls.at(i).path() << "renamed to" << imageTitle;
        imageMetaData[QLatin1String("title")] = imageTitle;
        d->imagesDescInfo[urls.at(i).path()]  = imageMetaData;
    }
}

void WmWidget::slotApplyDate()
{
    QList<QUrl> urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* const l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));

        if (l_item)
            urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                        = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData[QLatin1String("date")] = date();
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }
}

void WmWidget::slotApplyCategories()
{
    QList<QUrl> urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* const l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));

        if (l_item)
            urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                              = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData[QLatin1String("categories")] = categories();
        d->imagesDescInfo[urls.at(i).path()]       = imageMetaData;
    }
}

void WmWidget::slotApplyDescription()
{
    QList<QUrl> urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* const l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));

        if (l_item)
            urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                               = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData[QLatin1String("description")] = description();
        d->imagesDescInfo[urls.at(i).path()]        = imageMetaData;
    }
}

void WmWidget::slotApplyLatitude()
{

    QList<QUrl> urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* const l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));

        if (l_item)
            urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                            = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData[QLatin1String("latitude")] = latitude();
        d->imagesDescInfo[urls.at(i).path()]     = imageMetaData;
    }
}

void WmWidget::slotApplyLongitude()
{
    QList<QUrl> urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* const l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));

        if (l_item)
            urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                             = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData[QLatin1String("longitude")] = longitude();
        d->imagesDescInfo[urls.at(i).path()]      = imageMetaData;
    }
}

QMap <QString,QMap <QString,QString> > WmWidget::allImagesDesc()
{
    QList<QUrl> urls = d->imgList->imageUrls(false);

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData          = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData[QLatin1String("author")]        = author();
        imageMetaData[QLatin1String("source")]        = source();
        imageMetaData[QLatin1String("license")]       = license();
        imageMetaData[QLatin1String("genCategories")] = genCategories();
        imageMetaData[QLatin1String("genText")]       = genText();
        imageMetaData[QLatin1String("comments")]      = genComments();
        d->imagesDescInfo[urls.at(i).path()]          = imageMetaData;
    }

    return d->imagesDescInfo;
}

QString WmWidget::author() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::author()";
    return d->authorEdit->text();
}

QString WmWidget::source() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::source()";
    return d->sourceEdit->text();
}

QString WmWidget::genCategories() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::genCategories()";
    return d->genCatEdit->toPlainText();
}

QString WmWidget::genText() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::genText()";
    return d->genTxtEdit->toPlainText();
}

QString WmWidget::genComments() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::genComments()";
    return d->genComEdit->toPlainText();
}

int WmWidget::quality() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::quality()";
    return d->imageQualitySpB->value();
}

int WmWidget::dimension() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::dimension()";
    return d->dimensionSpB->value();
}

bool WmWidget::resize() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::resize()";
    return d->resizeChB->isChecked();
}

bool WmWidget::removeMeta() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::removeMeta()";
    return d->removeMetaChB->isChecked();
}

bool WmWidget::removeGeo() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::removeGeo()";
    return d->removeGeoChB->isChecked();
}

QString WmWidget::license() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::license()";
    return d->licenseComboBox->itemData(d->licenseComboBox->currentIndex()).toString();
}

QString WmWidget::title() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::title()";
    return d->titleEdit->text();
}

QString WmWidget::categories() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::categories()";
    return d->categoryEdit->toPlainText();
}

QString WmWidget::description() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::description()";
    return d->descEdit->toPlainText();
}

QString WmWidget::date() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::date()";
    return d->dateEdit->text();
}

QString WmWidget::latitude() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::latitude()";
    return d->latitudeEdit->text();
}

QString WmWidget::longitude() const
{
    qCDebug(KIPIPLUGINS_LOG) << "WmWidget::longitude()";
    return d->longitudeEdit->text();
}

} // namespace KIPIWikimediaPlugin
