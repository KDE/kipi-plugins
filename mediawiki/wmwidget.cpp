/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
 * Copyright (C) 2012      by Nathan Damie <nathan dot damie at gmail dot com>
 * Copyright (C) 2012      by Iliya Ivanov <ilko2002 at abv dot bg>
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

#include "wmwidget.moc"

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

// KDE includes

#include <kcombobox.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktextedit.h>
#include <kvbox.h>

// LibKIPI includes

#include <libkipi/uploadwidget.h>
#include <libkipi/interface.h>

// libKdcraw includes

#include <libkdcraw/rexpanderbox.h>
#include <libkdcraw/version.h>
#include <libkdcraw/squeezedcombobox.h>

// Local includes

#include "kpimageinfo.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "kpimageinfo.h"
#include "wmwidget.h"

using namespace KIPI;
using namespace KDcrawIface;

namespace KIPIWikiMediaPlugin
{

class WmWidget::Private
{
public:

    Private()
    {
        uploadBox          = 0;
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
        accountBox         = 0;
        headerLbl          = 0;
        userNameDisplayLbl = 0;
        changeUserBtn      = 0;
        optionsBox         = 0;
        resizeChB          = 0;
        dimensionSpB       = 0;
        imageQualitySpB    = 0;
        licenseComboBox    = 0;
        progressBar        = 0;
        settingsExpander   = 0;
        imgList            = 0;
        uploadWidget       = 0;
        defaultMessage     = i18n("Select an image");
    }

    KHBox*                                   uploadBox;
    QWidget*                                 fileBox;
    KLineEdit*                               titleEdit;
    KTextEdit*                               descEdit;
    KLineEdit*                               dateEdit;
    KLineEdit*                               longitudeEdit;
    KLineEdit*                               latitudeEdit;
    KTextEdit*                               categoryEdit;

    KVBox*                                   userBox;
    QWidget*                                 loginBox;
    QLabel*                                  loginHeaderLbl;
    KLineEdit*                               nameEdit;
    KLineEdit*                               passwdEdit;
    QScrollArea*                             newWikiSv;
    KLineEdit*                               newWikiNameEdit;
    KLineEdit*                               newWikiUrlEdit;
    QComboBox*                               wikiSelect;

    QWidget*                                 textBox;

    KLineEdit*                               authorEdit;
    KLineEdit*                               sourceEdit;

    KTextEdit*                               genCatEdit;
    KTextEdit*                               genTxtEdit;

    KHBox*                                   accountBox;
    QLabel*                                  headerLbl;
    QLabel*                                  userNameDisplayLbl;
    KPushButton*                             changeUserBtn;

    QWidget*                                 optionsBox;
    QCheckBox*                               resizeChB;
    QSpinBox*                                dimensionSpB;
    QSpinBox*                                imageQualitySpB;
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
    : QWidget(parent), d(new Private)
{
    setObjectName("WmWidget");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

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

    QScrollArea* wrapperScroll = new QScrollArea(this);
    KVBox* wrapperPan          = new KVBox(wrapperScroll->viewport());
    wrapperPan->setAutoFillBackground(false);
    wrapperScroll->setWidget(wrapperPan);
    wrapperScroll->setWidgetResizable(true);
    wrapperScroll->setAutoFillBackground(false);
    wrapperScroll->viewport()->setAutoFillBackground(false);
    wrapperScroll->setVisible(false);

    QWidget* wrapper           = new QWidget(wrapperPan);
    QHBoxLayout* wrapperLayout = new QHBoxLayout(wrapper);

    QScrollArea* upload = new QScrollArea(wrapper);
    KVBox* pan          = new KVBox(upload->viewport());
    pan->setAutoFillBackground(true);

    upload->setWidget(pan);
    upload->setWidgetResizable(true);
    upload->setAutoFillBackground(false);
    upload->viewport()->setAutoFillBackground(false);

    KVBox* uploadBox             = new KVBox(pan);
    QWidget* uploadPanel         = new QWidget(uploadBox);
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadPanel);

