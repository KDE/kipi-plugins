/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-02-08
 * Description : a kipi plugin to print images
 *
 * Copyright 2009 by Angelo Naselli <anaselli at linux dot it>
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

#ifndef SIGNALBLOCKER_H
#define SIGNALBLOCKER_H

// Qt includes.

#include <QObject>

namespace KIPIPrintImagesPlugin
{

  /**
   * An RAII class to block and unblock signals from a QObject instance
   */
  class SignalBlocker
  {
    public:
      SignalBlocker ( QObject* object )
      {
        mObject = object;
        mWasBlocked = object->blockSignals ( true );
      }

      ~SignalBlocker()
      {
        mObject->blockSignals ( mWasBlocked );
      }

    private:
      QObject* mObject;
      bool mWasBlocked;
  };

} // namespace

#endif /* SIGNALBLOCKER_H */
