/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2006-09-19
 * @brief  GPS data file parser (GPX format http://www.topografix.com/gpx.asp).
 *
 * @author Copyright (C) 2006-2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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


#ifndef GPSDATAPARSER_P_H
#define GPSDATAPARSER_P_H

// Qt includes

#include <QXmlDefaultHandler>

namespace KIPIGPSSyncPlugin
{

class GPXFileReader : public QXmlDefaultHandler
{
public:
    GPXFileReader(GPSDataParser::GPXFileData* const dataTarget);

    virtual bool characters(const QString& ch);
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);

    static GPSDataParser::GPXFileData loadGPXFile(const KUrl& url);

private:
    void rebuildElementPath();
    static QString myQName(const QString& namespaceURI, const QString& localName);
    static QDateTime ParseTime(QString timeString);

private:
    GPSDataParser::GPXFileData* const fileData;
    QString currentElementPath;
    QStringList currentElements;
    QString currentText;
    GPSDataParser::GPXDataPoint currentDataPoint;
    bool verifyFoundGPXElement;

    friend class ::TestGPXParsing;
};

} /* KIPIGPSSyncPlugin */

#endif /* GPSDATAPARSER_P_H */
