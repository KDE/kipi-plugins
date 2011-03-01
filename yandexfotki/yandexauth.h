/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-14
 * Description : Yandex authentication module
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
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
 * ============================================================
 *
 * See http://api.yandex.ru/ (ru)
 *   for detailes
 */

#ifndef YANDEXAUTH_H
#define YANDEXAUTH_H

// Qt includes

#include <QString>

namespace YandexAuth
{

QString makeCredentials(const QString& publicKey, const QString& login,
                        const QString& password);

} // namespace YandexAuth

#endif // YANDEXAUTH_H