    d->fileBox = new QWidget(uploadBox);
    d->fileBox->setWhatsThis(i18n("This is the login form to your account on the chosen wiki."));
    QGridLayout* fileBoxLayout = new QGridLayout(d->fileBox);

    loadImageInfoFirstLoad();

    d->titleEdit     = new KLineEdit(d->defaultMessage, d->fileBox);
    d->dateEdit      = new KLineEdit(d->defaultMessage, d->fileBox);

    d->descEdit     = new KTextEdit(d->fileBox);
    d->descEdit->setPlainText(d->defaultMessage);
    d->descEdit->setTabChangesFocus(1);
    d->descEdit->setAcceptRichText(false);
    d->categoryEdit = new KTextEdit(d->fileBox);
    d->categoryEdit->setPlainText(d->defaultMessage);
    d->categoryEdit->setTabChangesFocus(1);
    d->categoryEdit->setAcceptRichText(false);

    d->latitudeEdit  = new KLineEdit(d->defaultMessage, d->fileBox);
    d->longitudeEdit = new KLineEdit(d->defaultMessage, d->fileBox);

    QLabel* titleLabel     = new QLabel(d->fileBox);
    titleLabel->setText(i18n("Title:"));
    QLabel* dateLabel      = new QLabel(d->fileBox);
    dateLabel->setText(i18n("Date:"));
    QLabel* descLabel      = new QLabel(d->fileBox);
    descLabel->setText(i18n("Description:"));
    QLabel* categoryLabel  = new QLabel(d->fileBox);
    categoryLabel->setText(i18n("Categories:"));
    QLabel* latitudeLabel  = new QLabel(d->fileBox);
    latitudeLabel->setText(i18n("Latitude:"));
    QLabel* longitudeLabel = new QLabel(d->fileBox);
    longitudeLabel->setText(i18n("Longitude:"));

    uploadBoxLayout->setSpacing(KDialog::spacingHint());
    uploadBoxLayout->addWidget(d->fileBox, 0, Qt::AlignTop);

    fileBoxLayout->addWidget(titleLabel,       1, 0,1,1);
    fileBoxLayout->addWidget(dateLabel,        2, 0,1,1);
    fileBoxLayout->addWidget(descLabel,        3, 0,1,1);
    fileBoxLayout->addWidget(categoryLabel,    4, 0,1,1);
    fileBoxLayout->addWidget(latitudeLabel,    5, 0,1,1);
    fileBoxLayout->addWidget(longitudeLabel,   6, 0,1,1);
    fileBoxLayout->addWidget(d->titleEdit,     1, 1,1,3);
    fileBoxLayout->addWidget(d->dateEdit,      2, 1,1,3);
    fileBoxLayout->addWidget(d->descEdit,      3, 1,1,3);
    fileBoxLayout->addWidget(d->categoryEdit,  4, 1,1,3);
    fileBoxLayout->addWidget(d->latitudeEdit,  5, 1,1,3);
    fileBoxLayout->addWidget(d->longitudeEdit, 6, 1,1,3);

    // --------------------- Config tab ----------------------------------

    QScrollArea* config = new QScrollArea(wrapper);
    KVBox* panel2       = new KVBox(config->viewport());
    panel2->setAutoFillBackground(false);
    config->setWidget(panel2);
    config->setWidgetResizable(true);
    config->setAutoFillBackground(false);
    config->viewport()->setAutoFillBackground(false);

    d->settingsExpander = new RExpanderBox(panel2);
    d->settingsExpander->setObjectName("MediaWiki Settings Expander");

    d->userBox    = new KVBox(panel2);
    d->loginBox   = new QWidget(d->userBox);
    d->loginBox->setWhatsThis(i18n("This is the login form to your MediaWiki account."));
    QGridLayout* loginBoxLayout = new QGridLayout(d->loginBox);

    d->wikiSelect = new QComboBox(d->loginBox);
    KPushButton* newWikiBtn = new KPushButton(KGuiItem(i18n("New"), "list-add", i18n("Add a wiki to this list")), d->loginBox);
    d->nameEdit   = new KLineEdit(d->loginBox);
    d->passwdEdit = new KLineEdit(d->loginBox);
    d->passwdEdit->setEchoMode(KLineEdit::Password);

