#ifndef CAMERALIST_H
#define CAMERALIST_H

#include <qptrlist.h>
#include <qobject.h>

class QString;
class CameraType;
class CameraListPrivate;

class CameraList : public QObject {
	Q_OBJECT

public:
	CameraList(QObject *parent, const QString& file);
	~CameraList();
	bool load();
	bool close();
	void insert(CameraType* ctype);
	void remove(CameraType* ctype);
	CameraType* find(const QString& title);
	void clear();
	QPtrList<CameraType>* cameraList();
	static CameraList* instance();

private:
	static CameraList* instance_;
	CameraListPrivate *d;
	void insertPrivate(CameraType* ctype);
	void removePrivate(CameraType* ctype);

signals:
	void signalCameraListChanged();
};

#endif 

