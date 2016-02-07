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

#define CXN_BDADDR_STR_LEN          17   // 6 two-digit hex values plus 5 colons
#define CXN_MAX_INQUIRY_RETRY       3
#define CXN_DELAY_NEXT_INQUIRY      15
#define CXN_SUCCESS                 0
#define CXN_ERROR                   1
#define CXN_DEFAULT_LISTEN_BACKLOG  4

/**
 * This structure describes a Bluetooth's Device name & address
 * as it was returned from the OS-dependent bluetooth inquiry.
 */
struct BluetoothInqItem {
    std::string name;
    SOCKADDR_BTH address;

    BluetoothInqItem(std::string const& n, PSOCKADDR_BTH & addr) {
        name.assign(n);
        memcpy_s(&address, sizeof(SOCKADDR_BTH), addr, sizeof(SOCKADDR_BTH));
        address.addressFamily = AF_BTH;
        address.port = 0;
        memcpy((void *)&address.serviceClassId, &RFCOMM_PROTOCOL_UUID, sizeof(GUID));
    }
};

/**
 * This class abstracts the basics for the Bluetooth Protocol.
 * Instantiate this class in order to create a boost asio
 * stream protocol, and to keep track of the correct socket and
 * endpoint types.
 */
class BluetoothProto {
public:
    boost::asio::generic::stream_protocol stream_protocol_;
    typedef boost::asio::basic_stream_socket<boost::asio::generic::stream_protocol> socket;
    typedef boost::asio::generic::basic_endpoint<boost::asio::generic::stream_protocol> endpoint;

    BluetoothProto()
        : stream_protocol_(AF_BTH, BTHPROTO_RFCOMM) {}
    virtual ~BluetoothProto() {}

    boost::asio::generic::stream_protocol & streamProtocol() {
        return stream_protocol_;
    }

#if defined(_WIN32)
    static void bluetoothInquire(std::vector<BluetoothInqItem> & vOut) {
        vOut.clear();
        const size_t QUERY_SET_COUNT = 4092;
        const size_t QUERY_SET_SIZE = QUERY_SET_COUNT * sizeof(WSAQUERYSETA);

        int result = CXN_SUCCESS;
        DWORD inqFlags =
            LUP_RETURN_NAME | LUP_RETURN_ADDR | LUP_RETURN_BLOB;
        HANDLE inqHandle = 0;
        WSAQUERYSETA *querySet = (WSAQUERYSETA *) calloc(QUERY_SET_COUNT, sizeof(WSAQUERYSETA));

        querySet->dwNameSpace = NS_BTH;
        querySet->dwSize = QUERY_SET_SIZE;

        result = WSALookupServiceBeginA(querySet,
                                        inqFlags
                                        | LUP_CONTAINERS
                                        | LUP_FLUSHCACHE,
                                        &inqHandle);
        if (NO_ERROR == result && inqHandle) {
            do {
                DWORD size = QUERY_SET_SIZE;
                result = WSALookupServiceNextA(inqHandle,
                                               inqFlags,
                                               &size,
                                               querySet);
                if (result != NOERROR) continue;

                if (querySet->lpszServiceInstanceName != NULL &&
                    querySet->lpcsaBuffer->RemoteAddr.lpSockaddr != NULL) {

                    PSOCKADDR_BTH sockAddr = reinterpret_cast<PSOCKADDR_BTH>(
                        querySet->lpcsaBuffer->RemoteAddr.lpSockaddr);
                    const char * name = querySet->lpszServiceInstanceName;
                    vOut.push_back(
                        BluetoothInqItem(name, sockAddr));
                }
            } while (NO_ERROR == result);

            WSALookupServiceEnd(inqHandle);
        }

        free(querySet);
    }

#elif defined(LINUX)
    // TODO: write a linux variant for bluetooth inquiry
    static void bluetoothInquire(std::vector<BluetoothInqItem> & vOut) {
    }

#endif

};

