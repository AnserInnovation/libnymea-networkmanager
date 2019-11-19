/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 * Copyright (C) 2018-2019 Simon Stürz <simon.stuerz@nymea.io>             *
 *                                                                         *
 * This file is part of libnymea-networkmanager.                           *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 2.1 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library; If not, see                           *
 *  <http://www.gnu.org/licenses/>.                                        *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*!
    \class WirelessAccessPoint
    \brief Represents a discovered wireless access point.
    \inmodule nymea-networkmanager
    \ingroup networkmanager


*/

/*! \enum WirelessAccessPoint::ApSecurityMode
    \value ApSecurityModeNone
    \value ApSecurityModePairWep40
    \value ApSecurityModePairWep104
    \value ApSecurityModePairTkip
    \value ApSecurityModePairCcmp
    \value ApSecurityModeGroupWep40
    \value ApSecurityModeGroupWep104
    \value ApSecurityModeGroupTkip
    \value ApSecurityModeGroupCcmp
    \value ApSecurityModeKeyMgmtPsk
    \value ApSecurityModeKeyMgmt8021X
    \value ApSecurityModeKeyMgmtSae
*/

/*! \fn void WirelessAccessPoint::signalStrengthChanged();
    This signal will be emitted when the signalStrength of this \l{WirelessAccessPoint} has changed.
*/

#include "wirelessaccesspoint.h"
#include "networkmanagerutils.h"

/*! Constructs a new \l{WirelessAccessPoint} with the given dbus \a objectPath and \a parent. */
WirelessAccessPoint::WirelessAccessPoint(const QDBusObjectPath &objectPath, QObject *parent) :
    QObject(parent),
    m_objectPath(objectPath)
{
    QDBusInterface accessPointInterface(NetworkManagerUtils::networkManagerServiceString(), m_objectPath.path(), NetworkManagerUtils::accessPointInterfaceString(), QDBusConnection::systemBus());
    if(!accessPointInterface.isValid()) {
        qCWarning(dcNetworkManager()) << "Invalid access point dbus interface";
        return;
    }

    // Init properties
    setSsid(accessPointInterface.property("Ssid").toString());
    setMacAddress(accessPointInterface.property("HwAddress").toString());
    setFrequency(accessPointInterface.property("Frequency").toDouble() / 1000);
    setSignalStrength(accessPointInterface.property("Strength").toInt());
    m_capabilities = static_cast<WirelessAccessPoint::ApFlags>(accessPointInterface.property("Flags").toUInt());
    setWpaFlags(WirelessAccessPoint::ApSecurityModes(accessPointInterface.property("WpaFlags").toUInt()));
    setRsnFlags(WirelessAccessPoint::ApSecurityModes(accessPointInterface.property("RsnFlags").toUInt()));
    setIsProtected(m_rsnFlags != 0);

    qCDebug(dcNetworkManager()) << ssid() << "WPA flags:" << m_wpaFlags;
    qCDebug(dcNetworkManager()) << ssid() << "RSN flags:" << m_rsnFlags;
    qCDebug(dcNetworkManager()) << ssid() << "Capabilities:" << m_capabilities;

    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), objectPath.path(), NetworkManagerUtils::accessPointInterfaceString(), "PropertiesChanged", this, SLOT(onPropertiesChanged(QVariantMap)));
}

/*! Returns the dbus object path of this \l{WirelessAccessPoint}. */
QDBusObjectPath WirelessAccessPoint::objectPath() const
{
    return m_objectPath;
}

/*! Returns the ssid of this \l{WirelessAccessPoint}. */
QString WirelessAccessPoint::ssid() const
{
    return m_ssid;
}

void WirelessAccessPoint::setSsid(const QString &ssid)
{
    m_ssid = ssid;
}

/*! Returns the mac address of this \l{WirelessAccessPoint}. */
QString WirelessAccessPoint::macAddress() const
{
    return m_macAddress;
}

void WirelessAccessPoint::setMacAddress(const QString &macAddress)
{
    m_macAddress = macAddress;
}

/*! Returns the frequency of this \l{WirelessAccessPoint}. (2.4 GHz or 5GHz) */
double WirelessAccessPoint::frequency() const
{
    return m_frequency;
}

void WirelessAccessPoint::setFrequency(double frequency)
{
    m_frequency = frequency;
}

/*! Returns the signal strength in percentage [0, 100] % of this \l{WirelessAccessPoint}. */
int WirelessAccessPoint::signalStrength() const
{
    return m_signalStrength;
}

void WirelessAccessPoint::setSignalStrength(int signalStrength)
{
    m_signalStrength = signalStrength;
    emit signalStrengthChanged();
}

void WirelessAccessPoint::setWpaFlags(WirelessAccessPoint::ApSecurityModes wpaFlags)
{
    m_wpaFlags = wpaFlags;
}

void WirelessAccessPoint::setRsnFlags(WirelessAccessPoint::ApSecurityModes rsnFlags)
{
    m_rsnFlags = rsnFlags;
}

void WirelessAccessPoint::setIsProtected(bool isProtected)
{
    m_isProtected = isProtected;
}

/*! Returns true if this \l{WirelessAccessPoint} is password protected. */
bool WirelessAccessPoint::isProtected() const
{
    return m_isProtected;
}

WirelessAccessPoint::ApFlags WirelessAccessPoint::capabilities() const
{
    return m_capabilities;
}

/*! Returns the WPA security flags of this \l{WirelessAccessPoint}.

    This flags are describing the access point's capabilities according to WPA (Wifi Protected Access).

    \sa WirelessAccessPoint::ApSecurityModes
*/
WirelessAccessPoint::ApSecurityModes WirelessAccessPoint::wpaFlags() const
{
    return m_wpaFlags;
}

/*! Returns the RSN security flags of this \l{WirelessAccessPoint}.

    Flags describing the access point's capabilities according to the RSN (Robust Secure Network) protocol.

    \sa WirelessAccessPoint::ApSecurityModes
*/
WirelessAccessPoint::ApSecurityModes WirelessAccessPoint::rsnFlags() const
{
    return m_rsnFlags;
}

void WirelessAccessPoint::onPropertiesChanged(const QVariantMap &properties)
{
    if (properties.contains("Strength"))
        setSignalStrength(properties.value("Strength").toInt());

}

QDebug operator<<(QDebug debug, WirelessAccessPoint *accessPoint)
{
    debug.nospace() << "AccessPoint(" << accessPoint->signalStrength() << "%, "
                           <<  accessPoint->frequency()<< " GHz, "
                            << accessPoint->ssid() << ", " <<
                               (accessPoint->isProtected() ? "protected" : "open" )
                            << ")";
    return debug.space();
}
