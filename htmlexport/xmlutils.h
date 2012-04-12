/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : A KIPI plugin to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free.fr>
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

// vim: set tabstop=4 shiftwidth=4 noexpandtab:

#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <kdebug.h>

#include <libxml/xmlwriter.h>

namespace KIPIHTMLExport
{

/**
 * A simple wrapper for a C structure pointed to by @Ptr, which must be freed
 * with @freeFcn
 */
template <class Ptr, void(*freeFcn)(Ptr)>
class CWrapper {
public:
    CWrapper() : mPtr(0) {}
    CWrapper(Ptr ptr)
    : mPtr(ptr) {}

    ~CWrapper() {
        freeFcn(mPtr);
    }

    operator Ptr() const {
        return mPtr;
    }

    bool operator!() const {
        return !mPtr;
    }

    void assign(Ptr ptr) {
        if (mPtr) freeFcn(mPtr);
        mPtr=ptr;
    }

private:
    Ptr mPtr;
};


/**
 * Simple wrapper around xmlTextWriter
 */
class XMLWriter {
public:
    bool open(const QString& name) {
        xmlTextWriterPtr ptr=xmlNewTextWriterFilename(name.toLocal8Bit(), 0);
        if (!ptr) return false;
        mWriter.assign(ptr);

        int rc=xmlTextWriterStartDocument(ptr, NULL, "UTF-8", NULL);
        if (rc<0) {
            mWriter.assign(0);
            return false;
        }

        xmlTextWriterSetIndent(ptr, 1);

        return true;
    }

    operator xmlTextWriterPtr() const {
        return mWriter;
    }

    void writeElement(const char* element, const QString& value) {
        xmlTextWriterWriteElement(mWriter, BAD_CAST element, BAD_CAST value.toUtf8().data());
    }

    void writeElement(const char* element, int value) {
        writeElement(element, QString::number(value));
    }

private:
    CWrapper<xmlTextWriterPtr,xmlFreeTextWriter> mWriter;
};


/**
 * A list of attributes for an XML element. To be used with @ref XMLElement
 */
class XMLAttributeList {
    typedef QMap<QString, QString> Map;
public:
    void write(XMLWriter& writer) const {
        Map::const_iterator it=mMap.begin();
        Map::const_iterator end=mMap.end();
        for (; it!=end; ++it) {
            xmlTextWriterWriteAttribute(writer,
                BAD_CAST it.key().toAscii().data(),
                BAD_CAST it.value().toUtf8().data());
        }
    }
    
    void append(const QString& key, const QString& value) {
        mMap[key]=value;
    }
    
    void append(const QString& key, int value) {
        mMap[key]=QString::number(value);
    }

private:
    Map mMap;
};


/**
 * A class to generate an XML element
 */
class XMLElement {
public:
    XMLElement(XMLWriter& writer, const QString& element, const XMLAttributeList* attributeList=0)
    : mWriter(writer)
    {
        xmlTextWriterStartElement(writer, BAD_CAST element.toAscii().data());
        if (attributeList) {
            attributeList->write(writer);
        }
    }

    ~XMLElement() {
        xmlTextWriterEndElement(mWriter);
    }

private:
    XMLWriter& mWriter;
};

} // namespace

#endif /* XMLUTILS_H */
