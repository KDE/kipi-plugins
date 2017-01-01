/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#include "rajcesession.h"

// Qt includes

#include <QWidget>
#include <QCryptographicHash>
#include <QXmlResultItems>
#include <QXmlQuery>
#include <QFileInfo>
#include <QUrl>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"
#include "mpform.h"
#include "kpversion.h"
#include "kputil.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIRajcePlugin
{

const QUrl     RAJCE_URL(QString::fromLatin1("http://www.rajce.idnes.cz/liveAPI/index.php"));
const unsigned THUMB_SIZE = 100;

struct PreparedImage
{
    QString scaledImagePath;
    QString thumbPath;
};

PreparedImage _prepareImageForUpload(const QString& saveDir, const QImage& img, const QString& imagePath,
                                     unsigned maxDimension, unsigned thumbDimension, int jpgQuality)
{
    PreparedImage ret;

    if (img.isNull())
        return ret;

    QImage image(img);

    // get temporary file name
    QString baseName    = saveDir  + QFileInfo(imagePath).baseName().trimmed();
    ret.scaledImagePath = baseName + QString::fromLatin1(".jpg");
    ret.thumbPath       = baseName + QString::fromLatin1(".thumb.jpg");

    if (maxDimension > 0 && ((unsigned) image.width() > maxDimension || (unsigned) image.height() > maxDimension))
    {
        qCDebug(KIPIPLUGINS_LOG) << "Resizing to " << maxDimension;
        image = image.scaled(maxDimension, maxDimension, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    qCDebug(KIPIPLUGINS_LOG) << "Saving to temp file: " << ret.scaledImagePath;
    image.save(ret.scaledImagePath, "JPEG", jpgQuality);

    QImage thumb = image.scaled(thumbDimension, thumbDimension, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    qCDebug(KIPIPLUGINS_LOG) << "Saving thumb to temp file: " << ret.thumbPath;
    thumb.save(ret.thumbPath, "JPEG", jpgQuality);

    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        Interface* const iface = pl->interface();

        if (iface)
        {
            // copy meta data to temporary image
            QPointer<MetadataProcessor> meta = iface->createMetadataProcessor();

            if (meta->load(QUrl::fromLocalFile(imagePath)))
            {
                meta->setImageDimensions(image.size());
                meta->setImageOrientation(MetadataProcessor::NORMAL);
                meta->setImageProgramId(QString::fromLatin1("Kipi-plugins"), kipipluginsVersion());
                meta->save(QUrl::fromLocalFile(ret.scaledImagePath), true);
            }
        }
    }

    return ret;
}

/// Commands definitions

class RajceCommand
{
public:

    explicit RajceCommand(const QString& name, RajceCommandType commandType);
    virtual ~RajceCommand();

    QString getXml() const;

    void processResponse(const QString& response, SessionState& state);

    RajceCommandType commandType() const;
    virtual QByteArray encode()    const;
    virtual QString contentType()  const;

protected:

    virtual void parseResponse(QXmlQuery& query, SessionState& state) = 0;
    virtual void cleanUpOnError(SessionState& state) = 0;

    QMap<QString, QString>& parameters() const; // allow modification in const methods for lazy init to be possible

    // additional xml after the "parameters"
    virtual QString additionalXml() const;

private:

    bool _parseError(QXmlQuery& query, SessionState& state);

private:

    QString                m_name;
    RajceCommandType       m_commandType;
    QMap<QString, QString> m_parameters;
};

// -----------------------------------------------------------------------

class LoginCommand : public RajceCommand
{
public:

    LoginCommand(const QString& username, const QString& password);

protected:

    virtual void parseResponse(QXmlQuery& response, SessionState& state);
    virtual void cleanUpOnError(SessionState& state);
};

// -----------------------------------------------------------------------

class OpenAlbumCommand : public RajceCommand
{
public:

    explicit OpenAlbumCommand(unsigned albumId, const SessionState& state);

protected:

    virtual void parseResponse(QXmlQuery& response, SessionState& state);
    virtual void cleanUpOnError(SessionState& state);
};

// -----------------------------------------------------------------------

class CreateAlbumCommand : public RajceCommand
{
public:

    CreateAlbumCommand(const QString& name, const QString& description, bool visible, const SessionState& state);

protected:

    virtual void parseResponse(QXmlQuery& response, SessionState& state);
    virtual void cleanUpOnError(SessionState& state);
};

// -----------------------------------------------------------------------

class CloseAlbumCommand : public RajceCommand
{
public:

    CloseAlbumCommand(const SessionState& state);

protected:

    virtual void parseResponse(QXmlQuery& response, SessionState& state);
    virtual void cleanUpOnError(SessionState& state);
};

// -----------------------------------------------------------------------

class AlbumListCommand : public RajceCommand
{
public:

    AlbumListCommand(const SessionState&);

protected:

    virtual void parseResponse(QXmlQuery& response, SessionState& state);
    virtual void cleanUpOnError(SessionState& state);
};

// -----------------------------------------------------------------------

class AddPhotoCommand : public RajceCommand
{
public:

    AddPhotoCommand(const QString& tmpDir, const QString& path, unsigned dimension, int jpgQuality, const SessionState& state);
    virtual ~AddPhotoCommand();

    virtual QByteArray encode() const;
    virtual QString    contentType() const;

protected:

    virtual void    cleanUpOnError(KIPIRajcePlugin::SessionState& state);
    virtual void    parseResponse(QXmlQuery& query, KIPIRajcePlugin::SessionState& state);
    virtual QString additionalXml() const;

private:

    int        m_jpgQuality;

    unsigned   m_desiredDimension;
    unsigned   m_maxDimension;

    QString    m_tmpDir;
    QString    m_imagePath;

    QImage     m_image;

    MPForm*    m_form;
};

/// Commands impls

RajceCommand::RajceCommand(const QString& name, RajceCommandType type)
    : m_name(name),
      m_commandType(type)
{
}

RajceCommand::~RajceCommand()
{
}

QMap<QString, QString>& RajceCommand::parameters() const
{
    return const_cast<QMap<QString, QString> &>(m_parameters);
}

QString RajceCommand::getXml() const
{
    QString ret(QString::fromLatin1("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"));

    ret.append(QString::fromLatin1("<request>\n"));
    ret.append(QString::fromLatin1("  <command>")).append(m_name).append(QString::fromLatin1("</command>\n"));
    ret.append(QString::fromLatin1("  <parameters>\n"));

    foreach(QString key, m_parameters.keys())
    {
        ret.append(QString::fromLatin1("    <")).append(key).append(QString::fromLatin1(">"));
        ret.append(m_parameters[key]);
        ret.append(QString::fromLatin1("</")).append(key).append(QString::fromLatin1(">\n"));
    }

    ret.append(QString::fromLatin1("</parameters>\n"));
    ret.append(additionalXml());
    ret.append(QString::fromLatin1("\n</request>\n"));

    return ret;
}

bool RajceCommand::_parseError(QXmlQuery& query, SessionState& state)
{
    QString results;

    query.setQuery(QString::fromLatin1("/response/string(errorCode)"));
    query.evaluateTo(&results);

    if (results.trimmed().length() > 0)
    {
        state.lastErrorCode()    = results.toUInt();
        query.setQuery(QString::fromLatin1("/response/string(result)"));
        query.evaluateTo(&results);
        state.lastErrorMessage() = results.trimmed();

        return true;
    }

    return false;
}

void RajceCommand::processResponse(const QString& response, SessionState& state)
{
    QXmlQuery q;
    q.setFocus(response);

    state.lastCommand() = m_commandType;

    if (_parseError(q, state))
    {
        cleanUpOnError(state);
    }
    else
    {
        parseResponse(q, state);
    }
}

QString RajceCommand::additionalXml() const
{
    return QString();
}

QByteArray RajceCommand::encode() const
{
    QByteArray ret = QString::fromLatin1("data=").toLatin1();
    ret.append(QUrl::toPercentEncoding(getXml()));

    return ret;
}

QString RajceCommand::contentType() const
{
    return QString::fromLatin1("application/x-www-form-urlencoded");
}

RajceCommandType RajceCommand::commandType() const
{
    return m_commandType;
}

// -----------------------------------------------------------------------

OpenAlbumCommand::OpenAlbumCommand(unsigned albumId, const SessionState& state)
    : RajceCommand(QString::fromLatin1("openAlbum"), OpenAlbum)
{
    parameters()[QString::fromLatin1("token")]   = state.sessionToken();
    parameters()[QString::fromLatin1("albumID")] = QString::number(albumId);
}

void OpenAlbumCommand::parseResponse(QXmlQuery& q, SessionState& state)
{
    state.openAlbumToken() = QString();

    QString result;

    q.setQuery(QString::fromLatin1("/response/data(albumToken)"));
    q.evaluateTo(&result);

    state.openAlbumToken() = result.trimmed();
}

void OpenAlbumCommand::cleanUpOnError(SessionState& state)
{
    state.openAlbumToken() = QString();
}

// -----------------------------------------------------------------------

LoginCommand::LoginCommand(const QString& username, const QString& password)
    : RajceCommand(QString::fromLatin1("login"), Login)
{
    parameters()[QString::fromLatin1("login")]    = username;
    parameters()[QString::fromLatin1("password")] = QString::fromLatin1(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());
}

void LoginCommand::parseResponse(QXmlQuery& q, SessionState& state)
{
    QString results;

    q.setQuery(QString::fromLatin1("/response/string(maxWidth)"));
    q.evaluateTo(&results);
    state.maxWidth()     = results.toUInt();

    q.setQuery(QString::fromLatin1("/response/string(maxHeight)"));
    q.evaluateTo(&results);
    state.maxHeight()    = results.toUInt();

    q.setQuery(QString::fromLatin1("/response/string(quality)"));
    q.evaluateTo(&results);
    state.imageQuality() = results.toUInt();

    q.setQuery(QString::fromLatin1("/response/string(nick)"));
    q.evaluateTo(&results);
    state.nickname()     = results.trimmed();

    q.setQuery(QString::fromLatin1("data(/response/sessionToken)"));
    q.evaluateTo(&results);
    state.sessionToken() = results.trimmed();

    state.username()     = parameters()[QString::fromLatin1("login")];
}

void LoginCommand::cleanUpOnError(SessionState& state)
{
    state.openAlbumToken() = QString::fromLatin1("");
    state.nickname()       = QString::fromLatin1("");
    state.username()       = QString::fromLatin1("");
    state.imageQuality()   = 0;
    state.maxHeight()      = 0;
    state.maxWidth()       = 0;
    state.sessionToken()   = QString::fromLatin1("");
    state.albums().clear();
}

// -----------------------------------------------------------------------

CreateAlbumCommand::CreateAlbumCommand(const QString& name, const QString& description,
                                       bool visible, const SessionState& state)
    : RajceCommand(QString::fromLatin1("createAlbum"), CreateAlbum)
{
    parameters()[QString::fromLatin1("token")]            = state.sessionToken();
    parameters()[QString::fromLatin1("albumName")]        = name;
    parameters()[QString::fromLatin1("albumDescription")] = description;
    parameters()[QString::fromLatin1("albumVisible")]     = visible ? QString::fromLatin1("1") : QString::fromLatin1("0");
}

void CreateAlbumCommand::parseResponse(QXmlQuery&, SessionState&)
{
}

void CreateAlbumCommand::cleanUpOnError(SessionState&)
{
}

void CloseAlbumCommand::parseResponse(QXmlQuery&, SessionState&)
{
}

void CloseAlbumCommand::cleanUpOnError(SessionState&)
{
}

// -----------------------------------------------------------------------

CloseAlbumCommand::CloseAlbumCommand(const SessionState& state)
    : RajceCommand(QString::fromLatin1("closeAlbum"), CloseAlbum)
{
    parameters()[QString::fromLatin1("token")]      = state.sessionToken();
    parameters()[QString::fromLatin1("albumToken")] = state.openAlbumToken();
}

// -----------------------------------------------------------------------

AlbumListCommand::AlbumListCommand(const SessionState& state)
    : RajceCommand(QString::fromLatin1("getAlbumList"), ListAlbums)
{
    parameters()[QString::fromLatin1("token")] = state.sessionToken();
}

void AlbumListCommand::parseResponse(QXmlQuery& q, SessionState& state)
{
    state.albums().clear();

    QXmlResultItems results;

    q.setQuery(QString::fromLatin1("/response/albums/album"));
    q.evaluateTo(&results);

    QXmlItem item(results.next());

    while(!item.isNull())
    {
        q.setFocus(item);
        Album album;
        QString detail;

        q.setQuery(QString::fromLatin1("data(./@id)"));
        q.evaluateTo(&detail);
        album.id          = detail.toUInt();

        q.setQuery(QString::fromLatin1("data(./albumName)"));
        q.evaluateTo(&detail);
        album.name        = detail.trimmed();

        q.setQuery(QString::fromLatin1("data(./description)"));
        q.evaluateTo(&detail);
        album.description = detail.trimmed();

        q.setQuery(QString::fromLatin1("data(./url)"));
        q.evaluateTo(&detail);
        album.url         = detail.trimmed();

        q.setQuery(QString::fromLatin1("data(./thumbUrl)"));
        q.evaluateTo(&detail);
        album.thumbUrl    = detail.trimmed();

        q.setQuery(QString::fromLatin1("data(./createDate)"));
        q.evaluateTo(&detail);
        album.createDate  = QDateTime::fromString(detail.trimmed(), QString::fromLatin1("yyyy-MM-dd hh:mm:ss"));

        qCDebug(KIPIPLUGINS_LOG) << "Create date: " << detail.trimmed() << " = " << QDateTime::fromString(detail.trimmed(), QString::fromLatin1("yyyy-MM-dd hh:mm:ss"));

        q.setQuery(QString::fromLatin1("data(./updateDate)"));
        q.evaluateTo(&detail);
        album.updateDate  = QDateTime::fromString(detail.trimmed(), QString::fromLatin1("yyyy-MM-dd hh:mm:ss"));

        q.evaluateTo(&detail);
        album.isHidden    = detail.toUInt() != 0;

        q.setQuery(QString::fromLatin1("data(./secure)"));
        q.evaluateTo(&detail);
        album.isSecure    = detail.toUInt() != 0;

        q.setQuery(QString::fromLatin1("data(./startDateInterval)"));
        q.evaluateTo(&detail);

        if (detail.trimmed().length() > 0)
        {
            album.validFrom = QDateTime::fromString(detail, QString::fromLatin1("yyyy-MM-dd hh:mm:ss"));
        }

        q.setQuery(QString::fromLatin1("data(./endDateInterval)"));
        q.evaluateTo(&detail);

        if (detail.trimmed().length() > 0)
        {
            album.validTo = QDateTime::fromString(detail, QString::fromLatin1("yyyy-MM-dd hh:mm:ss"));
        }

        q.setQuery(QString::fromLatin1("data(./thumbUrlBest)"));
        q.evaluateTo(&detail);
        album.bestQualityThumbUrl = detail.trimmed();

        state.albums().append(album);
        item = results.next();
    }
}

void AlbumListCommand::cleanUpOnError(SessionState& state)
{
    state.albums().clear();
}

// -----------------------------------------------------------------------

AddPhotoCommand::AddPhotoCommand(const QString& tmpDir, const QString& path, unsigned dimension,
                                 int jpgQuality, const SessionState& state)
    : RajceCommand(QString::fromLatin1("addPhoto"), AddPhoto),
      m_jpgQuality(jpgQuality),
      m_desiredDimension(dimension),
      m_maxDimension(0),
      m_tmpDir(tmpDir),
      m_imagePath(path),
      m_form(0)
{
    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        Interface* const iface = pl->interface();

        if (iface)
        {
            m_image = iface->preview(QUrl::fromLocalFile(path));
        }
    }

    if (m_image.isNull())
    {
        m_image.load(path);
    }

    if (m_image.isNull())
    {
        qCDebug(KIPIPLUGINS_LOG) << "Could not read in an image from " << path << ". Adding the photo will not work.";
        return;
    }

    m_maxDimension                                  = (state.maxHeight() > state.maxWidth()) ? state.maxWidth()
                                                                                             : state.maxHeight();
    parameters()[QString::fromLatin1("token")]      = state.sessionToken();
    parameters()[QString::fromLatin1("albumToken")] = state.openAlbumToken();
    m_form                                          = new MPForm;
}

AddPhotoCommand::~AddPhotoCommand()
{
    delete m_form;
}

void AddPhotoCommand::cleanUpOnError(KIPIRajcePlugin::SessionState&)
{
}

void AddPhotoCommand::parseResponse(QXmlQuery&, KIPIRajcePlugin::SessionState&)
{
}

QString AddPhotoCommand::additionalXml() const
{
    if (m_image.isNull())
    {
        return QString();
    }

    QMap<QString, QString> metadata;
    QFileInfo f(m_imagePath);

    metadata[QString::fromLatin1("FullFilePath")]          = m_imagePath;
    metadata[QString::fromLatin1("OriginalFileName")]      = f.fileName();
    metadata[QString::fromLatin1("OriginalFileExtension")] = QString::fromLatin1(".") + f.suffix();
    metadata[QString::fromLatin1("PerceivedType")]         = QString::fromLatin1("image"); //what are the other values here? video?
    metadata[QString::fromLatin1("OriginalWidth")]         = QString::number(m_image.width());
    metadata[QString::fromLatin1("OriginalHeight")]        = QString::number(m_image.height());
    metadata[QString::fromLatin1("LengthMS")]              = QLatin1Char('0');
    metadata[QString::fromLatin1("FileSize")]              = QString::number(f.size());

    //TODO extract these from exif
    metadata[QString::fromLatin1("Title")]                 = QString::fromLatin1("");
    metadata[QString::fromLatin1("KeywordSet")]            = QString::fromLatin1("");
    metadata[QString::fromLatin1("PeopleRegionSet")]       = QString::fromLatin1("");

    qsrand((uint)QTime::currentTime().msec());
    QString id = QString::number(qrand());
    QString ret(QString::fromLatin1("  <objectInfo>\n    <Item id=\""));
    ret.append(id).append(QString::fromLatin1("\">\n"));

    foreach(QString key, metadata.keys())
    {
        ret.append(QString::fromLatin1("      <")).append(key);
        QString value = metadata[key];

        if (value.length() == 0)
        {
            ret.append(QString::fromLatin1(" />\n"));
        }
        else
        {
            ret.append(QString::fromLatin1(">"));
            ret.append(value);
            ret.append(QString::fromLatin1("</"));
            ret.append(key);
            ret.append(QString::fromLatin1(">\n"));
        }
    }

    ret.append(QString::fromLatin1("    </Item>\n  </objectInfo>\n"));

    return ret;
}

QString AddPhotoCommand::contentType() const
{
    return m_form->contentType();
}

QByteArray AddPhotoCommand::encode() const
{
    if (m_image.isNull())
    {
        qCDebug(KIPIPLUGINS_LOG) << m_imagePath << " could not be read, no data will be sent.";
        return QByteArray();
    }

    PreparedImage prepared                      = _prepareImageForUpload(m_tmpDir,
                                                                         m_image,
                                                                         m_imagePath,
                                                                         m_desiredDimension,
                                                                         THUMB_SIZE,
                                                                         m_jpgQuality);

    //add the rest of the parameters to be encoded as xml
    QImage scaled(prepared.scaledImagePath);
    parameters()[QString::fromLatin1("width")]  = QString::number(scaled.width());
    parameters()[QString::fromLatin1("height")] = QString::number(scaled.height());
    QString xml                                 = getXml();

    qCDebug(KIPIPLUGINS_LOG) << "Really sending:\n" << xml;

    //now we can create the form with all the info
    m_form->reset();

    m_form->addPair(QString::fromLatin1("data"), xml);

    m_form->addFile(QString::fromLatin1("thumb"), prepared.thumbPath);
    m_form->addFile(QString::fromLatin1("photo"), prepared.scaledImagePath);

    QFile::remove(prepared.thumbPath);
    QFile::remove(prepared.scaledImagePath);

    m_form->finish();

    QByteArray ret = m_form->formData();

    return ret;
}

/// RajceSession impl

RajceSession::RajceSession(QWidget* const parent, const QString& tmpDir)
    : QObject(parent),
      m_queueAccess(QMutex::Recursive),
      m_tmpDir(tmpDir),
      m_netMngr(0),
      m_reply(0)
{
    m_netMngr = new QNetworkAccessManager(this);

    connect(m_netMngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotFinished(QNetworkReply*)));
}

const SessionState& RajceSession::state() const
{
    return m_state;
}

void RajceSession::_startJob(RajceCommand* command)
{
    qCDebug(KIPIPLUGINS_LOG) << "Sending command:\n" << command->getXml();

    QByteArray data = command->encode();

    QNetworkRequest netRequest(RAJCE_URL);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, command->contentType());

    m_reply = m_netMngr->post(netRequest, data);

    connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)),
            SLOT(slotUploadProgress(qint64,qint64)));

    emit busyStarted(command->commandType());
}

