/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#ifndef KIPIRAJCEEXPORTPLUGIN_SESSIONSTATE_H
#define KIPIRAJCEEXPORTPLUGIN_SESSIONSTATE_H

#include <QString>
#include <QVector>
#include "album.h"
#include <kdebug.h>

namespace KIPIRajceExportPlugin {

enum RajceCommandType {
    Login = 0,
    Logout,
    ListAlbums,
    CreateAlbum,
    OpenAlbum,
    CloseAlbum,
    AddPhoto
};

enum RajceErrorCode {
/*
    1 Neznámá chyba.
    2 Neplatný příkaz.
    3 Chybné přihlašovací údaje.
    4 Chybný přihlašovací token.
    5 Neznámý nebo opakující se sloupec {colName}.
    6 Není uvedeno správné albumID.
    7 Album neexistuje nebo není přihlášeného uživatele.
    8 Chybný token alba.
    9 Album nemůže mít prázdné jméno.
    10 Nepodařilo se vytvořit album. (těžko říct, proč ... asi chyba na straně serveru)
    11 Album neexistuje.
    12 Neexistující aplikace.
    13 Chybný klíč aplikace.
    14 Není přiložený soubor.
    15 Již existuje novější verze {version}.
    16 Chyba při ukládání souboru.
    17 Nepovolená přípona souboru {extension}.
    18 Chybné číslo verze klienta.
    19 Neexistující cíl (target).
*/
  UnknownError = 1,
  InvalidCommand,
  InvalidCredentials,
  InvalidSessionToken,
  InvalidOrRepeatedColumnName,
  InvalidAlbumId,
  AlbumDoesntExistOrNoPrivileges,
  InvalidAlbumToken,
  FailedToCreateAlbum,
  AlbumDoesntExist,
  UnknownApplication,
  InvalidApplicationKey,
  FileNotAttached,
  NewerVersionExists,
  SavingFileFailed,
  UnsupportedFileExtension,
  UnknownClientVersion,
  NonexistentTarget
};

class SessionState
{
public:

    SessionState() : _maxWidth(0), _maxHeight(0), _imageQuality(0), _lastErrorCode(0)
    {}

    inline QString & sessionToken() {
        return _sessionToken;
    }

    inline QString const & sessionToken() const {
        return _sessionToken;
    }

    inline QString & nickname() {
        return _nickname;
    }

    inline QString const & nickname() const {
        return _nickname;
    }

    inline QString & username() {
        return _username;
    }

    inline QString const & username() const {
        return _username;
    }

    inline QString & openAlbumToken() {
        return _albumToken;
    }

    inline QString const & openAlbumToken() const {
        return _albumToken;
    }

    inline QString & lastErrorMessage() {
        return _lastErrorMessage;
    }

    inline QString const & lastErrorMessage() const {
        return _lastErrorMessage;
    }

    inline unsigned & maxWidth() {
        return _maxWidth;
    }

    inline unsigned maxWidth() const {
        return _maxWidth;
    }

    inline unsigned & maxHeight() {
        return _maxHeight;
    }

    inline unsigned maxHeight() const {
        return _maxHeight;
    }

    inline unsigned & imageQuality() {
        return _imageQuality;
    }

    inline unsigned imageQuality() const {
        return _imageQuality;
    }

    inline unsigned & lastErrorCode() {
        return _lastErrorCode;
    }

    inline unsigned lastErrorCode() const {
        return _lastErrorCode;
    }

    inline QVector<Album> & albums() {
        return _albums;
    }

    inline const QVector<Album>& albums() const {
        return _albums;
    }

    inline RajceCommandType lastCommand() const {
        return _lastCommand;
    }

    inline RajceCommandType& lastCommand() {
        return _lastCommand;
    }

private:
    QString _sessionToken;
    QString _nickname;
    QString _username;
    QString _albumToken;
    QString _lastErrorMessage;
    unsigned _maxWidth;
    unsigned _maxHeight;
    unsigned _imageQuality;
    unsigned _lastErrorCode;
    RajceCommandType _lastCommand;
    QVector<Album> _albums;
};

}

QDebug operator<<(QDebug d, const KIPIRajceExportPlugin::SessionState & s);

#endif // KIPIRAJCEEXPORTPLUGIN_SESSIONSTATE_H
