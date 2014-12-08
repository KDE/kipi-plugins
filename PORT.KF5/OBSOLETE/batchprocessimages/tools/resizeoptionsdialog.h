/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 18.10.2009
 * Description : option dialogs for resizing
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
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

#ifndef RESIZEOPTIONSDIALOG_H
#define RESIZEOPTIONSDIALOG_H

// Qt includes

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>

// KDE includes

#include <kdialog.h>
#include <kcombobox.h>
#include <knuminput.h>
#include <kcolorbutton.h>

// Local inclues

#include "resizecommandbuilder.h"

namespace KIPIBatchProcessImagesPlugin
{

/**
 * Base class for option dialogs that configure a resize process. This class
 * already has support for managing the image quality consisting of a
 * quality and a filter value.
 */
class ResizeOptionsBaseDialog : public KDialog
{
    Q_OBJECT

public:

    /**
     * Constructor. All subclasses must have a constructor with only the first
     * two arguments.
     *
     * @param parent parent
     * @param commandBuilder command builder who's options are managed by this
     *                       dialog
     * @param settingsPrefix prefix prepended to the setting names managed by
     *                       this base class. This is used to have e.g. separate
     *                       quality settings per tool
     */
    ResizeOptionsBaseDialog(QWidget* const parent,
                            ResizeCommandBuilder* const commandBuilder,
                            const QString& settingsPrefix);
    virtual ~ResizeOptionsBaseDialog();

    /**
     * Clients of this class must call this method before executing the dialog
     * to start the layout process.
     */
    void layout();

    /**
     * Subclasses must implement this method to return a localized string that
     * describes the actions this tool does.
     *
     * @return description of the resize operation this type does
     */
    virtual QString getWhatsThis() = 0;

    /**
     * Restore settings for this resize type. Subclasses must implement this
     * method and call the base classes' version. It is important that the
     * settings are applied to the command builder after reading them.
     *
     * @param rcname name of the rc file for KConfig
     * @param groupName name of the config group to use
     */
    virtual void readSettings(const QString& rcname, const QString& groupName);

    /**
     * Store settings for this resize type. Subclasses must implement this
     * method and call the base classes' version.
     *
     * @param rcname name of the rc file for KConfig
     * @param groupName name of the config group to use
     */
    virtual void saveSettings(const QString& rcname, const QString& groupName);

protected:

    /**
     * Implement this template method to prepend widgets before the quality
     * settings on the dialog's main widget. For this purpose use
     * addOptionWidget.
     */
    virtual void prependWidgets() = 0;

    /**
     * Implement this template method to append widgets after the quality
     * settings on the dialog's main widget. For this purpose use
     * addOptionWidget.
     */
    virtual void appendWidgets() = 0;

    /**
     * Adds a new widget with options to this dialog managing the overall layout
     * of the dialog.
     *
     * @param widget widget to add
     */
    void addOptionWidget(QWidget *widget);

    /**
     * Implement this template method to react on a click to the ok button of
     * the dialog. This is meant to set the options for the command builder.
     *
     * @return return <code>true</code> if everything of the private handling
     *         was successful so that the base class can continue its own
     *         processing, false means there was an error, the dialog cannot
     *         be closed and options should not be transferred to the command
     *         builder
     */
    virtual bool handleOk() = 0;

private Q_SLOTS:

    void slotOk();

private:

    const static QString OPTION_QUALITY_NAME;
    const static QString OPTION_FILTER_NAME;

    QString               m_settingsPrefix;

    QString               m_defaultFilterName;

    ResizeCommandBuilder *m_commandBuilder;

    QWidget              *m_mainWidget;
    QVBoxLayout          *m_mainWidgetLayout;

    // quality settings
    QLabel               *m_resizeFilterLabel;
    KComboBox            *m_resizeFilterComboBox;
    KIntNumInput         *m_qualityInput;
};

/**
 * Options for one dim proportional resize.
 */
class OneDimResizeOptionsDialog: public ResizeOptionsBaseDialog
{
    Q_OBJECT

public:
    OneDimResizeOptionsDialog(QWidget *parent, OneDimResizeCommandBuilder *commandBuilder);
    virtual ~OneDimResizeOptionsDialog();

    QString getWhatsThis();

    virtual void readSettings(const QString& rcname, const QString& groupName);
    virtual void saveSettings(const QString& rcname, const QString& groupName);

private:

    void prependWidgets();
    void appendWidgets();
    bool handleOk();

private:

    const static QString OPTION_SIZE_NAME;

    OneDimResizeCommandBuilder *m_commandBuilder;

    KIntNumInput               *m_sizeInput;
};

/**
 * Options for two dim proportional resize.
 */
class TwoDimResizeOptionsDialog: public ResizeOptionsBaseDialog
{
Q_OBJECT

public:

    TwoDimResizeOptionsDialog(QWidget *parent, TwoDimResizeCommandBuilder *commandBuilder);
    virtual ~TwoDimResizeOptionsDialog();

    QString getWhatsThis();

    virtual void readSettings(const QString& rcname, const QString& groupName);
    virtual void saveSettings(const QString& rcname, const QString& groupName);

private:

    void prependWidgets();
    void appendWidgets();
    bool handleOk();

private:

    const static QString OPTION_WIDTH_NAME;
    const static QString OPTION_HEIGHT_NAME;
    const static QString OPTION_FILL_NAME;
    const static QString OPTION_FILL_COLOR_NAME;

    TwoDimResizeCommandBuilder *m_commandBuilder;

    KIntNumInput               *m_widthInput;
    KIntNumInput               *m_heightInput;
    QCheckBox                  *m_fillCheckBox;
    QLabel                     *m_fillColorLabel;
    KColorButton               *m_fillColorButton;
};

/**
 * Options for non-proportional resize.
 */
class NonProportionalResizeOptionsDialog: public ResizeOptionsBaseDialog
{
    Q_OBJECT

public:

    NonProportionalResizeOptionsDialog(QWidget *parent, NonProportionalResizeCommandBuilder *commandBuilder);
    virtual ~NonProportionalResizeOptionsDialog();

    QString getWhatsThis();

    virtual void readSettings(const QString& rcname, const QString& groupName);
    virtual void saveSettings(const QString& rcname, const QString& groupName);

private:

    void prependWidgets();
    void appendWidgets();
    bool handleOk();

private:

    const static QString OPTION_WIDTH_NAME;
    const static QString OPTION_HEIGHT_NAME;

    NonProportionalResizeCommandBuilder *m_commandBuilder;

    KIntNumInput                        *m_widthInput;
    KIntNumInput                        *m_heightInput;
};

/**
 * Options for print preview resize.
 */
class PrintPrepareResizeOptionsDialog: public ResizeOptionsBaseDialog
{
    Q_OBJECT

public:

    PrintPrepareResizeOptionsDialog(QWidget *parent, PrintPrepareResizeCommandBuilder *commandBuilder);
    virtual ~PrintPrepareResizeOptionsDialog();

    QString getWhatsThis();

    virtual void readSettings(const QString& rcname, const QString& groupName);
    virtual void saveSettings(const QString& rcname, const QString& groupName);

private Q_SLOTS:

    void slotCustomSettingsEnabled(bool enable);

private:

    void prependWidgets();
    void appendWidgets();
    bool handleOk();

private:

    const static QString OPTION_PAPER_SIZE_NAME;
    const static QString OPTION_DPI_NAME;
    const static QString OPTION_CUSTOM_PAPER_WIDTH_NAME;
    const static QString OPTION_CUSTOM_PAPTER_HEIGHT_NAME;
    const static QString OPTION_CUSTOM_DPI_NAME;
    const static QString OPTION_STRETCH_NAME;
    const static QString OPTION_CUSTOM_SETTINGS_NAME;

    PrintPrepareResizeCommandBuilder *m_commandBuilder;

    QLabel                           *m_paperSizeLabel;
    QLabel                           *m_dpiLabel;
    QLabel                           *m_customPaperWidthLabel;
    QLabel                           *m_customPaperHeightLabel;
    QLabel                           *m_customDpiLabel;

    KIntNumInput                     *m_customPaperWidthInput;
    KIntNumInput                     *m_customPaperHeightInput;
    KIntNumInput                     *m_customDpiInput;

    KComboBox                        *m_paperSizeComboBox;
    KComboBox                        *m_dpiComboBox;

    QCheckBox                        *m_customSettingsCheckBox;
    QCheckBox                        *m_stretchCheckBox;
};

} // namespace KIPIBatchProcessImagesPlugin

#endif /* RESIZEOPTIONSDIALOG_H */
