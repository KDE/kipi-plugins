// Qt
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>
// Local
#include "cameralist.h"
#include "cameratype.h"

CameraList* CameraList::instance_ = 0;

CameraList* CameraList::instance() {
    return instance_;    
}

class CameraListPrivate {
    public:
	QPtrList<CameraType> clist;
	QString file;
	bool modified;
};

CameraList::CameraList(QObject *parent, const QString& file) : QObject(parent) {
    d = new CameraListPrivate;
    d->clist.setAutoDelete(true);
    d->file = file;
    d->modified = false;
    instance_ = this;
}

CameraList::~CameraList() {
    close();
    d->clist.clear();
    delete d;
    instance_ = 0;
}

bool CameraList::load() {
    d->modified = false;
    QFile cfile(d->file);
    if(!cfile.open(IO_ReadOnly)) {
        return false;
	}
    QDomDocument doc("cameralist");
    if(!doc.setContent(&cfile)) {
        return false;
	}
    QDomElement docElem = doc.documentElement();
    if(docElem.tagName()!="cameralist") {
        return false;
    }
    for(QDomNode n = docElem.firstChild(); !n.isNull(); n = n.nextSibling()) {
        QDomElement e = n.toElement();
        if (e.isNull()) { 
			continue;
		}
        if (e.tagName() != "item") {
			continue;
		}
        QString title  = e.attribute("title");
        QString model  = e.attribute("model");
        QString port   = e.attribute("port");
        QString path   = e.attribute("path");
        CameraType *ctype = new CameraType(title, model, port, path);
        insertPrivate(ctype);
    }
    return true;
}

bool CameraList::close() {
    // If not modified don't save the file
    if (!d->modified) {
        return true;
	}
    QDomDocument doc("cameralist");
    doc.setContent(QString("<!DOCTYPE XMLCameraList><cameralist version=\"1.0\" client=\"kipiplugin_kameraklietnt\"/>"));
    QDomElement docElem=doc.documentElement();
    for (CameraType *ctype = d->clist.first(); ctype; ctype = d->clist.next()) {
       QDomElement elem = doc.createElement("item");
       elem.setAttribute("title", ctype->title());
       elem.setAttribute("model", ctype->model());
       elem.setAttribute("port", ctype->port());
       elem.setAttribute("path", ctype->path());
       docElem.appendChild(elem);
    }
    QFile cfile(d->file);
    if (!cfile.open(IO_WriteOnly)) {
        return false;
	}
    QTextStream stream(&cfile);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    stream << doc.toString();
    cfile.close();
    return true;    
}

void CameraList::insert(CameraType* ctype) {
    if(!ctype) {
		return;
	}
    d->modified = true;    
    insertPrivate(ctype);
}

void CameraList::remove(CameraType* ctype) {
    if (!ctype) {
		return;
    }
    d->modified = true;
    removePrivate(ctype);
}

void CameraList::insertPrivate(CameraType* ctype) {
    if (!ctype) {
		return;
	}
    d->clist.append(ctype);
    emit signalCameraListChanged();    
}

void CameraList::removePrivate(CameraType* ctype) {
    if (!ctype) {
		return;
    }
    d->clist.remove(ctype);
    emit signalCameraListChanged();
}

QPtrList<CameraType>* CameraList::cameraList() {
    return &d->clist; 
}

CameraType* CameraList::find(const QString& title) {
    for (CameraType *ctype = d->clist.first(); ctype; ctype = d->clist.next()) {
        if (ctype->title() == title) {
            return ctype;
		}
    }
    return 0;
}

void CameraList::clear() {
    CameraType *ctype = d->clist.first();
    while (ctype) {
        remove(ctype);
        ctype = d->clist.first();
    }
}

