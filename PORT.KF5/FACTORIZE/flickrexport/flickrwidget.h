/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef FLICKRWIDGET_H
#define FLICKRWIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "comboboxintermediate.h"
#include "flickrlist.h"

class QGroupBox;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QLabel;
class QTreeWidgetItem;

class KComboBox;
class KTabWidget;
class KLineEdit;
class KHTMLPart;

namespace KIPIPlugins
{
    class KPProgressWidget;
}

namespace KIPIFlickrExportPlugin
{

class FlickrList;

class FlickrWidget : public QWidget
{
    Q_OBJECT

public:

    enum SettingsTab
    {
        FILELIST = 0,
        UPLOAD
    };

public:

    FlickrWidget(QWidget* const parent, const QString& serviceName);
    ~FlickrWidget();

    KIPIPlugins::KPProgressWidget* progressBar() const;

protected:

    virtual void showEvent(QShowEvent* event);

private Q_SLOTS:

    void slotResizeChecked();
    void slotOriginalChecked();
    void slotPermissionChanged(FlickrList::FieldType, Qt::CheckState);
    void slotSafetyLevelChanged(FlickrList::SafetyLevel);
    void slotContentTypeChanged(FlickrList::ContentType);
    void slotMainPublicToggled(int);
    void slotMainFamilyToggled(int);
    void slotMainFriendsToggled(int);
    void slotMainSafetyLevelChanged(int);
    void slotMainContentTypeChanged(int);
    void slotExtendedPublicationToggled(bool);
    void slotExtendedTagsToggled(bool);
    void slotAddExtraTagsToggled(bool);


private: // Functions

    void mainPermissionToggled(FlickrList::FieldType, Qt::CheckState);

private: // Data

    KTabWidget*                         m_tab;

    KHTMLPart*                          m_photoView;

    QLabel*                             m_userNameDisplayLabel;

    QPushButton*                        m_changeUserButton;
    QPushButton*                        m_removeAccount;
    QPushButton*                        m_newAlbumBtn;
    QPushButton*                        m_reloadphotoset;
    QPushButton*                        m_extendedTagsButton;
    QPushButton*                        m_extendedPublicationButton;

    KComboBox*                          m_albumsListComboBox;

    QCheckBox*                          m_exportHostTagsCheckBox;
    QCheckBox*                          m_stripSpaceTagsCheckBox;
    QCheckBox*                          m_addExtraTagsCheckBox;
    QCheckBox*                          m_familyCheckBox;
    QCheckBox*                          m_friendsCheckBox;
    QCheckBox*                          m_publicCheckBox;
    QCheckBox*                          m_resizeCheckBox;
    QCheckBox*                          m_sendOriginalCheckBox;

    QGroupBox*                          m_extendedTagsBox;
    QGroupBox*                          m_extendedPublicationBox;
    QGroupBox*                          m_resizingBox;

    KLineEdit*                          m_tagsLineEdit;

    ComboBoxIntermediate*               m_contentTypeComboBox;
    ComboBoxIntermediate*               m_safetyLevelComboBox;

    QSpinBox*                           m_dimensionSpinBox;
    QSpinBox*                           m_imageQualitySpinBox;

    KIPIFlickrExportPlugin::FlickrList* m_imglst;
    KIPIPlugins::KPProgressWidget*      m_progressBar;

    friend class FlickrWindow;
};

} // namespace KIPIFlickrExportPlugin

#endif // FLICKRWIDGET_H
