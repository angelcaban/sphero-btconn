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

#pragma once

typedef std::shared_ptr<SpheroServerResponse> SpheroResponsePtr;
typedef std::shared_ptr<SpheroClientCommand> SpheroCmdPtr;

class SpheroHandler
{
    bt::BtConnection<SpheroMessage> spheroConn_;
    std::size_t seqNum;

public:
    SpheroHandler(SOCKADDR_BTH * bluetoothAddress);
    virtual ~SpheroHandler();

    bt::BtConnection<SpheroMessage> & getConnection() {
        return spheroConn_;
    }

    template<unsigned char DID, unsigned char CID>
    void sendCommand(SpheroCommand<DID, CID> & cmd) {
        if (!spheroConn_.isConnected())
            throw NotConnectedException(
                    "Unable to send Sphero Commands - Not Connected");

        auto msg = cmd.generateCommandMessage<SpheroClientCommand>(
            (unsigned char)seqNum);
        msg->assemble();
        spheroConn_.send(msg);
        ++seqNum;
    }

    SpheroResponsePtr readResponse();
};