    d->wikiSelect->addItem(i18n("Wikimedia Commons"),  QString("http://commons.wikimedia.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikimedia Meta"),     QString("http://meta.wikimedia.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikipedia"),          QString("http://en.wikipedia.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikibooks"),          QString("http://en.wikibooks.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikinews"),           QString("http://en.wikinews.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikiquote"),          QString("http://en.wikiquote.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikisource"),         QString("http://en.wikinews.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wiktionary"),         QString("http://en.wiktionary.org/w/api.php"));
    d->wikiSelect->addItem(i18n("MediaWiki"),          QString("http://www.mediawiki.org/w/api.php"));
    d->wikiSelect->addItem(i18n("Wikia Foto"),         QString("http://foto.wikia.com/api.php"));
    d->wikiSelect->addItem(i18n("Wikia Uncyclopedia"), QString("http://uncyclopedia.wikia.com/api.php"));

    d->wikiSelect->setEditable(false);

    QLabel* wikiLabel = new QLabel(d->loginBox);
    wikiLabel->setText(i18n("Wiki:"));

    // --------------------- New wiki area ----------------------------------

    d->newWikiSv = new QScrollArea(this);
    KVBox* newWikiPanel = new KVBox(d->newWikiSv->viewport());
    newWikiPanel->setAutoFillBackground(false);
    d->newWikiSv->setWidget(newWikiPanel);
    d->newWikiSv->setWidgetResizable(true);
    d->newWikiSv->setAutoFillBackground(false);
    d->newWikiSv->viewport()->setAutoFillBackground(false);
    d->newWikiSv->setVisible(false);

    QWidget* newWikiBox         = new QWidget(newWikiPanel);
    newWikiBox->setWhatsThis(i18n("These are options for adding a Wiki."));

    QGridLayout* newWikiLayout  = new QGridLayout(newWikiBox);

    QLabel*  newWikiNameLabel   = new QLabel(newWikiPanel);
    newWikiNameLabel->setText(i18n("Wiki:"));


    QLabel*  newWikiUrlLabel    = new QLabel(newWikiPanel);
    newWikiUrlLabel->setText(i18n("Url:"));

    d->newWikiNameEdit          = new KLineEdit(newWikiPanel);
    d->newWikiUrlEdit           = new KLineEdit(newWikiPanel);

    KPushButton* addWikiBtn     = new KPushButton(KGuiItem(i18n("Add"), "list-add", i18n("Add a new wiki")), newWikiPanel);

    newWikiLayout->addWidget(newWikiNameLabel,   0, 0, 1, 1);
    newWikiLayout->addWidget(d->newWikiNameEdit, 0, 1, 1, 1);
    newWikiLayout->addWidget(newWikiUrlLabel,    1, 0, 1, 1);
    newWikiLayout->addWidget(d->newWikiUrlEdit,  1, 1, 1, 1);
    newWikiLayout->addWidget(addWikiBtn,         2, 1, 1, 1);

    QLabel* nameLabel     = new QLabel(d->loginBox);
    nameLabel->setText(i18n( "Login:" ));

    QLabel* passwdLabel   = new QLabel(d->loginBox);
    passwdLabel->setText(i18n("Password:"));

    QPushButton* loginBtn = new QPushButton(d->loginBox);
    loginBtn->setAutoDefault(true);
    loginBtn->setDefault(true);
    loginBtn->setText(i18n("&Log in"));

    loginBoxLayout->addWidget(wikiLabel,     0, 0, 1, 1);
    loginBoxLayout->addWidget(d->wikiSelect, 0, 1, 1, 1);
    loginBoxLayout->addWidget(newWikiBtn,    0, 2, 1, 1);
    loginBoxLayout->addWidget(d->newWikiSv,  1, 1, 3, 3);
    loginBoxLayout->addWidget(nameLabel,     4, 0, 1, 1);
    loginBoxLayout->addWidget(d->nameEdit,   4, 1, 1, 1);
    loginBoxLayout->addWidget(d->passwdEdit, 5, 1, 1, 1);
    loginBoxLayout->addWidget(passwdLabel,   5, 0, 1, 1);
    loginBoxLayout->addWidget(loginBtn,      6, 0, 1, 1);
    loginBoxLayout->setObjectName("d->loginBoxLayout");

