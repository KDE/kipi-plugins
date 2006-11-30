//////////////////////////////////////////////////////////////////////////////
//
//    KIMG2MPG.H
//
//    Copyright (C) 2003 Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef KImg2mpgData_included
#define KImg2mpgData_included

// Include files for Qt

#include <qstring.h>
#include <qcolor.h>
#include <qtimer.h>
#include <qguardedptr.h>
#include <qdatetime.h>

// Include files for KDE

#include <kdialog.h>
#include <klistbox.h>
#include <kprocess.h>
#include <kio/previewjob.h>

// Include files for KIPI

#include "kpaboutdata.h"
#include <libkipi/interface.h>

// Local includes

#include "kimg2mpgbase.h"

class KFileItem;
class QPushButton;
class QComboBox;
class QSpinBox;
class QGroupBox;
class QListBoxItem;
class QLabel;
class QWidget;
class QPixmap;

class KConfig;
class KProcess;
class KLineEdit;
class KIconLoader;
class KColorButton;
class KListBox;
class KButtonBox;
class KProgress;
class KURL::List;

namespace KIPIMPEGEncoderPlugin
{

class KShowDebuggingOutput;
class OptionsDialog;

class ListImageItems : public KListBox
{
Q_OBJECT

public:
    ListImageItems(QWidget *parent=0, const char *name=0);

signals:
    void addedDropItems(KURL::List filesUrl);

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
};


class KImg2mpgData : public KImg2mpgBase
{
Q_OBJECT

public:

  KImg2mpgData( KIPI::Interface* interface, QWidget* parent = 0, const char * name = 0 );
  virtual ~KImg2mpgData();

  void show();
  void ShowNumberImages( int Number );
  void closeEvent(QCloseEvent*);
  QPixmap LoadIcon( QString Name, int Group );
  void addItems(const KURL::List& fileList);
  void writeSettings();
  void readSettings();

  OptionsDialog* m_OptionDlg;

public slots:

  void reset();
  void readStderr(KProcess *proc, char *buffer, int buflen);
  void EncodeDone(KProcess* );
  void slotMPEGFilenameDialog( void );
  void slotAudioFilenameDialog( void );
  void slotImagesFilesButtonAdd( void );
  void slotImagesFilesButtonDelete( void );
  void slotImagesFilesButtonUp( void );
  void slotImagesFilesButtonDown( void );
  void slotEncode( void );
  void slotOptions( void );
  void slotClose( void );
  void slotHelp( void );
  void slotImagesFilesSelected( QListBoxItem *item );
  void SlotPortfolioDurationChanged ( int );
  void slotOptionDlgOkClicked( void );
  void slotGotPreview(const KFileItem* , const QPixmap &pixmap);
  void slotFailedPreview(const KFileItem*);
  void slotAddDropItems(KURL::List filesUrl);
  
private:
  
  QString               m_VideoFormatConfig;
  QString               m_VideoTypeConfig;
  QString               m_ChromaConfig;
  QString               m_ImageDurationConfig;
  QString               m_TransitionSpeedConfig;
  QString               m_MPEGOutputFileConfig;
  QString               m_AudioInputFileConfig;
  QString               m_IMBinFolderConfig;
  QString               m_MJBinFolderConfig;
  QString               m_TmpFolderConfig;
  QString               m_NoneLabel;
  QString               m_EncodeString;
  QString               m_DebugOuputMessages;
  QString               m_CommandLine;
  QString               m_ImagesFilesSort;

  QColor                m_BackgroundColorConfig;

  KConfig*              m_config;

  KIO::PreviewJob*      m_thumbJob;

  QTime                 m_EncodingDuration;
  QTime                 m_DurationTime;

  bool                  m_Abort;
  bool                  m_Encoding;

  pid_t                 m_Img2mpgPidNum;

  KProcess*             m_Proc;

  KIconLoader*          m_Icons;

  KIPI::Interface*      m_interface;

  KIPIPlugins::KPAboutData* m_about;

  class KShowDebuggingOutput* m_DebuggingDialog;

  void RemoveTmpFiles( void );
  bool DeleteDir(QString dirname);
  bool deldir(QString dirname);
};

}  // NameSpace KIPIMPEGEncoderPlugin

#endif // KImg2mpgData_included
