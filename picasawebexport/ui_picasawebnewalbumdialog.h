#include <kdialog.h>
#include <klocale.h>

/********************************************************************************
** Form generated from reading ui file 'picasawebnewalbumdialog.ui'
**
** Created: Mon Oct 27 12:46:19 2008
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PICASAWEBNEWALBUMDIALOG_H
#define UI_PICASAWEBNEWALBUMDIALOG_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>
#include "kdatetimewidget.h"
#include "klineedit.h"
#include "ktextedit.h"

QT_BEGIN_NAMESPACE

class Ui_NewAlbumDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *textLabel1;
    KLineEdit *m_titleLineEdit;
    QLabel *textLabel3;
    KDateTimeWidget *m_dateAndTimeEdit;
    QLabel *textLabel2;
    KTextEdit *m_descriptionTextBox;
    QLabel *textLabel4;
    QLineEdit *m_locationLineEdit;
    QGroupBox *m_accessRadioButton;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *m_isPublicRadioButton;
    QRadioButton *m_isUnlistedRadioButton;
    QPushButton *m_createAlbumButton;
    QPushButton *m_cancelButton;

    void setupUi(QDialog *NewAlbumDialog)
    {
    if (NewAlbumDialog->objectName().isEmpty())
        NewAlbumDialog->setObjectName(QString::fromUtf8("NewAlbumDialog"));
    NewAlbumDialog->resize(504, 417);
    gridLayout = new QGridLayout(NewAlbumDialog);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    textLabel1 = new QLabel(NewAlbumDialog);
    textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
    textLabel1->setWordWrap(false);

    gridLayout->addWidget(textLabel1, 0, 0, 1, 1);

    m_titleLineEdit = new KLineEdit(NewAlbumDialog);
    m_titleLineEdit->setObjectName(QString::fromUtf8("m_titleLineEdit"));

    gridLayout->addWidget(m_titleLineEdit, 0, 1, 1, 2);

    textLabel3 = new QLabel(NewAlbumDialog);
    textLabel3->setObjectName(QString::fromUtf8("textLabel3"));
    textLabel3->setWordWrap(false);

    gridLayout->addWidget(textLabel3, 1, 0, 1, 1);

    m_dateAndTimeEdit = new KDateTimeWidget(NewAlbumDialog);
    m_dateAndTimeEdit->setObjectName(QString::fromUtf8("m_dateAndTimeEdit"));

    gridLayout->addWidget(m_dateAndTimeEdit, 1, 1, 1, 2);

    textLabel2 = new QLabel(NewAlbumDialog);
    textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
    textLabel2->setWordWrap(false);

    gridLayout->addWidget(textLabel2, 2, 0, 1, 1);

    m_descriptionTextBox = new KTextEdit(NewAlbumDialog);
    m_descriptionTextBox->setObjectName(QString::fromUtf8("m_descriptionTextBox"));

    gridLayout->addWidget(m_descriptionTextBox, 2, 1, 1, 2);

    textLabel4 = new QLabel(NewAlbumDialog);
    textLabel4->setObjectName(QString::fromUtf8("textLabel4"));
    textLabel4->setWordWrap(false);

    gridLayout->addWidget(textLabel4, 3, 0, 1, 1);

    m_locationLineEdit = new QLineEdit(NewAlbumDialog);
    m_locationLineEdit->setObjectName(QString::fromUtf8("m_locationLineEdit"));

    gridLayout->addWidget(m_locationLineEdit, 3, 1, 1, 2);

    m_accessRadioButton = new QGroupBox(NewAlbumDialog);
    m_accessRadioButton->setObjectName(QString::fromUtf8("m_accessRadioButton"));
    verticalLayout_2 = new QVBoxLayout(m_accessRadioButton);
    verticalLayout_2->setSpacing(6);
    verticalLayout_2->setMargin(11);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    m_isPublicRadioButton = new QRadioButton(m_accessRadioButton);
    m_isPublicRadioButton->setObjectName(QString::fromUtf8("m_isPublicRadioButton"));

    verticalLayout_2->addWidget(m_isPublicRadioButton);

    m_isUnlistedRadioButton = new QRadioButton(m_accessRadioButton);
    m_isUnlistedRadioButton->setObjectName(QString::fromUtf8("m_isUnlistedRadioButton"));

    verticalLayout_2->addWidget(m_isUnlistedRadioButton);


    gridLayout->addWidget(m_accessRadioButton, 4, 1, 1, 2);

    m_createAlbumButton = new QPushButton(NewAlbumDialog);
    m_createAlbumButton->setObjectName(QString::fromUtf8("m_createAlbumButton"));

    gridLayout->addWidget(m_createAlbumButton, 5, 1, 1, 1);

    m_cancelButton = new QPushButton(NewAlbumDialog);
    m_cancelButton->setObjectName(QString::fromUtf8("m_cancelButton"));

    gridLayout->addWidget(m_cancelButton, 5, 2, 1, 1);


    retranslateUi(NewAlbumDialog);
    QObject::connect(m_createAlbumButton, SIGNAL(clicked()), NewAlbumDialog, SLOT(accept()));
    QObject::connect(m_cancelButton, SIGNAL(clicked()), NewAlbumDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(NewAlbumDialog);
    } // setupUi

    void retranslateUi(QDialog *NewAlbumDialog)
    {
    NewAlbumDialog->setWindowTitle(tr2i18n("New Album Dialog", 0));
    textLabel1->setText(tr2i18n("Title", 0));
    textLabel3->setText(tr2i18n("Date", 0));
    textLabel2->setText(tr2i18n("Description", 0));
    textLabel4->setText(tr2i18n("Place Taken", 0));
    m_accessRadioButton->setTitle(tr2i18n("Access Level", 0));
    m_isPublicRadioButton->setText(tr2i18n("Public", 0));
    m_isUnlistedRadioButton->setText(tr2i18n("Unlisted ", 0));
    m_createAlbumButton->setText(tr2i18n("Create", 0));
    m_cancelButton->setText(tr2i18n("Cancel", 0));
    Q_UNUSED(NewAlbumDialog);
    } // retranslateUi

};

namespace Ui {
    class NewAlbumDialog: public Ui_NewAlbumDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PICASAWEBNEWALBUMDIALOG_H

