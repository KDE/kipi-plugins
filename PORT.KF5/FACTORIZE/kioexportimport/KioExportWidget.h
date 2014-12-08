/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-09-28
 * Description : a tool to export image to a KIO accessible
 *               location
 *
 * Copyright (C) 2006-2009 by Johannes Wienke <languitar at semipol dot de>
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

#ifndef KIOEXPORTWIDGET_H
#define KIOEXPORTWIDGET_H

// Qt includes

#include <QWidget>
#include <QLabel>

// KDE includes

#include <kfiledialog.h>
#include <kpushbutton.h>
#include <kurl.h>
#include <kurlrequester.h>

class KComboBox;

namespace KIPIPlugins
{
    class KPImagesList;
}

using namespace KIPIPlugins;

namespace KIPIKioExportPlugin
{

class KioExportWidget: public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent the parent widget
     */
    explicit KioExportWidget(QWidget* const parent);

    /**
     * Destructor.
     */
    virtual ~KioExportWidget();

    /**
     * Returns a pointer to the imagelist that is displayed.
     */
    KPImagesList* imagesList() const;

    /**
     * Returns the currently selected target url. Maybe invalid.
     */
    KUrl targetUrl() const;

    /**
     * Sets the target url this widget should point at.
     */
    void setTargetUrl(const KUrl& url);

    KUrl::List history() const;
    void setHistory(const KUrl::List& urls);

private Q_SLOTS:

    void slotLabelUrlChanged();
    void slotShowTargetDialogClicked(bool checked);

Q_SIGNALS:

    void signalTargetUrlChanged(const KUrl& target);

private:

    void updateTargetLabel();

private:

    KUrlComboRequester* m_targetLabel;
    KFileDialog*        m_targetDialog;
    KPushButton*        m_targetSearchButton;
    KUrl                m_targetUrl;
    KPImagesList*       m_imageList;
};

} // namespace KIPIKioExportPlugin

#endif /* KIOEXPORTWIDGET_H */
