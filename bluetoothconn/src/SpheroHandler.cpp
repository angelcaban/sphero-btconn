/*
 * Sphero C++ Bluetooth Library
 * Copyright(C) 2016 Angel Caban
 *
 * This program is free software : you can redistribute it and / or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see <http://www.gnu.org/licenses/>.
 */

#include "btconn/stdafx.h"
#include "btconn/BtLogger.h"
#include "btconn.h"
#include "btconn/SpheroCommands.h"
#include "btconn/SpheroPacketTypes.h"
#include "btconn/NotConnectedException.h"
#include "btconn/SpheroHandler.h"

using namespace std;

#define CONNECTION_RETRY 5
#define CONNECTION_SLEEP 5

SpheroHandler::SpheroHandler(SOCKADDR_BTH * bluetoothAddress) :
        seqNum(0) {
    if (bluetoothAddress == nullptr) return;

    // TODO: Make Bluetooth Connect Retry # configurable
    for (int i = 0; i <= CONNECTION_RETRY; ++i) {
        try {
            spheroConn_.setEndpoint(*bluetoothAddress);
            spheroConn_.connect();

            BtLogger::log() << "Successfully Connected to bluetooth device "
                << std::hex << bluetoothAddress->btAddr << std::endl;
            return;
        } catch (boost::system::system_error & sysexc) {
            BtLogger::log()
                << "Caught exception while connecting to device - "
                << sysexc.what() <<
                ((i < CONNECTION_RETRY) ?
                    " -- retrying " :
                    " -- quitting ") << std::endl;

            if (i == CONNECTION_RETRY)
                throw NotConnectedException(sysexc.what());
        } catch (std::exception & exc) {
            BtLogger::log()
                << "Caught exception while connecting to device - "
                << exc.what() <<
                ((i < CONNECTION_RETRY) ?
                    " -- retrying " :
                    " -- quitting ") << std::endl;

            if (i == CONNECTION_RETRY)
                throw NotConnectedException(exc.what());
        }
        // TODO: Make Connect Retry Sleep configurable
        Sleep(CONNECTION_SLEEP * 1000);
    }
}

SpheroHandler::~SpheroHandler() {
}

SpheroResponsePtr SpheroHandler::readResponse() {
    if (!spheroConn_.isConnected())
        throw NotConnectedException(
                "Unable to talk to Sphero - Not Connected");

    auto resPtr = spheroConn_.read<SpheroServerResponse>();
    if (resPtr) resPtr->parseFromInternalData();
    return (resPtr);
}
