/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-27-10
 * Description : Twain interface
 *
 * Copyright (C) 2002-2003 Stephan Stapel <stephan dot stapel at web dot de>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef QTWAIN_H
#define QTWAIN_H

// Qt includes

#include <QWidget>
#include <QImage>

// C++ includes

#include <climits>

// Local includes

#include "twainiface.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIAcquireImagesPlugin
{

const unsigned int AnyCount = UINT_MAX;

class TwainController : public QWidget, public TwainIface
{
    Q_OBJECT

public:

    TwainController(KIPI::Interface* iface=0, QWidget* parent=0);
    virtual ~TwainController();

    /**
     * Allows the selection of the device to be used, in a true Twain
     * implementation, this method opens the device selection dialog
     */
    virtual bool selectSource();

    /**
     * Acquires a number of images using scanning dialog.
     */
    virtual bool acquire(unsigned int maxNumImages=AnyCount);

    /**
     * Returns whether the device that is currently being used
     * is valid or not.
     */
    virtual bool isValidDriver() const;

    /**
     * Hook-in. See class documentation for details!
     *
     * @result    One should return false to get the message being
     *            processed by the application (should return false by default!)
     */
    virtual bool processMessage(MSG& msg);

    /**
     * Sets the parent of the twain interface. Also calls the 
     * onSetParent() hook where implementations of this interface
     * might perform custom behaviour.
     * See the class documentation for details
     *
     * @see onSetParent()
     */
    virtual void setParent(QWidget* parent);

Q_SIGNALS:

    void signalImageAcquired(const QImage& img);

protected:

    bool winEvent(MSG* pMsg, long *result);

    virtual bool onSetParent();
    virtual void CopyImage(TW_MEMREF data, TW_IMAGEINFO& info);

private:

  void saveImage(const QImage& img);

private:

    QWidget         *m_parent;

    KIPI::Interface *m_interface;
};

}  // namespace KIPIAcquireImagesPlugin

#endif /* QTWAIN_H */
