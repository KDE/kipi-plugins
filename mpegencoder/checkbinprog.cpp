//////////////////////////////////////////////////////////////////////////////
//
//    CHECKBINPROG.CPP
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at free.fr>
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
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

#include "checkbinprog.h"

// Qt includes

#include <qwidget.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>

// KDElib includes

#include <klocale.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

namespace KIPIMPEGEncoderPlugin
{

///////////////////////////////// CONSTRUCTOR ///////////////////////////////////////////////

CheckBinProg::CheckBinProg(QObject *parent)
{
  config = new KConfig("kipirc");
  config->setGroup("MPEGEncoder Settings");
  ImageMagickPath = config->readPathEntry("ImageMagickBinFolder");
  MjpegToolsPath = config->readPathEntry("MjpegToolsBinFolder");
}


/////////////////////////////// DESTRUCTOR //////////////////////////////////////////////////

CheckBinProg::~CheckBinProg()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////

int CheckBinProg::findExecutables( void )
{
  QFile montage, composite, convert, identify, ppmtoy4m, yuvscaler, mpeg2enc, mp2enc, mplex;
  int ValRet = 0;

  if (ImageMagickPath.isEmpty())
        {
        KMessageBox::error(kapp->activeWindow(), i18n("Your 'ImageMagick' binary program path is empty.\nSetting to default value. Check option's setting."));
        return (ValRet);
        }

if (MjpegToolsPath.isEmpty())
        {
        KMessageBox::error(kapp->activeWindow(), i18n("Your 'MjpegTools' binary programs path is empty.\nSetting to default value. Check option's setting."));
        return (ValRet);
        }

  QString str = "";

  // ImageMagick external programs.

  if (montage.exists(QString (ImageMagickPath+"/montage")) == FALSE)
    str = i18n("Cannot find 'montage' binary program from ImageMagick package. Please install it.");

  if (composite.exists(QString (ImageMagickPath+"/composite")) == FALSE)
    str = i18n("Cannot find 'composite' binary program from ImageMagick package. Please install it.");

  if (convert.exists(QString (ImageMagickPath+"/convert")) == FALSE)
    str = i18n("Cannot find 'convert' binary program from ImageMagick package. Please install it.");

  if (identify.exists(QString (ImageMagickPath+"/identify")) == FALSE)
    str = i18n("Cannot find 'identify' binary program from ImageMagick package. Please install it.");

  // MjpegTools external programs.

  if (ppmtoy4m.exists(QString (MjpegToolsPath+"/ppmtoy4m")) == FALSE)
    str = i18n("Cannot find 'ppmtoy4m' binary program from MjpegTools package. Please install it.");

  if (yuvscaler.exists(QString (MjpegToolsPath+"/yuvscaler")) == FALSE)
    str = i18n("Cannot find 'yuvscaler' binary program from MjpegTools package. Please install it.");

  if (mpeg2enc.exists(QString (MjpegToolsPath+"/mpeg2enc")) == FALSE)
    str = i18n("Cannot find 'mpeg2enc' binary program from MjpegTools package. Please install it.");

  if (  mplex.exists(QString (MjpegToolsPath+"/mplex")) == FALSE)
    str = i18n("Cannot find 'mplex' binary program from MjpegTools package. Please install it.");

  if (mp2enc.exists(QString (MjpegToolsPath+"/mp2enc")) == FALSE)
    {
    str = i18n("Cannot find 'mp2enc' binary program from MjpegTools package. Please install it.\n"
               "Audio capabilities will be disabled.");
    ValRet = 2;
    }

  if ( !str.isEmpty() )
    {
    str += i18n("\nCheck your installation and this plugin's options settings."
                 "\n\nVisit these URLs for more information:"
                 "\nImageMagick package: http://www.imagemagick.org/"
                 "\nMjpegTools package: http://mjpeg.sourceforge.net/");
    KMessageBox::error(kapp->activeWindow(), str);
    return (ValRet);
    }

  return (1);
}

}  // NameSpace KIPIMPEGEncoderPlugin

#include "checkbinprog.moc"
