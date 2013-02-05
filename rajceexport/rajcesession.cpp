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

#include "rajcesession.moc"

// Qt includes

#include <QWidget>
#include <QCryptographicHash>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QFileInfo>

// KDE includes

#include <kurl.h>
#include <kdebug.h>
#include <krandom.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>

// Libkdcraw includes

#include <libkdcraw/kdcraw.h>

// Local includes

#include "mpform.h"
#include "kpversion.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIRajceExportPlugin
{

const KUrl     RAJCE_URL("http://www.rajce.idnes.cz/liveAPI/index.php");
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
    QString baseName    = saveDir + QFileInfo(imagePath).baseName().trimmed();
    ret.scaledImagePath = baseName + ".jpg";
    ret.thumbPath       = baseName + ".thumb.jpg";

    if (maxDimension > 0 && ((unsigned) image.width() > maxDimension || (unsigned) image.height() > maxDimension))
    {
        kDebug() << "Resizing to " << maxDimension;
        image = image.scaled(maxDimension, maxDimension, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    kDebug() << "Saving to temp file: " << ret.scaledImagePath;
    image.save(ret.scaledImagePath, "JPEG", jpgQuality);

    QImage thumb = image.scaled(thumbDimension, thumbDimension, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    kDebug() << "Saving thumb to temp file: " << ret.thumbPath;
    thumb.save(ret.thumbPath, "JPEG", jpgQuality);

    // copy meta data to temporary image
    KPMetadata meta;

    if (meta.load(imagePath))
    {
        meta.setImageDimensions(image.size());
        meta.setImageProgramId("Kipi-plugins", kipiplugins_version);
        meta.save(ret.scaledImagePath);
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

    virtual QByteArray encode()   const;
    virtual QString contentType() const;

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
    virtual QString contentType() const;

protected:

    virtual void cleanUpOnError(KIPIRajceExportPlugin::SessionState& state);
    virtual void parseResponse(QXmlQuery& query, KIPIRajceExportPlugin::SessionState& state);
    virtual QString additionalXml() const;

private:

    int      m_jpgQuality;

    unsigned m_desiredDimension;
    unsigned m_maxDimension;

    QString  m_tmpDir;
    QString  m_imagePath;

    QImage   m_image;

    MPForm*  m_form;
};

/// Commands impls

RajceCommand::RajceCommand(const QString& name, RajceCommandType type)
    : m_name(name), m_commandType(type)
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
    QString ret("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");

    ret.append("<request>\n");
    ret.append("  <command>").append(m_name).append("</command>\n");
    ret.append("  <parameters>\n");

    foreach(QString key, m_parameters.keys())
    {
        ret.append("    <").append(key).append(">");
        ret.append(m_parameters[key]);
        ret.append("</").append(key).append(">\n");
    }

    ret.append("</parameters>\n");
    ret.append(additionalXml());
    ret.append("\n</request>\n");

    return ret;
}

bool RajceCommand::_parseError(QXmlQuery& query, SessionState& state)
{
    QString results;

    query.setQuery("/response/string(errorCode)");
    query.evaluateTo(&results);

    if (results.trimmed().length() > 0)
    {
        state.lastErrorCode() = results.toUInt();
        query.setQuery("/response/string(result)");
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
    QByteArray ret = QString("data=").toAscii();
    ret.append(QUrl::toPercentEncoding(getXml()));

    return ret;
}

QString RajceCommand::contentType() const
{
    return QString("application/x-www-form-urlencoded");
}

RajceCommandType RajceCommand::commandType() const
{
    return m_commandType;
}

// -----------------------------------------------------------------------

OpenAlbumCommand::OpenAlbumCommand(unsigned albumId, const SessionState& state)
    : RajceCommand("openAlbum", OpenAlbum)
{
    parameters()["token"]   = state.sessionToken();
    parameters()["albumID"] = QString::number(albumId);
}

void OpenAlbumCommand::parseResponse(QXmlQuery& q, SessionState& state)
{
    state.openAlbumToken() = QString();

    QString result;

    q.setQuery("/response/data(albumToken)");
    q.evaluateTo(&result);

    state.openAlbumToken() = result.trimmed();
}

void OpenAlbumCommand::cleanUpOnError(SessionState& state)
{
    state.openAlbumToken() = QString();
}

// -----------------------------------------------------------------------

LoginCommand::LoginCommand(const QString& username, const QString& password)
    : RajceCommand("login", Login)
{
    parameters()["login"]    = username;
    parameters()["password"] = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();
}

void LoginCommand::parseResponse(QXmlQuery& q, SessionState& state)
{
    QString results;

    q.setQuery("/response/string(maxWidth)");
    q.evaluateTo(&results);
    state.maxWidth() = results.toUInt();

    q.setQuery("/response/string(maxHeight)");
    q.evaluateTo(&results);
    state.maxHeight() = results.toUInt();

    q.setQuery("/response/string(quality)");
    q.evaluateTo(&results);
    state.imageQuality() = results.toUInt();

    q.setQuery("/response/string(nick)");
    q.evaluateTo(&results);
    state.nickname() = results.trimmed();

    q.setQuery("data(/response/sessionToken)");
    q.evaluateTo(&results);
    state.sessionToken() = results.trimmed();

    state.username() = parameters()["login"];
}

void LoginCommand::cleanUpOnError(SessionState& state)
{
    state.openAlbumToken() = "";
    state.nickname()       = "";
    state.username()       = "";
    state.imageQuality()   = 0;
    state.maxHeight()      = 0;
    state.maxWidth()       = 0;
    state.sessionToken()   = "";
    state.albums().clear();
}

// -----------------------------------------------------------------------

CreateAlbumCommand::CreateAlbumCommand(const QString& name, const QString& description,
                                       bool visible, const SessionState& state)
    : RajceCommand("createAlbum", CreateAlbum)
{
    parameters()["token"]            = state.sessionToken();
    parameters()["albumName"]        = name;
    parameters()["albumDescription"] = description;
    parameters()["albumVisible"]     = visible ? "1" : "0";
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
    : RajceCommand("closeAlbum", CloseAlbum)
{
    parameters()["token"]      = state.sessionToken();
    parameters()["albumToken"] = state.openAlbumToken();
}

// -----------------------------------------------------------------------

AlbumListCommand::AlbumListCommand(const SessionState& state)
    : RajceCommand("getAlbumList", ListAlbums)
{
    parameters()["token"] = state.sessionToken();
}

void AlbumListCommand::parseResponse(QXmlQuery& q, SessionState& state)
{
    state.albums().clear();

    QXmlResultItems results;

    q.setQuery("/response/albums/album");
    q.evaluateTo(&results);

    QXmlItem item(results.next());

    while(!item.isNull())
    {
        q.setFocus(item);
        Album album;
        QString detail;

        q.setQuery("data(./@id)");
        q.evaluateTo(&detail);
        album.id = detail.toUInt();

        q.setQuery("data(./albumName)");
        q.evaluateTo(&detail);
        album.name = detail.trimmed();

        q.setQuery("data(./description)");
        q.evaluateTo(&detail);
        album.description = detail.trimmed();

        q.setQuery("data(./url)");
        q.evaluateTo(&detail);
        album.url = detail.trimmed();

        q.setQuery("data(./thumbUrl)");
        q.evaluateTo(&detail);
        album.thumbUrl = detail.trimmed();

        q.setQuery("data(./createDate)");
        q.evaluateTo(&detail);
        album.createDate = QDateTime::fromString(detail.trimmed(), "yyyy-MM-dd hh:mm:ss");

        kDebug() << "Create date: " << detail.trimmed() << " = " << QDateTime::fromString(detail.trimmed(), "yyyy-MM-dd hh:mm:ss");

        q.setQuery("data(./updateDate)");
        q.evaluateTo(&detail);
        album.updateDate = QDateTime::fromString(detail.trimmed(), "yyyy-MM-dd hh:mm:ss");

        q.evaluateTo(&detail);
        album.isHidden = detail.toUInt() != 0;

        q.setQuery("data(./secure)");
        q.evaluateTo(&detail);
        album.isSecure = detail.toUInt() != 0;

        q.setQuery("data(./startDateInterval)");
        q.evaluateTo(&detail);

        if (detail.trimmed().length() > 0)
        {
            album.validFrom = QDateTime::fromString(detail, "yyyy-MM-dd hh:mm:ss");
        }

        q.setQuery("data(./endDateInterval)");
        q.evaluateTo(&detail);

        if (detail.trimmed().length() > 0)
        {
            album.validTo = QDateTime::fromString(detail, "yyyy-MM-dd hh:mm:ss");
        }

        q.setQuery("data(./thumbUrlBest)");
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
    : RajceCommand("addPhoto", AddPhoto),
      m_jpgQuality(jpgQuality),
      m_desiredDimension(dimension),
      m_maxDimension(0),
      m_tmpDir(tmpDir),
      m_imagePath(path),
      m_form(0)
{
    bool isRaw = KPMetadata::isRawFile(path);

    if (isRaw)
    {
        kDebug() << "Get RAW preview " << path;
        KDcrawIface::KDcraw::loadRawPreview(m_image, path);
    }
    else
    {
        m_image.load(path);
    }

    if (m_image.isNull())
    {
        kDebug() << "Could not read in an image from " << path << ". Adding the photo will not work.";
        return;
    }

    m_maxDimension             = state.maxHeight() > state.maxWidth() ? state.maxWidth() : state.maxHeight();
    parameters()["token"]      = state.sessionToken();
    parameters()["albumToken"] = state.openAlbumToken();
    m_form                     = new MPForm;
}

AddPhotoCommand::~AddPhotoCommand()
{
    delete m_form;
}

void AddPhotoCommand::cleanUpOnError(KIPIRajceExportPlugin::SessionState&)
{
}

void AddPhotoCommand::parseResponse(QXmlQuery&, KIPIRajceExportPlugin::SessionState&)
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

    metadata["FullFilePath"]          = m_imagePath;
    metadata["OriginalFileName"]      = f.fileName();
    metadata["OriginalFileExtension"] = QString(".") + f.suffix();
    metadata["PerceivedType"]         = "image"; //what are the other values here? video?
    metadata["OriginalWidth"]         = QString::number(m_image.width());
    metadata["OriginalHeight"]        = QString::number(m_image.height());
    metadata["LengthMS"]              = '0';
    metadata["FileSize"]              = QString::number(f.size());

    //TODO extract these from exif
    //    KPMetadata meta;
    //    if (meta.load(m_imagePath)) {
    metadata["Title"]           = "";
    metadata["KeywordSet"]      = "";
    metadata["PeopleRegionSet"] = "";
    //    }

    QString id = QString::number(KRandom::random());
    QString ret("  <objectInfo>\n    <Item id=\"");
    ret.append(id).append("\">\n");

    foreach(QString key, metadata.keys())
    {
        ret.append("      <").append(key);
        QString value = metadata[key];

        if (value.length() == 0)
        {
            ret.append(" />\n");
        }
        else
        {
            ret.append(">");
            ret.append(value);
            ret.append("</");
            ret.append(key);
            ret.append(">\n");
        }
    }

    ret.append("    </Item>\n  </objectInfo>\n");

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
        kDebug() << m_imagePath << " could not be read, no data will be sent.";
        return QByteArray();
    }

    PreparedImage prepared = _prepareImageForUpload(m_tmpDir, m_image, m_imagePath, m_desiredDimension, THUMB_SIZE, m_jpgQuality);

    //add the rest of the parameters to be encoded as xml
    QImage scaled(prepared.scaledImagePath);
    parameters()["width"]  = QString::number(scaled.width());
    parameters()["height"] = QString::number(scaled.height());

    QString xml = getXml();

    kDebug() << "Really sending:\n" << xml;

    //now we can create the form with all the info
    m_form->reset();

    m_form->addPair("data", xml);

    m_form->addFile("thumb", prepared.thumbPath);
    m_form->addFile("photo", prepared.scaledImagePath);

    QFile::remove(prepared.thumbPath);
    QFile::remove(prepared.scaledImagePath);

    m_form->finish();

    QByteArray ret = m_form->formData();

    return ret;
}

/// RajceSession impl

RajceSession::RajceSession(QWidget* const parent, const QString& tmpDir)
    : QObject(parent), m_queueAccess(QMutex::Recursive), m_tmpDir(tmpDir), m_currentJob(0)
{
}

const SessionState& RajceSession::state() const
{
    return m_state;
}

void RajceSession::_startJob(RajceCommand* command)
{
    kDebug() << "Sending command:\n" << command->getXml();

    QByteArray data             = command->encode();
    KIO::TransferJob* const job = KIO::http_post(RAJCE_URL, data, KIO::HideProgressInfo);
    job->ui()->setWindow(static_cast<QWidget*>(parent()));
    job->addMetaData("content-type", command->contentType());

    connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
            this, SLOT(data(KIO::Job*,QByteArray)));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(finished(KJob*)));

    connect(job, SIGNAL(percent(KJob*,ulong)),
            this, SLOT(slotPercent(KJob*,ulong)));

    m_currentJob = job;
    m_buffer.resize(0);

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

void RajceSession::data(KIO::Job*, const QByteArray& data)
{
    if (data.isEmpty())
        return;

    int oldSize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data() + oldSize, data.data(), data.size());
}

void RajceSession::finished(KJob*)
{
    QString response = QString::fromUtf8(m_buffer.data());

    kDebug() << response;

    m_queueAccess.lock();

    RajceCommand* const c = m_commandQueue.head();
    m_currentJob          = 0;

    c->processResponse(response, m_state);

    RajceCommandType type = c->commandType();

    delete c;

    kDebug() << "State after command: " << m_state;

    //let the users react on the command before we
    //let the next queued command in.
    //This enables the connected slots to read in
    //reliable values from the state and/or
    //clear the error state once it's handled.
    emit busyFinished(type);

    //only deque the command after the above signal has been
    //emitted so that the users can queue other commands
    //without them being started straight away in the enqueue
    //method which would happen if the command was dequed
    //before the signal and the signal was emitted in the same
    //thread (which is the case (always?)).
    m_commandQueue.dequeue();

    //see if there's something to continue with
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

void RajceSession::openAlbum(const KIPIRajceExportPlugin::Album& album)
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
    m_state.lastErrorMessage() = "";
}

void RajceSession::slotPercent(KJob* job, ulong percent)
{
    kDebug() << "Percent signalled: " << percent;

    if (job == m_currentJob)
    {
        kDebug() << "Re-emitting percent";
        emit busyProgress(m_commandQueue.head()->commandType(), percent);
    }
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
    if (m_currentJob != 0)
    {
        KJob* const job = m_currentJob;
        finished(job);
        job->kill();
    }
}

void RajceSession::init(const KIPIRajceExportPlugin::SessionState& initialState)
{
    m_state = initialState;
}

} // namespace KIPIRajceExportPlugin
