// Local
#include "cameratype.h"

CameraType::CameraType() {
    valid_  = false;    
}

CameraType::CameraType(const QString& title, const QString& model, const QString& port, const QString& path) {
    title_  = title;
    model_  = model;
    port_   = port;
    path_   = path;
    valid_  = true;    
}

CameraType::~CameraType() {
}

CameraType::CameraType(const CameraType& ctype) {
    title_ = ctype.title_;
    model_ = ctype.model_;
    port_  = ctype.port_;
    path_  = ctype.path_;
    valid_ = ctype.valid_;
}

CameraType& CameraType::operator=(const CameraType& ctype) {
    if (this != &ctype) {
        title_ = ctype.title_;
        model_ = ctype.model_;
        port_  = ctype.port_;
        path_  = ctype.path_;
        valid_ = ctype.valid_;
    }
    return *this;
}

void CameraType::setTitle(const QString& title) {
    title_ = title;
}

void CameraType::setModel(const QString& model) {
    model_ = model;
}

void CameraType::setPort(const QString& port) {
    port_  = port;
}

void CameraType::setPath(const QString& path) {
    path_  = path;
}

QString CameraType::title() const {
    return title_;
}

QString CameraType::model() const {
    return model_;
}

QString CameraType::port() const {
    return port_;
}

QString CameraType::path() const {
    return path_;
}

bool CameraType::valid() {
    return valid_;
}

void CameraType::setValid(bool valid) {
    valid_ = valid;    
}

