#include "ImageFileDialog.h"

#include <kmessagebox.h>
#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

QString ImageFileDialog::FILTER =   "*.bpm|(*.bmp) Windows Bitmap\n"
                                    "*.jpg|(*.jpg) JPG\n"
                                    "*.jpeg|(*.jpeg) JPEG\n"
                                    "*.png|(*.png) Portable Network Graphics\n"
                                    "*.ppm|(*.ppm) Portable Pixmap\n"
                                    "*.tif|(*.tif) Tagged Image File Format\n"
                                    "*.xbm|(*.xbm) X11 Bitmap\n"
                                    "*.xpm|(*.xpm) X11 Pixmap";

ImageFileDialog::ImageFileDialog(const KUrl & startDir, QWidget * parent, QWidget * widget) :
    KFileDialog(startDir, FILTER, parent, widget),
    m_format(0)
{
    setMode(KFile::File);
    setKeepLocation(true);
}

int ImageFileDialog::exec()
{
    int result = KFileDialog::exec();
    QString filter = this->currentFilter();
    if (filter == "*.bpm")
        m_format = "BMP";
    else if (filter == "*.jpg")
        m_format = "JPG";
    else if (filter == "*.jpeg")
        m_format = "JPEG";
    else if (filter == "*.png")
        m_format = "PNG";
    else if (filter == "*.ppm")
        m_format = "PPM";
    else if (filter == "*.tif")
        m_format = "TIFF";
    else if (filter == "*.xbm")
        m_format = "XBM";
    else if (filter == "*.xpm")
        m_format = "XPM";
    else
    {
        m_format = 0;
        KMessageBox::error(this,
                           i18n("Currently this file type (%1) is unsupported.\nPleas notify the author and ask for it in the next versions of the application.", filter.toAscii().constData()),
                           i18n("The image can't be saved!"));
        return KFileDialog::Rejected;
    }

    return result;
}

const char * ImageFileDialog::format()
{
    return m_format;
}
