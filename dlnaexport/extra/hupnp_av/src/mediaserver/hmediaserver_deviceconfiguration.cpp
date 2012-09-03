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

#include "hmediaserver_deviceconfiguration.h"
#include "hmediaserver_deviceconfiguration_p.h"

#include "../contentdirectory/hcontentdirectory_serviceconfiguration.h"

namespace Herqq
{

namespace Upnp
{

namespace Av
{

/*******************************************************************************
 * HMediaServerDeviceConfigurationPrivate
 ******************************************************************************/
HMediaServerDeviceConfigurationPrivate::HMediaServerDeviceConfigurationPrivate() :
    m_cdsConf(0)
{
}

HMediaServerDeviceConfigurationPrivate::~HMediaServerDeviceConfigurationPrivate()
{
}

/*******************************************************************************
 * HMediaServerDeviceConfiguration
 ******************************************************************************/
HMediaServerDeviceConfiguration::HMediaServerDeviceConfiguration() :
    h_ptr(new HMediaServerDeviceConfigurationPrivate())
{
}

HMediaServerDeviceConfiguration::~HMediaServerDeviceConfiguration()
{
    delete h_ptr;
}

void HMediaServerDeviceConfiguration::doClone(HClonable* target) const
{
    HMediaServerDeviceConfiguration* conf =
        dynamic_cast<HMediaServerDeviceConfiguration*>(target);

    if (!target)
    {
        return;
    }

    conf->h_ptr->m_cdsConf.reset(contentDirectoryServiceConfiguration()->clone());
}

HMediaServerDeviceConfiguration* HMediaServerDeviceConfiguration::newInstance() const
{
    return new HMediaServerDeviceConfiguration();
}

void HMediaServerDeviceConfiguration::setContentDirectoryConfiguration(
    const HContentDirectoryServiceConfiguration& arg)
{
    if (arg.isValid())
    {
        h_ptr->m_cdsConf.reset(arg.clone());
    }
}

const HContentDirectoryServiceConfiguration*
    HMediaServerDeviceConfiguration::contentDirectoryServiceConfiguration() const
{
    return h_ptr->m_cdsConf.data();
}

bool HMediaServerDeviceConfiguration::isValid() const
{
    return contentDirectoryServiceConfiguration();
}

HMediaServerDeviceConfiguration* HMediaServerDeviceConfiguration::clone() const
{
    return static_cast<HMediaServerDeviceConfiguration*>(HClonable::clone());
}

}
}
}