    d->accountBox         = new KHBox(d->userBox);
    d->accountBox->setWhatsThis(i18n("This is the account that is currently logged in."));

    QLabel* userNameLbl   = new QLabel(d->accountBox);
    userNameLbl->setText(i18nc("Username which is used to connect to the wiki", "Logged as: "));
    d->userNameDisplayLbl = new QLabel(d->accountBox);
    QLabel* space         = new QLabel(d->accountBox);
    d->changeUserBtn      = new KPushButton(KGuiItem(i18n("Change Account"), "system-switch-user",
                               i18n("Logout and change the account used for transfer")),
                               d->accountBox);
    d->accountBox->setStretchFactor(space, 10);
    d->accountBox->hide();

    d->settingsExpander->addItem(d->userBox, i18n("Account"), QString("account"), true);
    d->settingsExpander->setItemIcon(0, SmallIcon("user-properties"));

    // --------------------- Login area ----------------------------------

    d->textBox                 = new QWidget(panel2);
    d->textBox->setWhatsThis(i18n("This is the login form to your account on the chosen wiki."));
    QGridLayout* textBoxLayout = new QGridLayout(d->textBox);

    QLabel* authorLbl      = new QLabel(i18n("Author:"), d->textBox);
    d->authorEdit          = new KLineEdit(d->textBox);

    QLabel* sourceLbl      = new QLabel(i18n("Source:"), d->textBox);
    d->sourceEdit          = new KLineEdit(d->textBox);

    QLabel* licenseLbl     = new QLabel(i18n("License:"), d->textBox);
    d->licenseComboBox     = new SqueezedComboBox(d->textBox);

    d->licenseComboBox->addSqueezedItem(i18n("Own work, multi-license with CC-BY-SA-3.0 and GFDL"), 
                                       QString("{{self|cc-by-sa-3.0|GFDL|migration=redundant}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Own work, multi-license with CC-BY-SA-3.0 and older"), 
                                       QString("{{self|cc-by-sa-3.0,2.5,2.0,1.0}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Creative Commons Attribution-Share Alike 3.0"),
                                       QString("{{self|cc-by-sa-3.0}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Own work, Creative Commons Attribution 3.0"),
                                       QString("{{self|cc-by-3.0}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Own work, release into public domain under the CC-Zero license"),
                                       QString("{{self|cc-zero}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Author died more than 100 years ago"),
                                       QString("{{PD-old}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Photo of a two-dimensional work whose author died more than 100 years ago"),
                                       QString("{{PD-art}}"));
    d->licenseComboBox->addSqueezedItem(i18n("First published in the United States before 1923"),
                                       QString("{{PD-US}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Work of a U.S. government agency"),
                                       QString("{{PD-USGov}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Simple typefaces, individual words or geometric shapes"),
                                       QString("{{PD-text}}"));
    d->licenseComboBox->addSqueezedItem(i18n("Logos with only simple typefaces, individual words or geometric shapes"),
                                       QString("{{PD-textlogo}}"));

    QLabel* genCatLbl      = new QLabel(i18n("Generic categories:"), d->textBox);
    d->genCatEdit          = new KTextEdit(d->textBox);
    d->genCatEdit->setTabChangesFocus(1);
    d->genCatEdit->setWhatsThis(i18n("This is a place to enter categories that will be added to all the files."));
    d->genCatEdit->setAcceptRichText(false);

    QLabel* genTxtLbl      = new QLabel(i18n("Generic text:"), d->textBox);
    d->genTxtEdit          = new KTextEdit(d->textBox);
    d->genTxtEdit->setTabChangesFocus(1);
    d->genTxtEdit->setWhatsThis(i18n("This is a place to enter text that will be added to all the files, below the Information template."));
    d->genTxtEdit->setAcceptRichText(false);

