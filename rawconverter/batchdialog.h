/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2003-10-24
 * Description : Raw converter batch dialog
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef BATCHDIALOG_H
#define BATCHDIALOG_H

// Qt includes.

#include <qstringlist.h>
#include <qdict.h>

// KDE includes.

#include <kdialogbase.h>

class QListViewItem;
class QCheckBox;
class QRadioButton;
class QVButtonGroup;
class QLabel;
class QPixmap;

class KFileItem;
class KListView;
class KListViewItem;
class KURL;
class KProgress;
class KDoubleNumInput;

namespace KIPI
{
class ThumbnailJob;
}

namespace KIPIRawConverterPlugin
{

class  ProcessController;
struct RawItem;

class BatchDialog : public KDialogBase
{

    Q_OBJECT

public:

    enum TargetFileOp 
    {
        OVERWRITE,
        OPENFILEDIALOG
    };

    BatchDialog(QWidget *parent);
    ~BatchDialog();

    void addItems(const QStringList& itemList);

private:

    void readSettings();
    void saveSettings();
    void processOne();

private slots:

    void slotHelp();
    void slotUser1();
    void slotUser2();
    void slotAborted();

    void slotSaveFormatChanged();
    void slotIdentify();
    void slotIdentified(const QString& file, const QString& identity);
    void slotIdentifyFailed(const QString& file, const QString& identity);

    void slotProcessing(const QString& file);
    void slotProcessed(const QString& file, const QString& tmpFile);
    void slotProcessingFailed(const QString& file);

    void slotBusy(bool busy);
    void slotGotThumbnail(const KFileItem* url, const QPixmap& pix);

private:

    bool               busy_;

    QCheckBox         *cameraWBCheckBox_;
    QCheckBox         *fourColorCheckBox_;

    QVButtonGroup     *saveButtonGroup_;
    QVButtonGroup     *conflictButtonGroup_;

    QRadioButton      *jpegButton_;
    QRadioButton      *tiffButton_;
    QRadioButton      *ppmButton_;
    QRadioButton      *pngButton_;

    QRadioButton      *overwriteButton_;
    QRadioButton      *promptButton_;
    
    QDict<RawItem>     itemDict_;

    QStringList        fileList_;

    QString            targetExtension_;

    KDoubleNumInput   *brightnessSpinBox_;
    KDoubleNumInput   *redSpinBox_;
    KDoubleNumInput   *blueSpinBox_;

    KProgress         *progressBar_;

    KListView         *listView_;

    ProcessController *controller_;

    TargetFileOp       targetFileOp_;
};

} // NameSpace KIPIRawConverterPlugin

#endif /* BATCHDIALOG_H */
