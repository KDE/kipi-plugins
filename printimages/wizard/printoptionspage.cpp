/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-02-08
 * Description : a kipi plugin to print images
 *
 * Copyright 2009-2012 by Angelo Naselli <anaselli at linux dot it>
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

#include "printoptionspage.moc"

// Qt includes

#include <QButtonGroup>
#include <QGridLayout>
#include <QToolButton>
#include <QPushButton>
#include <QPainter>

// KDE includes

#include <kconfigdialogmanager.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kmessagebox.h>
#include <kdebug.h>

// Local includes

#include "tphoto.h"
#include "printimagesconfig.h"
#include "signalblocker.h"
#include "ui_printoptionspage.h"

namespace KIPIPrintImagesPlugin
{

class PrintOptionsPage::Private
    : public Ui_PrintOptionsPage
{
public:

    QWidget*              mParent;
    QList<TPhoto*>*       m_photos;
    int                   m_currentPhoto;
    QButtonGroup          mScaleGroup;
    QButtonGroup          mPositionGroup;
    KConfigDialogManager* mConfigDialogManager;

public:

    void initPositionFrame()
    {
        mPositionFrame->setStyleSheet
        (
            "QFrame {"
            " background-color: palette(mid);"
            " border: 1px solid palette(dark);"
            "}"
            "QToolButton {"
            " border: none;"
            " background: palette(base);"
            "}"
            "QToolButton:hover {"
            " background: palette(alternate-base);"
            " border: 1px solid palette(highlight);"
            "}"
            "QToolButton:checked {"
            " background-color: palette(highlight);"
            "}"
        );

        QGridLayout* const layout = new QGridLayout ( mPositionFrame );
        layout->setMargin ( 0 );
        layout->setSpacing ( 1 );

        for ( int row = 0; row < 3; ++row )
        {
            for ( int col = 0; col < 3; ++col )
            {
                QToolButton* const button = new QToolButton ( mPositionFrame );
                button->setFixedSize ( 40, 40 );
                button->setCheckable ( true );
                layout->addWidget ( button, row, col );

                Qt::Alignment alignment;

                if ( row == 0 )
                {
                    alignment = Qt::AlignTop;
                }
                else if ( row == 1 )
                {
                    alignment = Qt::AlignVCenter;
                }
                else
                {
                    alignment = Qt::AlignBottom;
                }
                if ( col == 0 )
                {
                    alignment |= Qt::AlignLeft;
                }
                else if ( col == 1 )
                {
                    alignment |= Qt::AlignHCenter;
                }
                else
                {
                    alignment |= Qt::AlignRight;
                }

                mPositionGroup.addButton ( button, int ( alignment ) );
            }
        }
    }
};

PrintOptionsPage::PrintOptionsPage(QWidget* const parent, QList<TPhoto*>* const photoList)
    : QWidget(), d(new Private)
{
    d->setupUi ( this );
    d->mParent              = parent;
    d->m_photos             = photoList;
    d->m_currentPhoto       = 0;
    d->mConfigDialogManager = new KConfigDialogManager ( this, PrintImagesConfig::self() );

    d->initPositionFrame();

    d->mScaleGroup.addButton ( d->mNoScale, NoScale );
    d->mScaleGroup.addButton ( d->mScaleToPage, ScaleToPage );
    d->mScaleGroup.addButton ( d->mScaleTo, ScaleToCustomSize );
    d->mPhotoXPage->setRange ( 0, d->m_photos->size() );

    d->mPX->setSpecialValueText ( i18n ( "disabled" ) );
    d->mPY->setSpecialValueText ( i18n ( "disabled" ) );
    d->mPhotoXPage->setSpecialValueText ( i18n ( "disabled" ) );

    //Change cursor to waitCursor during transition
    QApplication::setOverrideCursor ( QCursor ( Qt::WaitCursor ) );

//     showAdditionalInfo();
    imagePreview();
    enableButtons();
    QApplication::restoreOverrideCursor();

    connect ( d->kcfg_PrintWidth, SIGNAL (valueChanged(double)),
              SLOT (adjustHeightToRatio()) );

    connect ( d->kcfg_PrintHeight, SIGNAL (valueChanged(double)),
              SLOT (adjustWidthToRatio()) );

    connect ( d->kcfg_PrintKeepRatio, SIGNAL (toggled(bool)),
              SLOT (adjustHeightToRatio()) );

    connect ( d->mPhotoXPage, SIGNAL (valueChanged(int)), SLOT (photoXpageChanged(int)) );
    connect ( d->mPX, SIGNAL (valueChanged(int)), SLOT (horizontalPagesChanged(int)) );
    connect ( d->mPY, SIGNAL (valueChanged(int)), SLOT (verticalPagesChanged(int)) );

    connect ( d->mRightButton, SIGNAL (clicked()),
              this, SLOT (selectNext()) );

    connect ( d->mLeftButton, SIGNAL (clicked()),
              this, SLOT (selectPrev()) );

    connect ( d->mSaveSettings, SIGNAL (clicked()),
              this, SLOT (saveConfig()) );

    connect ( d->mNoScale, SIGNAL (clicked(bool)), SLOT (scaleOption()) );
    connect ( d->mScaleToPage, SIGNAL (clicked(bool)), SLOT (scaleOption()) );
    connect ( d->mScaleTo, SIGNAL (clicked(bool)), SLOT (scaleOption()) );
    connect ( d->kcfg_PrintAutoRotate, SIGNAL (toggled(bool)), SLOT (autoRotate(bool)) );
    connect ( &d->mPositionGroup, SIGNAL (buttonClicked(int)), SLOT (positionChosen(int)));

    layout()->setMargin ( 0 );
}

PrintOptionsPage::~PrintOptionsPage()
{
    delete d;
}

double PrintOptionsPage::unitToInches ( PrintOptionsPage::Unit unit )
{
    if ( unit == PrintOptionsPage::Inches )
    {
      return 1.;
    }
    else if ( unit == PrintOptionsPage::Centimeters )
    {
      return 1/2.54;
    }
    else   // Millimeters
    {
      return 1/25.4;
    }
}

Qt::Alignment PrintOptionsPage::alignment() const
{
    int id = d->mPositionGroup.checkedId();
//     kDebug() << "alignment=" << id;

    return Qt::Alignment ( id );
}

PrintOptionsPage::Unit PrintOptionsPage::scaleUnit() const
{
    d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mUnit = PrintOptionsPage::Unit ( d->kcfg_PrintUnit->currentIndex() );

    return PrintOptionsPage::Unit ( d->kcfg_PrintUnit->currentIndex() );
}

double PrintOptionsPage::scaleWidth() const
{
    d->m_photos->at ( d->m_currentPhoto )->cropRegion = QRect ( 0, 0,
        ( int ) ( d->kcfg_PrintWidth->value() * unitToInches ( scaleUnit() ) ),
        ( int ) ( d->kcfg_PrintHeight->value() * unitToInches ( scaleUnit() ) ) );

    return d->kcfg_PrintWidth->value() * unitToInches ( scaleUnit() );
}

double PrintOptionsPage::scaleHeight() const
{
    d->m_photos->at ( d->m_currentPhoto )->cropRegion = QRect ( 0, 0,
        ( int ) ( d->kcfg_PrintWidth->value() * unitToInches ( scaleUnit() ) ),
        ( int ) ( d->kcfg_PrintHeight->value() * unitToInches ( scaleUnit() ) ) );

    return d->kcfg_PrintHeight->value() * unitToInches ( scaleUnit() );
}

void PrintOptionsPage::adjustWidthToRatio()
{
    if ( !d->kcfg_PrintKeepRatio->isChecked() )
    {
        return;
    }

    double width = d->m_photos->at ( d->m_currentPhoto )->width() * d->kcfg_PrintHeight->value() /
                   d->m_photos->at ( d->m_currentPhoto )->height();
    d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintHeight = d->kcfg_PrintHeight->value();
    d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintWidth  =  width ? width : 1.;
    SignalBlocker blocker ( d->kcfg_PrintWidth );
    d->kcfg_PrintWidth->setValue ( d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintWidth );
//     kDebug() << " width " << d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintWidth << " height " <<  d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintHeight;
}

void PrintOptionsPage::adjustHeightToRatio()
{
    if ( !d->kcfg_PrintKeepRatio->isChecked() )
    {
      return;
    }

    double height = d->m_photos->at ( d->m_currentPhoto )->height() * d->kcfg_PrintWidth->value() / d->m_photos->at ( d->m_currentPhoto )->width();
    d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintWidth  = d->kcfg_PrintWidth->value();
    d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintHeight =  height ? height : 1. ;
    SignalBlocker blocker ( d->kcfg_PrintHeight );
    d->kcfg_PrintHeight->setValue ( d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintHeight );
//     kDebug() << "height " <<  d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintHeight << " width " << d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintWidth;
}

void PrintOptionsPage::manageQPrintDialogChanges ( QPrinter * /*printer*/ )
{
    kDebug() << "It has been called!";
}

int PrintOptionsPage::photoXPage() const
{
    return d->mPhotoXPage->value();
}

int PrintOptionsPage::mp_horPages() const
{
    return d->mPX->value();
}

int PrintOptionsPage::mp_verPages() const
{
    return d->mPY->value();
}
bool PrintOptionsPage::printUsingAtkinsLayout() const
{
    return ( d->mPhotoXPage->value() > 0 );
}

void PrintOptionsPage::enableButtons()
{
    if ( d->m_photos->size() == 1 )
    {
        d->mLeftButton->setEnabled ( false );
        d->mRightButton->setEnabled ( false );
    }
    else if ( d->m_currentPhoto == 0 )
    {
        d->mLeftButton->setEnabled ( false );
        d->mRightButton->setEnabled ( true );
    }
    else if ( d->m_currentPhoto == d->m_photos->size()-1 )
    {
        d->mRightButton->setEnabled ( false );
        d->mLeftButton->setEnabled ( true );
    }
    else
    {
        d->mLeftButton->setEnabled ( true );
        d->mRightButton->setEnabled ( true );
    }
}

void PrintOptionsPage::imagePreview()
{
//     kDebug() << d->m_currentPhoto;
    TPhoto *pPhoto = d->m_photos->at ( d->m_currentPhoto );
    d->mPreview->setPixmap ( pPhoto->thumbnail() );
    if ( pPhoto->cropRegion != QRect() )
    {
        // TODO
    }
}

void PrintOptionsPage::selectNext()
{
    //Change cursor to waitCursor during transition
    QApplication::setOverrideCursor ( QCursor ( Qt::WaitCursor ) );

//     kDebug() << d->m_currentPhoto;

    d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintPosition = alignment();

    if ( d->m_currentPhoto+1 < d->m_photos->size() )
      d->m_currentPhoto++;

    showAdditionalInfo();
    imagePreview();
    enableButtons();
    QApplication::restoreOverrideCursor();
}

void PrintOptionsPage::selectPrev()
{
    //Change cursor to waitCursor during transition
    QApplication::setOverrideCursor ( QCursor ( Qt::WaitCursor ) );

//     kDebug() << d->m_currentPhoto;
    d->m_photos->at ( d->m_currentPhoto )->pAddInfo->mPrintPosition = alignment();

    if ( d->m_currentPhoto-1 >= 0 )
      d->m_currentPhoto--;

    showAdditionalInfo();
    imagePreview();
    enableButtons();
    QApplication::restoreOverrideCursor();
}

void PrintOptionsPage::setAdditionalInfo()
{
    for ( int i=0; i < d->m_photos->count(); ++i )
    {
        TPhoto* pPhoto = d->m_photos->at ( i );

        if ( pPhoto )
        {
            pPhoto->pAddInfo->mUnit                 = PrintImagesConfig::printUnit();
            pPhoto->pAddInfo->mPrintPosition        = PrintImagesConfig::printPosition();
            pPhoto->pAddInfo->mKeepRatio            = PrintImagesConfig::printKeepRatio();
            pPhoto->pAddInfo->mScaleMode            = PrintImagesConfig::printScaleMode();
            pPhoto->pAddInfo->mAutoRotate           = PrintImagesConfig::printAutoRotate();
            pPhoto->pAddInfo->mPrintWidth           = PrintImagesConfig::printWidth();
            pPhoto->pAddInfo->mPrintHeight          = PrintImagesConfig::printHeight();
            pPhoto->pAddInfo->mEnlargeSmallerImages = PrintImagesConfig::printEnlargeSmallerImages();

            if (pPhoto->pAddInfo->mKeepRatio)
            {
                double height = d->m_photos->at(i)->height() * pPhoto->pAddInfo->mPrintWidth / d->m_photos->at(i)->width();
                d->m_photos->at(i)->pAddInfo->mPrintHeight =  height ? height : PrintImagesConfig::printHeight();
            }
//           kDebug() << " photo " << i << " printWidth " <<  pPhoto->pAddInfo->mPrintWidth
//                    << " printHeight " << pPhoto->pAddInfo->mPrintHeight; 
//     
        }
    }
}

void PrintOptionsPage::showAdditionalInfo()
{
    QAbstractButton* button = 0;
    int i                   = d->m_currentPhoto;
    TPhoto* const pPhoto    = d->m_photos->at ( i );

    if ( pPhoto )
    {
        d->kcfg_PrintUnit->setCurrentIndex ( pPhoto->pAddInfo->mUnit );
        button = d->mPositionGroup.button ( pPhoto->pAddInfo->mPrintPosition );

        if ( button )
        {
            button->setChecked ( true );
        }
        else
        {
            kWarning() << "Unknown button for position group";
        }

        button = d->mScaleGroup.button ( pPhoto->pAddInfo->mScaleMode );

        if ( button )
        {
            button->setChecked ( true );
        }
        else
        {
            kWarning() << "Unknown button for scale group";
        }

        d->kcfg_PrintKeepRatio->setChecked ( pPhoto->pAddInfo->mKeepRatio );
        d->kcfg_PrintAutoRotate->setChecked ( pPhoto->pAddInfo->mAutoRotate );
        d->kcfg_PrintEnlargeSmallerImages->setChecked ( pPhoto->pAddInfo->mEnlargeSmallerImages );
        d->kcfg_PrintWidth->setValue ( pPhoto->pAddInfo->mPrintWidth );
        d->kcfg_PrintHeight->setValue ( pPhoto->pAddInfo->mPrintHeight );

        if ( d->kcfg_PrintKeepRatio->isChecked() )
        {
            adjustHeightToRatio();
        }
    }
}

void PrintOptionsPage::loadConfig()
{
    QAbstractButton* button = 0;
    button = d->mPositionGroup.button ( PrintImagesConfig::printPosition() );

    if ( button )
    {
        button->setChecked ( true );
    }
    else
    {
        kWarning() << "Unknown button for position group";
    }

    button = d->mScaleGroup.button ( PrintImagesConfig::printScaleMode() );

    if ( button )
    {
        button->setChecked ( true );
    }
    else
    {
        kWarning() << "Unknown button for scale group";
    }

    d->mConfigDialogManager->updateWidgets();

    // config has been read, now we set photo additional info
    setAdditionalInfo();
}

void PrintOptionsPage::saveConfig()
{
    int position        = d->mPositionGroup.checkedId();
    PrintImagesConfig::setPrintPosition ( position );

    ScaleMode scaleMode = ScaleMode ( d->mScaleGroup.checkedId() );
    PrintImagesConfig::setPrintScaleMode ( scaleMode );

    bool checked        = d->kcfg_PrintAutoRotate->isChecked();
    PrintImagesConfig::setPrintAutoRotate ( checked );

    d->mConfigDialogManager->updateSettings();

    PrintImagesConfig::self()->writeConfig();
}

void PrintOptionsPage::photoXpageChanged ( int i )
{
    bool disabled = ( i>0 );
    d->mPositionFrame->setDisabled ( disabled );
    d->mGroupScaling->setDisabled ( disabled );
    d->mGroupImage->setDisabled ( disabled );
    d->kcfg_PrintAutoRotate->setDisabled ( disabled );
    d->mPreview->setDisabled ( disabled );

    if ( disabled )
    {
        d->mRightButton->setDisabled ( disabled );
        d->mLeftButton->setDisabled ( disabled );
        SignalBlocker block_mPX ( d->mPX );
        d->mPX->setValue ( 0 );
        SignalBlocker block_mPY ( d->mPY );
        d->mPY->setValue ( 0 );
    }
    else
    {
        enableButtons();
    }
}

void PrintOptionsPage::horizontalPagesChanged ( int i )
{
    bool disabled = ( i>0 );
    d->mPositionFrame->setDisabled ( disabled );
    d->mGroupScaling->setDisabled ( disabled );
    d->mGroupImage->setDisabled ( disabled );
    d->kcfg_PrintAutoRotate->setDisabled ( disabled );
    d->mPreview->setDisabled ( disabled );

    if ( disabled )
    {
        d->mRightButton->setDisabled ( disabled );
        d->mLeftButton->setDisabled ( disabled );
        SignalBlocker blocker ( d->mPhotoXPage );
        d->mPhotoXPage->setValue ( 0 );

        if ( d->mPY->value() == 0 )
        {
            SignalBlocker block_mPY ( d->mPY );
            d->mPY->setValue ( 1 );
        }
    }
    else
    {
        SignalBlocker block_mPX ( d->mPY );
        d->mPY->setValue ( 0 );
        enableButtons();
    }
}

void PrintOptionsPage::verticalPagesChanged ( int i )
{
    bool disabled = ( i>0 );
    d->mPositionFrame->setDisabled ( disabled );
    d->mGroupScaling->setDisabled ( disabled );
    d->mGroupImage->setDisabled ( disabled );
    d->kcfg_PrintAutoRotate->setDisabled ( disabled );
    d->mPreview->setDisabled ( disabled );

    if ( disabled )
    {
        d->mRightButton->setDisabled ( disabled );
        d->mLeftButton->setDisabled ( disabled );
        SignalBlocker blocker ( d->mPhotoXPage );
        d->mPhotoXPage->setValue ( 0 );

        if ( d->mPX->value() == 0 )
        {
            SignalBlocker block_mPX ( d->mPX );
            d->mPX->setValue ( 1 );
        }
    }
    else
    {
        SignalBlocker block_mPX ( d->mPX );
        d->mPX->setValue ( 0 );
        enableButtons();
    }
}

void PrintOptionsPage::scaleOption()
{
    ScaleMode scaleMode  = ScaleMode ( d->mScaleGroup.checkedId() );
//   kDebug() << "ScaleMode " << int ( scaleMode );
    int i                = d->m_currentPhoto;
    TPhoto* const pPhoto = d->m_photos->at ( i );

    if (pPhoto)
    {
        pPhoto->pAddInfo->mScaleMode = scaleMode;
    }

    if (scaleMode == ScaleToCustomSize &&  d->kcfg_PrintKeepRatio->isChecked())
    {
        adjustHeightToRatio();
    }
}

void PrintOptionsPage::autoRotate ( bool value )
{
    int i                = d->m_currentPhoto;
    TPhoto* const pPhoto = d->m_photos->at ( i );

    if ( pPhoto )
    {
        pPhoto->pAddInfo->mAutoRotate = value;
    }
}

void PrintOptionsPage::positionChosen(int id)
{
    //   kDebug() << "Current photo " << d->m_currentPhoto << "position " << id;
    d->m_photos->at(d->m_currentPhoto)->pAddInfo->mPrintPosition = Qt::Alignment(id);
}

} // namespace KIPIPrintImagesPlugin
