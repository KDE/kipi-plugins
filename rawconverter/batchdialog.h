/* ============================================================
 * File  : batchdialog.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-10-24
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
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

#include <qdialog.h>
#include <qstringlist.h>
#include <qdict.h>

class KFileItem;
class KListView;
class KListViewItem;
class KURL;
class KProgress;

class QListViewItem;
class QCheckBox;
class QPushButton;
class QVButtonGroup;
class QLabel;
class QPixmap;

namespace KIPI
{
class ThumbnailJob;
}

namespace RawConverter
{

class  CSpinBox;
class  ProcessController;
struct RawItem;

class BatchDialog : public QDialog
{

    Q_OBJECT

public:

    enum TargetFileOp {
        OVERWRITE,
        OPENFILEDIALOG
    };

    BatchDialog();
    ~BatchDialog();

    void addItems(const QStringList& itemList);

private:

    void readSettings();
    void saveSettings();

    void processOne();

    KListView*     listView_;
    QCheckBox*     cameraWBCheckBox_;
    QCheckBox*     fourColorCheckBox_;
    CSpinBox*      gammaSpinBox_;
    CSpinBox*      brightnessSpinBox_;
    CSpinBox*      redSpinBox_;
    CSpinBox*      blueSpinBox_;

    QVButtonGroup* saveButtonGroup_;
    QVButtonGroup* conflictButtonGroup_;

    KProgress*     progressBar_;

    QPushButton*   helpButton_;
    QPushButton*   aboutButton_;
    QPushButton*   processButton_;
    QPushButton*   closeButton_;
    QPushButton*   abortButton_;

    QDict<RawItem>         itemDict_;
    ProcessController     *controller_;
    bool                   busy_;
    QStringList            fileList_;

    QString                targetExtension_;
    TargetFileOp           targetFileOp_;


private slots:

    void slotSaveFormatChanged();

    void slotHelp();
    void slotAbout();

    void slotProcess();
    void slotAbort();

    void slotIdentify();
    void slotIdentified(const QString& file, const QString& identity);
    void slotIdentifyFailed(const QString& file, const QString& identity);

    void slotProcessing(const QString& file);
    void slotProcessed(const QString& file, const QString& tmpFile);
    void slotProcessingFailed(const QString& file);

    void slotBusy(bool busy);
    void slotGotThumbnail(const KFileItem* url, const QPixmap& pix);
};

}

#endif /* BATCHDIALOG_H */