    textBoxLayout->addWidget(authorLbl,          1, 0, 1, 1);
    textBoxLayout->addWidget(sourceLbl,          2, 0, 1, 1);
    textBoxLayout->addWidget(licenseLbl,         3, 0, 1, 1);
    textBoxLayout->addWidget(genCatLbl,          4, 0, 1, 1);
    textBoxLayout->addWidget(genTxtLbl,          5, 0, 1, 1);

    textBoxLayout->addWidget(d->authorEdit,      1, 2, 1, 2);
    textBoxLayout->addWidget(d->sourceEdit,      2, 2, 1, 2);
    textBoxLayout->addWidget(d->licenseComboBox, 3, 2, 1, 2);
    textBoxLayout->addWidget(d->genCatEdit,      4, 2, 1, 2);
    textBoxLayout->addWidget(d->genTxtEdit,      5, 2, 1, 2);
    textBoxLayout->setObjectName("d->textBoxLayout");

    d->settingsExpander->addItem(d->textBox, i18n("Information"), QString("information"), true);
    d->settingsExpander->setItemIcon(1, SmallIcon("document-properties"));

    // --------------------- Options area ----------------------------------

    d->optionsBox = new QWidget(panel2);
    d->optionsBox->setWhatsThis(i18n("These are options that will be applied to photos before upload."));
    QGridLayout* optionsBoxLayout = new QGridLayout(d->optionsBox);

    d->resizeChB = new QCheckBox(d->optionsBox);
    d->resizeChB->setText(i18n("Resize photos before uploading"));
    d->resizeChB->setChecked(false);

    d->dimensionSpB = new QSpinBox(d->optionsBox);
    d->dimensionSpB->setMinimum(0);
    d->dimensionSpB->setMaximum(5000);
    d->dimensionSpB->setSingleStep(10);
    d->dimensionSpB->setValue(600);
    d->dimensionSpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->dimensionSpB->setEnabled(false);
    QLabel* dimensionLbl = new QLabel(i18n("Maximum size:"), d->optionsBox);

    d->imageQualitySpB = new QSpinBox(d->optionsBox);
    d->imageQualitySpB->setMinimum(0);
    d->imageQualitySpB->setMaximum(100);
    d->imageQualitySpB->setSingleStep(1);
    d->imageQualitySpB->setValue(85);
    d->imageQualitySpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* imageQualityLbl = new QLabel(i18n("JPEG quality:"), d->optionsBox);