void RajceSession::login(const QString& username, const QString& password)
{
    LoginCommand* const command = new LoginCommand(username, password);
    _enqueue(command);
}

void RajceSession::loadAlbums()
{
    AlbumListCommand* const command = new AlbumListCommand(m_state);
    _enqueue(command);
}

void RajceSession::createAlbum(const QString& name, const QString& description, bool visible)
{
    CreateAlbumCommand* const command = new CreateAlbumCommand(name, description, visible, m_state);
    _enqueue(command);
}

void RajceSession::slotFinished(QNetworkReply* reply)
{
    if (reply != m_reply)
    {
        return;
    }

    QString response      = QString::fromUtf8(reply->readAll());

    qCDebug(KIPIPLUGINS_LOG) << response;

    m_queueAccess.lock();

    RajceCommand* const c = m_commandQueue.head();
    m_reply               = 0;

    c->processResponse(response, m_state);

    RajceCommandType type = c->commandType();

    delete c;

    qCDebug(KIPIPLUGINS_LOG) << "State after command: " << m_state;

    // Let the users react on the command before we
    // let the next queued command in.
    // This enables the connected slots to read in
    // reliable values from the state and/or
    // clear the error state once it's handled.
    emit busyFinished(type);

    reply->deleteLater();

    // Only dequeue the command after the above signal has been
    // emitted so that the users can queue other commands
    // without them being started straight away in the enqueue
    // method which would happen if the command was dequed
    // before the signal and the signal was emitted in the same
    // thread (which is the case (always?)).
    m_commandQueue.dequeue();

    // see if there's something to continue with
    if (m_commandQueue.size() > 0)
    {
        _startJob(m_commandQueue.head());
    }

    m_queueAccess.unlock();
}

