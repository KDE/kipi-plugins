#ifndef CAMERATYPE_H
#define CAMERATYPE_H

#include <qstring.h>

class CameraType {
public:
    CameraType();
    CameraType(const QString& title, const QString& model, const QString& port, const QString& path);
    ~CameraType();
    CameraType(const CameraType& ctype);
    CameraType& operator=(const CameraType& type);
    void setTitle(const QString& title);
    void setModel(const QString& model);
    void setPort(const QString& port);
    void setPath(const QString& path);
    void setValid(bool valid);
    QString title() const;
    QString model() const;
    QString port()  const;
    QString path()  const;
    bool valid();

private:
    QString title_;
    QString model_;
    QString port_;
    QString path_;
    bool    valid_;
};

#endif 