    optionsBoxLayout->addWidget(d->resizeChB,       0, 0, 1, 2);
    optionsBoxLayout->addWidget(dimensionLbl,       1, 0, 1, 1);
    optionsBoxLayout->addWidget(imageQualityLbl,    2, 0, 1, 1);
    optionsBoxLayout->addWidget(d->dimensionSpB,    1, 1, 1, 1);
    optionsBoxLayout->addWidget(d->imageQualitySpB, 2, 1, 1, 1);
    optionsBoxLayout->setRowStretch(3, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    d->settingsExpander->addItem(d->optionsBox, i18n("Options"), QString("options"), true);
    d->settingsExpander->setItemIcon(2, SmallIcon("system-run"));

    // ------------------------------------------------------------------------

    QTabWidget* tabWidget;
    tabWidget = new QTabWidget;
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
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->addWidget(d->progressBar);
    mainLayout->setMargin(0);

    updateLabels();  // use empty labels until login

    // --------------------- Slots definition ----------------------------------

    connect(d->resizeChB, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

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
    // textEdited() would be better, but KTextEdit does not have this.
    // Solution for the moment: we do not setText() when the selection changes
    connect(d->categoryEdit, SIGNAL(textChanged()),
            this, SLOT(slotApplyCategories()));

    // Problem: textChanged() is also called when the text is changed by setText()
    // textEdited() would be better, but KTextEdit does not have this.
    // Solution for the moment: we do not setText() when the selection changes
    connect(d->descEdit, SIGNAL(textChanged()),
            this, SLOT(slotApplyDescription()));

    connect(d->latitudeEdit, SIGNAL(textEdited(QString)),
            this, SLOT(slotApplyLatitude()));

    connect(d->longitudeEdit, SIGNAL(textEdited(QString)),
            this, SLOT(slotApplyLongitude()));

    connect(d->imgList, SIGNAL(signalItemClicked(QTreeWidgetItem*)),
            this, SLOT(slotLoadImagesDesc(QTreeWidgetItem*)));

    connect(d->imgList, SIGNAL(signalRemovedItems(KUrl::List)),
            this, SLOT(slotRemoveImagesDesc(KUrl::List)));
}

WmWidget::~WmWidget()
{
    delete d;
}

void WmWidget::readSettings(KConfigGroup& group)
{
    kDebug() <<  "Read settings from" << group.name();

#if KDCRAW_VERSION >= 0x020000
    d->settingsExpander->readSettings(group);
#else
    d->settingsExpander->readSettings();
#endif

    d->authorEdit->setText(group.readEntry("Author",        ""));
    d->sourceEdit->setText(group.readEntry("Source",        "{{own}}"));

    d->genCatEdit->setText(group.readEntry("genCategories", "Uploaded with KIPI uploader"));
    d->genTxtEdit->setText(group.readEntry("genText",       ""));

    d->resizeChB->setChecked(group.readEntry("Resize",      false));
    d->dimensionSpB->setValue(group.readEntry("Dimension",  600));
    d->imageQualitySpB->setValue(group.readEntry("Quality", 85));
    slotResizeChecked();

    d->WikisHistory = group.readEntry("Wikis history",      QStringList());
    d->UrlsHistory  = group.readEntry("Urls history",       QStringList());

    kDebug() <<  "UrlHistory.size: " << d->UrlsHistory.size() << "; WikisHistory.size:" << d->WikisHistory.size();

    for(int i = 0 ; i < d->UrlsHistory.size() && i < d->WikisHistory.size() ; i++)
    {
        d->wikiSelect->addItem(d->WikisHistory.at(i),d->UrlsHistory.at(i));
    }
}

void WmWidget::saveSettings(KConfigGroup& group)
{
    kDebug() <<  "Save settings to" << group.name();

#if KDCRAW_VERSION >= 0x020000
    d->settingsExpander->writeSettings(group);
#else
    d->settingsExpander->writeSettings();
#endif

    group.writeEntry("Author",        d->authorEdit->text());
    group.writeEntry("Source",        d->sourceEdit->text());

    group.writeEntry("genCategories", d->genCatEdit->toPlainText());
    group.writeEntry("genText",       d->genTxtEdit->toPlainText());

    group.writeEntry("Resize",        d->resizeChB->isChecked());
    group.writeEntry("Dimension",     d->dimensionSpB->value());
    group.writeEntry("Quality",       d->imageQualitySpB->value());
}

KPImagesList* WmWidget::imagesList() const
{
    return d->imgList;
}

KPProgressWidget* WmWidget::progressBar() const
{
    return d->progressBar;
}

void WmWidget::updateLabels(const QString& name, const QString& url)
{
    QString web("http://www.mediawiki.org");

    if (!url.isEmpty())
        web = url;

    d->headerLbl->setText(QString("<b><h2><a href='%1'><font color=\"#3B5998\">%2</font></a></h2></b>").arg(web).arg("MediaWiki"));
    if (name.isEmpty())
    {
        d->userNameDisplayLbl->clear();
    }
    else
    {
        d->userNameDisplayLbl->setText(QString::fromLatin1("<b>%1</b>").arg(name));
    }
}

void WmWidget::invertAccountLoginBox()
{
    if(d->accountBox->isHidden())
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

void WmWidget::slotChangeUserClicked()
{
    emit signalChangeUserRequest();
}

void WmWidget::slotLoginClicked()
{
     emit signalLoginRequest(d->nameEdit->text(), d->passwdEdit->text(), 
                             d->wikiSelect->itemData(d->wikiSelect->currentIndex()).toUrl());
}

void WmWidget::slotNewWikiClicked()
{
    if(d->newWikiSv->isVisible())
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
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("MediaWiki export settings"));

    d->UrlsHistory << d->newWikiUrlEdit->userText();
    group.writeEntry("Urls history", d->UrlsHistory);

    d->WikisHistory << d->newWikiNameEdit->userText();
    group.writeEntry("Wikis history", d->WikisHistory);

    d->wikiSelect->addItem(d->newWikiNameEdit->userText(), d->newWikiUrlEdit->userText());
    d->wikiSelect->setCurrentIndex(d->wikiSelect->count()-1);

    slotNewWikiClicked();
}

void WmWidget::loadImageInfoFirstLoad()
{
    KUrl::List urls = d->imgList->imageUrls(false);

    QString title;
    QString date;
    QString description;
    QString currentCategories;
    QString latitude;
    QString longitude;

    for(int j = 0; j < urls.size(); j++)
    {
        KPImageInfo info(urls.at(j).path());
        QStringList keywar = info.keywords();
        date               = info.date().toString(Qt::ISODate);
        date               = date.replace("T", " ", Qt::CaseSensitive);
        title              = info.name();
        description        = info.title();
        currentCategories  = "";

        for( int i = 0; i < keywar.size(); i++)
        {
            if(i == keywar.size()-1)
            {
                currentCategories.append(keywar.at(i));
            }
            else
            {
                currentCategories.append(keywar.at(i)).append("\n");
            }
        }

        if(info.hasLatitude())
        {
            latitude = QString::number(info.latitude());
        }

        if(info.hasLongitude())
        {
            longitude = QString::number(info.longitude());
        }

        QMap<QString, QString> imageMetaData;
        imageMetaData["title"]       = title;
        imageMetaData["date"]        = date;
        imageMetaData["categories"]  = currentCategories;
        imageMetaData["description"] = description;
        imageMetaData["latitude"]    = latitude;
        imageMetaData["longitude"]   = longitude;
        d->imagesDescInfo.insert(urls.at(j).path(), imageMetaData);
    }
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
    KPImagesListViewItem* l_item          = dynamic_cast<KPImagesListViewItem*>(item);
    QMap<QString, QString> imageMetaData  = d->imagesDescInfo[l_item->url().path()];

    d->titleEdit->setText(imageMetaData["title"]);
    d->dateEdit->setText(imageMetaData["date"].replace("T", " ", Qt::CaseSensitive));
    d->latitudeEdit->setText(imageMetaData["latitude"]);
    d->longitudeEdit->setText(imageMetaData["longitude"]);

    if (selectedItems.size() == 1)
    {
        d->categoryEdit->setText(imageMetaData["categories"]);
        d->descEdit->setText(imageMetaData["description"]);
    }
}

void WmWidget::slotRemoveImagesDesc(const KUrl::List urls)
{
    for (KUrl::List::const_iterator it = urls.begin(); it != urls.end(); ++it)
    {
        QString path = (*it).path();
        d->imagesDescInfo.remove(path);
        kDebug() << "Remove" << path << "; new length:" << d->imagesDescInfo.size();
    }
}

void WmWidget::slotRestoreExtension()
{
    kDebug() << "RestoreExtension";

    QString imageTitle;
    QString originalExtension;
    QString currentExtension;
    KUrl::List urls;
    QMap<QString, QString> imageMetaData;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    // Build the list of items to rename
    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));
        urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        imageMetaData = d->imagesDescInfo[urls.at(i).path()];
        imageTitle = imageMetaData["title"];

