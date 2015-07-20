/*
 Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either 
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public 
 License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PURPOSEJOB_H
#define PURPOSEJOB_H

#include <KJob>
#include <QJsonArray>
#include <QJsonObject>
#include <QMimeData>
#include <QUrl>

namespace KIPIGoogleServicesPlugin
{

struct JobPrivate;

/**
 * @brief Job that will actually perform the sharing
 *
 * When start is called, the sharing process will start and when the job
 * emits finished, we'll know it's over.
 *
 * The start method called shouldn't be called before all data has been
 * filled in. isReady can be used to check whether it's all ready to go,
 * these arguments will have to be filled by the file provided by
 * configSourceCode() and should end up defining all the arguments defined
 * by neededArguments.
 */
class Job : public KJob
{
Q_OBJECT
/**
 * Represents the data the job will have available to perform its task
 */
Q_PROPERTY(QJsonObject data READ data CONSTANT)
public:
    Job(QObject* parent = 0);
    virtual ~Job();

    /**
     * Should only be called after constructing
     *
     * @internal
     */
    void setData(const QJsonObject &data);
    QJsonObject data() const;

Q_SIGNALS:
    void output(const QJsonObject& output);
    void dataChanged();

private:
    Q_DECLARE_PRIVATE(Job);
    QScopedPointer<JobPrivate> const d_ptr;
};

}

#endif
