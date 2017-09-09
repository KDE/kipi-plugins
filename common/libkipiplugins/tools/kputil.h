/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2014-09-12
 * Description : Simple helpher widgets collection
 *
 * Copyright (C) 2015      by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2014-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPUTIL_H
#define KPUTIL_H

// Qt includes

#include <QString>
#include <QDir>
#include <QUrl>
#include <QFrame>
#include <QLineEdit>
#include <QSize>
#include <QPixmap>
#include <QFileDialog>
#include <QColor>
#include <QPushButton>
#include <QProcessEnvironment>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

QDir KIPIPLUGINS_EXPORT makeTemporaryDir(const char* prefix);
void KIPIPLUGINS_EXPORT removeTemporaryDir(const char* prefix);

// ------------------------------------------------------------------------------------

/** An Horizontal widget to host children widgets
 */
class KIPIPLUGINS_EXPORT KPHBox : public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(KPHBox)

public:

    explicit KPHBox(QWidget* const parent=0);
    virtual ~KPHBox();

    void setSpacing(int space);
    void setContentsMargins(const QMargins& margins);
    void setContentsMargins(int left, int top, int right, int bottom);
    void setStretchFactor(QWidget* const widget, int stretch);

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

protected:

    KPHBox(bool vertical, QWidget* const parent);

    void childEvent(QChildEvent* e) Q_DECL_OVERRIDE;
};

// ------------------------------------------------------------------------------------

/** A Vertical widget to host children widgets
 */
class KIPIPLUGINS_EXPORT KPVBox : public KPHBox
{
    Q_OBJECT
    Q_DISABLE_COPY(KPVBox)

  public:

    explicit KPVBox(QWidget* const parent=0);
    virtual ~KPVBox();
};

// --------------------------------------------------------------------------------------

/** A widget to draw progress wheel indicator over thumbnails.
 */
class KIPIPLUGINS_EXPORT KPWorkingPixmap
{
public:

    explicit KPWorkingPixmap();
    ~KPWorkingPixmap();

    bool    isEmpty()          const;
    QSize   frameSize()        const;
    int     frameCount()       const;
    QPixmap frameAt(int index) const;

private:

    QVector<QPixmap> m_frames;
};

// ------------------------------------------------------------------------------------

/** A widget to chosse a single local file or path.
 *  Use line edit and file dialog properties to customize operation modes.
 */
class KIPIPLUGINS_EXPORT KPFileSelector : public KPHBox
{
    Q_OBJECT

public:

    explicit KPFileSelector(QWidget* const parent=0);
    virtual ~KPFileSelector();

    QLineEdit* lineEdit() const;

    void setFileDlgMode(QFileDialog::FileMode mode);
    void setFileDlgFilter(const QString& filter);
    void setFileDlgTitle(const QString& title);
    void setFileDlgOptions(QFileDialog::Options opts);

Q_SIGNALS:

    void signalOpenFileDialog();
    void signalUrlSelected(const QUrl&);

private Q_SLOTS:

    void slotBtnClicked();

private:

    class Private;
    Private* const d;
};

// ------------------------------------------------------------------------------------

/** A widget to chosse a color from a palette.
 */
class KIPIPLUGINS_EXPORT KPColorSelector : public QPushButton
{
    Q_OBJECT

public:

    explicit KPColorSelector(QWidget* const parent=0);
    virtual ~KPColorSelector();

    void setColor(const QColor& color);
    QColor color() const;

Q_SIGNALS:

    void signalColorSelected(const QColor&);

private Q_SLOTS:

    void slotBtnClicked();

private:

    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

private:

    class Private;
    Private* const d;
};

// ---------------------------------------------------------------------------------------

/** Generates random string
 */

class KIPIPLUGINS_EXPORT KPRandomGenerator
{
public:

    explicit KPRandomGenerator();
    ~KPRandomGenerator();

    static QString randomString(const int& length);
};

// ---------------------------------------------------------------------------------------

/** If plugins run into AppImage, return a cleaned environnement for QProcess to execute a
 *  program outside the bundle without broken run-time dependencies.
 *  If plugins do not run as AppImage bundle, this method return a QProcessEnvironment instance
 *  based on system environment.
 */
KIPIPLUGINS_EXPORT QProcessEnvironment adjustedEnvironmentForAppImage();

} // namespace KIPIPlugins

#endif // KPUTIL_H