        // Add original extension if removed
        currentExtension = imageTitle.split('.').last();
        originalExtension = urls.at(i).path().split('.').last();
        if(QString::compare(currentExtension, originalExtension, Qt::CaseInsensitive) != 0)
        {
            imageTitle.append(".").append(originalExtension);
            d->titleEdit->setText(imageTitle);
        }

        kDebug() << urls.at(i).path() << "renamed to" << imageTitle;
        imageMetaData["title"] = imageTitle;
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }
}

void WmWidget::slotApplyTitle()
{
    kDebug() << "ApplyTitle";

    QString givenTitle = title();
    QString imageTitle;
    QString number;
    KUrl::List urls;
    QMap<QString, QString> imageMetaData;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();
    QStringList parts;

    const int minLength = givenTitle.count("#");

    // Build the list of items to rename
    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));
        urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        imageMetaData = d->imagesDescInfo[urls.at(i).path()];
        imageTitle = givenTitle;

        // If there is at least one #, replace it the correct number
        if(minLength > 0)
        {
            parts = imageTitle.split("#", QString::KeepEmptyParts);
            imageTitle = parts.first().append("#").append(parts.last());
            number = QString::number(i + 1);
            while (number.length() < minLength)
            {
                number.prepend("0");
            }
            imageTitle.replace(imageTitle.indexOf("#"), 1, number);
        }

        kDebug() << urls.at(i).path() << "renamed to" << imageTitle;
        imageMetaData["title"] = imageTitle;
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }
}

