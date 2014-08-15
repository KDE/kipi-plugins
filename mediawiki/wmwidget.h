/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
 * Copyright (C) 2013      by Peter Potrowl <peter dot potrowl at gmail dot com>
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

#ifndef WMWIDGET_H
#define WMWIDGET_H

// Qt includes

#include <QWidget>
#include <QStringList>
#include <QTreeWidgetItem>

//KDE includes

#include <kconfig.h>
#include <kurl.h>

namespace KIPIPlugins
{
    class KPImagesList;
    class KPProgressWidget;
}

using namespace KIPIPlugins;

namespace KIPIWikiMediaPlugin
{

enum WmDownloadType
{
    WmMyAlbum = 0,
    WmFriendAlbum,
    WmPhotosMe,
    WmPhotosFriend
};

class WmWidget : public QWidget
{
    Q_OBJECT

public:

    WmWidget(QWidget* const parent);
    ~WmWidget();

    void updateLabels(const QString& userName = QString(), const QString& wikiName = QString(), const QString& url = QString());
    void invertAccountLoginBox();

    KPImagesList*     imagesList()  const;
    KPProgressWidget* progressBar() const;

    int  dimension()        const;
    int  quality()          const;
    bool resize()           const;
    bool removeMeta()       const;
    bool removeGeo()        const;

    QString author()        const;
    QString source()        const;
    QString genCategories() const;
    QString genText()       const;
    QString genComments()   const;
    QString license()       const;
    QString categories()    const;
    QString title()         const;
    QString description()   const;
    QString date()          const;
    QString latitude()      const;
    QString longitude()     const;

    QMap <QString, QMap <QString, QString> > allImagesDesc();

    void clearImagesDesc();
    void readSettings(KConfigGroup& group);
    void saveSettings(KConfigGroup& group);
    void loadImageInfoFirstLoad();
    void clearEditFields();

Q_SIGNALS:

    void signalChangeUserRequest();
    void signalLoginRequest(const QString& login, const QString& pass, const QString& wikiName, const QUrl& wikiUrl);

private Q_SLOTS:

    void slotResizeChecked();
    void slotRemoveMetaChecked();
    void slotChangeUserClicked();
    void slotLoginClicked();
    void slotNewWikiClicked();
    void slotAddWikiClicked();
    void slotLoadImagesDesc(QTreeWidgetItem* item);
    void slotRemoveImagesDesc(const KUrl::List urls);
    void slotRestoreExtension();
    void slotApplyTitle();
    void slotApplyDate();
    void slotApplyCategories();
    void slotApplyDescription();
    void slotApplyLatitude();
    void slotApplyLongitude();

private:

    class Private;
    Private* const d;

    friend class WmWindow;
};

} // namespace KIPIWikiMediaPlugin

#endif // WMWIDGET_H
