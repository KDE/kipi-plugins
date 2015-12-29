/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-04
 * Description : Tool dialog
 *
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPTOOLDIALOG_H
#define KPTOOLDIALOG_H

// KDE includes

#include <QWizard>
#include <QDialogButtonBox>

// Local includes

#include "kipiplugins_export.h"

class QAbstractButton;
class QPushButton;
class QDialog;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIPlugins
{

class KPAboutData;

class KIPIPLUGINS_EXPORT KPDialogBase
{
public:

    KPDialogBase(QDialog* const dlg);
    virtual ~KPDialogBase();

    void setAboutData(KPAboutData* const data, QPushButton* help=0);

    Interface* iface() const;

private:

    QPushButton* getHelpButton() const;

private:

    class Private;
    Private* const d;
};

// -----------------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPToolDialog : public QDialog, public KPDialogBase
{
    Q_OBJECT

public:

    KPToolDialog(QWidget* const parent = 0);
    virtual ~KPToolDialog();

    void setMainWidget(QWidget* const widget);

    void setRejectButtonMode(QDialogButtonBox::StandardButton button);

    QPushButton* startButton() const;

    void addButton(QAbstractButton* button, QDialogButtonBox::ButtonRole role);

private Q_SLOTS:

    void slotCloseClicked();

Q_SIGNALS:

    void cancelClicked();

private:

    QPushButton* helpButton() const;

private:

    friend class KPDialogBase;

    class Private;
    Private* const d;
};

// -----------------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPWizardDialog : public QWizard, public KPDialogBase
{
public:

    KPWizardDialog(QWidget* const parent=0);
    virtual ~KPWizardDialog();

private:
    
    QPushButton* helpButton() const;
    
    friend class KPDialogBase;
};

} // namespace KIPIPlugins

#endif /* KPTOOLDIALOG_H */
