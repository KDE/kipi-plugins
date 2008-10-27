/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-27-10
 * Description : Twain interface
 *
 * Copyright (C) 2002-2003 Stephan Stapel <stephan dot stapel at web dot de>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTWAININTERFACE_H
#define QTWAININTERFACE_H

// Qt includes.

#include <QObject>
#include <QPixmap>
#include <QImage>

// C++ includes.

#include <climits>

class CDIB;

/**
 * This class allows to implement the actual twain interfaces (QTwain class)
 *
 * A window that invokes twain operations needs to overwrite two
 * methods:
 * - showEvent(): call setParent() function to initialize the twain interface
 * - winEvent() (win32 case): Call QTwainInterface::processMessage() function
 *
 * As the winEvent() might already been invoked when still in the main window's
 * constructor and thus an uninitialized QTwainInterface-type object might be invoked
 * consecutively, you should create the QTwainInterface-type object as your
 * first operation in the window's constructor
 *
 * For proper interaction with the application, the interface has two
 * signals, pixmapAcquired() and dibAcquired(). The first of those passes a pointer
 * to a QPixmap object whereas the latter one passes a pointer to a CDIB object
 * which is more effective. Huge problems occur if acquiring a high number
 * of pixmaps in a row (see it's implementation in Qt for details).
 */
class QTwainInterface : public QObject
{
    Q_OBJECT

public:

    /**
     * Standard constructor
     *
     * @param  parent  (optional); pointer to the parent widget
     */
    QTwainInterface(QWidget* parent=0);

    /**
     * Standard destructor
     */
    virtual ~QTwainInterface();

    /**
     * Sets the parent of the twain interface. Also calls the 
     * onSetParent() hook where implementations of this interface
     * might perform custom behaviour.
     * See the class documentation for details
     *
     * @see onSetParent()
     */
    virtual void setParent(QWidget* parent);

    /**
     * Allows the selection of the device to be used, in a true Twain
     * implementation, this method opens the device selection dialog
     */
    virtual bool selectSource() = 0;

    /**
     * Acquires a number of imagesusing scanning dialog.
     */
    virtual bool acquire(unsigned int maxNumImages=UINT_MAX) = 0;

    /**
     * Returns whether the device that is currently being used
     * is valid or not.
     */
    virtual bool isValidDriver() const = 0;

    /**
     * Hook-in. See class documentation for details!
     *
     * @result    One should return false to get the message being
     *            processed by the application (should return false by default!)
     */
    virtual bool processMessage(MSG& msg) = 0;

    /**
     * Enables or disables the signal emission of the pixmap acquisition
     * signal. If disabled, the dib acquisition signal is being emitted
     * instead
     */
    void setEmitPixmaps(bool emitThem = true)
    {
        m_bEmitPixmaps = emitThem;
    }

    /**
     * Helper operation to convert a dib structure into a QPixmap
     */
    static QPixmap* convertToPixmap();
    static QPixmap* convertToPixmap(CDIB* pDib);//,unsigned int nWidth = 0,unsigned int nHeight = 0);

    /**
     * Helper operation to convert a dib structure into a QImage
     */
    static QImage* convertToImage(CDIB* pDib, unsigned int nWidth = 0, unsigned int nHeight = 0);

    static CDIB* convertToDib(QPixmap* pPixmap, unsigned int nWidth = 0, unsigned int nHeight = 0);

protected:

    virtual bool onSetParent();

    /**
     * Returns whether the components emits pixmap pointers or dib
     * pointers
     */
    inline bool emitPixmaps() const
    {
        return m_bEmitPixmaps;
    }

signals:

    /**
     * This signal is being emitted when in pixmap mode (m_bEmitPixmaps is true).
     *
     * Be sure to delete the object that is being passed in the respective
     * slot as this doesn't happen in this class or it's implementations!
     *
     * @see dibAcquired()
     */
    void pixmapAcquired(QPixmap* pPixmap);

    /**
     * This signal is being emitted when in dib mode (m_bEmitPixmaps is false).
     *
     * Be sure to delete the object that is being passed in the respective
     * slot as this doesn't happen in this class or it's implementations!
     *
     * @see pixmapAcquired()
     */
    void dibAcquired(CDIB* pDib);

protected:

    bool     m_bEmitPixmaps;

    QWidget* m_pParent;
};

#endif /* QTWAININTERFACE_H */
