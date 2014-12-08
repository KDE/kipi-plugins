/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef CONVERTOPTIONSDIALOG_H
#define CONVERTOPTIONSDIALOG_H

// Qt includes

#include <QLabel>
#include <QString>

// KDE includes

#include <kdialog.h>

class QCheckBox;

class KComboBox;
class KIntNumInput;

namespace KIPIBatchProcessImagesPlugin
{

class ConvertOptionsDialog : public KDialog
{
    Q_OBJECT

public:

    explicit ConvertOptionsDialog(QWidget *parent = 0, int ImageFormatType = 0);
    ~ConvertOptionsDialog();

    QLabel          *m_label_imageCompression;
    KIntNumInput    *m_JPEGPNGCompression;
    QCheckBox       *m_compressLossLess;
    KComboBox       *m_TIFFCompressionAlgo;
    KComboBox       *m_TGACompressionAlgo;

public Q_SLOTS:

    void slotCompressLossLessEnabled(bool val);
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif  // CONVERTOPTIONSDIALOG_H
