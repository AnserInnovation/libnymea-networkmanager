/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 * Copyright (C) 2019 Simon Stürz <simon.stuerz@nymea.io>                  *
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

#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <QObject>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>

#include "../networkmanager.h"

class NetworkService : public QObject
{
    Q_OBJECT

public:
    enum NetworkServiceCommand {
        NetworkServiceCommandInvalid = -1,
        NetworkServiceCommandEnableNetworking   = 0x00,
        NetworkServiceCommandDisableNetworking  = 0x01,
        NetworkServiceCommandEnableWireless     = 0x02,
        NetworkServiceCommandDisableWireless    = 0x03
    };
    Q_ENUM(NetworkServiceCommand)

    enum NetworkServiceResponse {
        NetworkServiceResponseSuccess                      = 0x00,
        NetworkServiceResponseIvalidValue                  = 0x01,
        NetworkServiceResponseNetworkManagerNotAvailable   = 0x02,
        NetworkServiceResponseWirelessNotAvailable         = 0x03,
        NetworkServiceResponseUnknownError                 = 0x04,
    };
    Q_ENUM(NetworkServiceResponse)

    explicit NetworkService(QLowEnergyService *service, NetworkManager *networkManager, QObject *parent = nullptr);

    QLowEnergyService *service();

    static QLowEnergyServiceData serviceData(NetworkManager *networkManager);
    static QByteArray getNetworkManagerStateByteArray(const NetworkManager::NetworkManagerState &state);

private:
    QLowEnergyService *m_service = nullptr;
    NetworkManager *m_networkManager = nullptr;

    void sendResponse(const NetworkServiceResponse &response);

    NetworkServiceCommand verifyCommand(const QByteArray &commandData);

private slots:
    // Service
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void serviceError(const QLowEnergyService::ServiceError &error);

    // Commands
    void processCommand(const NetworkServiceCommand &command);

public slots:
    bool onNetworkManagerStateChanged();
    bool onNetworkingEnabledChanged();
    bool onWirelessEnabledChanged();

};

#endif // NETWORKSERVICE_H