void RajceSession::logout()
{
    //TODO
}

void RajceSession::openAlbum(const KIPIRajcePlugin::Album& album)
{
    OpenAlbumCommand* const command = new OpenAlbumCommand(album.id, m_state);
    _enqueue(command);
}

void RajceSession::closeAlbum()
{
    if (!m_state.openAlbumToken().isEmpty())
    {
        CloseAlbumCommand* const command = new CloseAlbumCommand(m_state);
        _enqueue(command);
    }
    else
    {
        emit busyFinished(CloseAlbum);
    }
}

void RajceSession::uploadPhoto(const QString& path, unsigned dimension, int jpgQuality)
{
    AddPhotoCommand* const command = new AddPhotoCommand(m_tmpDir, path, dimension, jpgQuality, m_state);
    _enqueue(command);
}

void RajceSession::clearLastError()
{
    m_state.lastErrorCode()    = 0;
    m_state.lastErrorMessage() = QString::fromLatin1("");
}

void RajceSession::slotUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if (bytesTotal <= 0)
    {
        return;
    }

    unsigned percent = (unsigned)((float)bytesSent / bytesTotal * 100);

    qCDebug(KIPIPLUGINS_LOG) << "Percent signalled: " << percent;

    emit busyProgress(m_commandQueue.head()->commandType(), percent);
}

void RajceSession::_enqueue(RajceCommand* command)
{
    if (m_state.lastErrorCode() != 0)
    {
        return;
    }

    m_queueAccess.lock();
    m_commandQueue.enqueue(command);

    if (m_commandQueue.size() == 1)
    {
        _startJob(command);
    }

    m_queueAccess.unlock();
}

void RajceSession::cancelCurrentCommand()
{
    if (m_reply != 0)
    {
        slotFinished(m_reply);
        m_reply->abort();
        m_reply = 0;
    }
}

void RajceSession::init(const KIPIRajcePlugin::SessionState& initialState)
{
    m_state = initialState;
}

} // namespace KIPIRajcePlugin
