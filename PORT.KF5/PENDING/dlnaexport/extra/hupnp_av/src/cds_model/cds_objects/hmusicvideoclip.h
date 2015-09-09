/*
 *  Copyright (C) 2011 Tuomo Penttinen, all rights reserved.
 *
 *  Author: Tuomo Penttinen <tp@herqq.org>
 *
 *  This file is part of Herqq UPnP Av (HUPnPAv) library.
 *
 *  Herqq UPnP Av is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Herqq UPnP Av is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Herqq UPnP Av. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HMUSICVIDEOCLIP_H_
#define HMUSICVIDEOCLIP_H_

#include <HUpnpAv/HVideoItem>

namespace Herqq
{

namespace Upnp
{

namespace Av
{

class HMusicVideoClipPrivate;

/*!
 * \brief This class represents video content that is a clip supporting a song.
 *
 * The class identifier specified by the AV Working
 * Committee is \c object.item.videoItem.musicVideoClip .
 *
 * \headerfile hmusicvideoclip.h HMusicVideoClip
 *
 * \ingroup hupnp_av_cds_objects
 *
 * \remarks This class is not thread-safe.
 */
class H_UPNP_AV_EXPORT HMusicVideoClip :
    public HVideoItem
{
Q_OBJECT
H_DISABLE_COPY(HMusicVideoClip)
H_DECLARE_PRIVATE(HMusicVideoClip)

protected:

    /*!
     * Constructs a new instance.
     *
     * \param clazz specifies the UPnP class of the object. This cannot be empty.
     *
     * \param cdsType specifies the CDS type of the object. This cannot be
     * HObject::UndefinedCdsType.
     *
     * \sa isInitialized()
     */
    HMusicVideoClip(const QString& clazz = sClass(), CdsType cdsType = sType());
    HMusicVideoClip(HMusicVideoClipPrivate&);

    // Documented in HClonable
    virtual HMusicVideoClip* newInstance() const;

public:

    /*!
     * \brief Creates a new instance.
     *
     * \param title specifies the title of the object.
     *
     * \param parentId specifies the ID of the object that contains this
     * object. If the object has no parent, this has to be left empty.
     *
     * \param id specifies the ID of this object. If this is not specified,
     * a unique identifier within the running process is created for the object.
     *
     * \sa isValid()
     */
    HMusicVideoClip(
        const QString& title,
        const QString& parentId,
        const QString& id = QString());

    /*!
     * \brief Destroys the instance.
     */
    virtual ~HMusicVideoClip();

    /*!
     * \brief Retrieves the artists of this item.
     *
     * upnp:artist, ContentDirectory:3, Appendix B.3.1.
     *
     * \return The artists of this item.
     *
     * \sa setArtists()
     */
    QList<HPersonWithRole> artists() const;

    /*!
     * \brief Indicates the type of storage medium used for the content.
     *
     * upnp:storageMedium, ContentDirectory:3, Appendix B.6.5.
     *
     * \return The type of storage medium used for the content.
     *
     * \sa setStorageMedium()
     */
    HStorageMedium storageMedium() const;

    /*!
     * \brief Returns the albums to which this item belongs.
     *
     * upnp:album, ContentDirectory:3, Appendix B.4.2.
     *
     * \return The albums to which this item belongs.
     *
     * \sa setAlbums()
     */
    QStringList albums() const;

    /*!
     * \brief Returns the start time of a scheduled program.
     *
     * \return The start time of a scheduled program.
     *
     * \sa setScheduledStartTime()
     */
    HScheduledTime scheduledStartTime() const;

    /*!
     * \brief Returns the end time of a scheduled program.
     *
     * \return The end time of a scheduled program.
     *
     * \sa setScheduledEndTime()
     */
    HScheduledTime scheduledEndTime() const;

    /*!
     * \brief Returns the directors of this item.
     *
     * upnp:director, ContentDirectory:3, Appendix B.3.5.
     *
     * \return The directors of this item.
     *
     * \sa setDirectors()
     */
    QStringList directors() const;

    /*!
     * \brief Returns the contributors of this item.
     *
     * dc:contributor, ContentDirectory:3, Appendix B.3.7.
     *
     * \return The contributors of this item.
     *
     * \sa setContributors()
     */
    QStringList contributors() const;

    /*!
     * \brief Returns the primary date of the content.
     *
     * dc:date, ContentDirectory:3, Appendix B.7.6.
     *
     * \return The primary date of the content.
     */
    QDateTime date() const;

    /*!
     * \brief Sets the artists of this item.
     *
     * \param arg specifies the artists of this item.
     *
     * \sa artists()
     */
    void setArtists(const QList<HPersonWithRole>& arg);

    /*!
     * \brief Specifies the type of storage medium used for the content.
     *
     * \param arg specifies the type of storage medium used for the content.
     *
     * \sa storageMedium()
     */
    void setStorageMedium(const HStorageMedium& arg);

    /*!
     * \brief Specifies the albums to which this item belongs.
     *
     * \param arg the albums to which this item belongs.
     *
     * \sa albums()
     */
    void setAlbums(const QStringList& arg);

    /*!
     * \brief Specifies the the start time of a scheduled program.
     *
     * \param arg specifies the start time of a scheduled program.
     *
     * \sa scheduledStartTime()
     */
    void setScheduledStartTime(const HScheduledTime& arg);

    /*!
     * \brief Specifies the the end time of a scheduled program.
     *
     * \param arg specifies the end time of a scheduled program.
     *
     * \sa scheduledEndTime()
     */
    void setScheduledEndTime(const HScheduledTime& arg);

    /*!
     * \brief Sets the directors of this item.
     *
     * \param arg specifies the directors of this item.
     *
     * \sa directors()
     */
    void setDirectors(const QStringList& arg);

    /*!
     * \brief Specifies the contributors of this item.
     *
     * \param arg specifies the contributors of this item.
     *
     * \sa contributors()
     */
    void setContributors(const QStringList& arg);

    /*!
     * \brief Specifies the primary date of the content.
     *
     * \param arg specifies the primary date of the content.
     *
     * \sa date()
     */
    void setDate(const QDateTime& arg);

    /*!
     * \brief Returns the CdsType value of this class.
     *
     * \return The CdsType value of this class.
     */
    inline static CdsType sType() { return MusicVideoClip; }

    /*!
     * \brief Returns the class identifier specified by the AV Working Committee.
     *
     * \return The class identifier specified by the AV Working Committee.
     */
    inline static QString sClass() { return "object.item.videoItem.musicVideoClip"; }

    /*!
     * Creates a new instance with no title or parentID.
     *
     * \return a pointer to the newly created instance.
     *
     * \remarks the ownership of the object is transferred to the caller. Make sure
     * to delete the object.
     */
    inline static HMusicVideoClip* create() { return new HMusicVideoClip(); }
};

}
}
}

#endif /* HMUSICVIDEOCLIP_H_ */