void WmWidget::slotApplyDate()
{
    KUrl::List urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));
        urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData["date"] = date();
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }
}

void WmWidget::slotApplyCategories()
{
    KUrl::List urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));
        urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                       = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData["categories"]         = categories();
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }
}

void WmWidget::slotApplyDescription()
{
    KUrl::List urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));
        urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                       = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData["description"]        = description();
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }
}

void WmWidget::slotApplyLatitude()
{

    KUrl::List urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));
        urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                       = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData["latitude"]           = latitude();
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }
}

void WmWidget::slotApplyLongitude()
{
    KUrl::List urls;
    QList<QTreeWidgetItem*> selectedItems = d->imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i)
    {
        KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));
        urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData;
        imageMetaData                       = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData["longitude"]          = longitude();
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }
}

void WmWidget::clearImagesDesc()
{
    d->imagesDescInfo.clear();
}

QMap <QString,QMap <QString,QString> > WmWidget::allImagesDesc()
{
    KUrl::List urls = d->imgList->imageUrls(false);

    for (int i = 0; i < urls.size(); ++i)
    {
        QMap<QString, QString> imageMetaData = d->imagesDescInfo[urls.at(i).path()];
        imageMetaData["author"]              = author();
        imageMetaData["source"]              = source();
        imageMetaData["license"]             = license();
        imageMetaData["genCategories"]       = genCategories();
        imageMetaData["genText"]             = genText();
        d->imagesDescInfo[urls.at(i).path()] = imageMetaData;
    }

    return d->imagesDescInfo;
}

QString WmWidget::author() const
{
    kDebug() << "WmWidget::author()";
    return d->authorEdit->text();
}

QString WmWidget::source() const
{
    kDebug() << "WmWidget::source()";
    return d->sourceEdit->text();
}

QString WmWidget::genCategories() const
{
    kDebug() << "WmWidget::genCategories()";
    return d->genCatEdit->toPlainText();
}

QString WmWidget::genText() const
{
    kDebug() << "WmWidget::genText()";
    return d->genTxtEdit->toPlainText();
}

int WmWidget::quality() const
{
    kDebug() << "WmWidget::quality()";
    return d->imageQualitySpB->value();
}

int WmWidget::dimension() const
{
    kDebug() << "WmWidget::dimension()";
    return d->dimensionSpB->value();
}

bool WmWidget::resize() const
{
    kDebug() << "WmWidget::resize()";
    return d->resizeChB->isChecked();
}

QString WmWidget::license() const
{
    kDebug() << "WmWidget::license()";
    return d->licenseComboBox->itemData(d->licenseComboBox->currentIndex()).toString();
}

QString WmWidget::title() const
{
    kDebug() << "WmWidget::title()";
    return d->titleEdit->text();
}

QString WmWidget::categories() const
{
    kDebug() << "WmWidget::categories()";
    return d->categoryEdit->toPlainText();
}

QString WmWidget::description() const
{
    kDebug() << "WmWidget::description()";
    return d->descEdit->toPlainText();
}

QString WmWidget::date() const
{
    kDebug() << "WmWidget::date()";
    return d->dateEdit->text();
}

QString WmWidget::latitude() const
{
    kDebug() << "WmWidget::latitude()";
    return d->latitudeEdit->text();
}

QString WmWidget::longitude() const
{
    kDebug() << "WmWidget::longitude()";
    return d->longitudeEdit->text();
}

} // namespace KIPIWikimediaPlugin
