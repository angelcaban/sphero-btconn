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

#include "BluetoothProto.h"
#include "SpheroPacketTypes.h"

namespace bt {

/**
 * Abstracts a persistent connection to a single bluetooth device.
 *
 * A bluetooth connection instantiated with this class has the ability
 * to connect, read & write to an bluetooth device via RFCOMM protocol.
 *
 * \tparam  MSGTYPE     The base class for a message that is capable of
 *                      storing raw data messages (raw data MUST be
 *                      accessible via a data() method, MUST return a
 *                      pointer or iterator to unsigned char and MUST
 *                      have a length() method that returns raw data size).
 */
template <class MSGTYPE>
class BtConnection {
public:
    typedef std::shared_ptr<MSGTYPE> MsgPtr;
    typedef std::array<unsigned char, 1024> RawData;

private:

    volatile bool connected_;
    volatile bool shutdown_;

    boost::asio::io_service io_;
    BluetoothProto proto_;
    BluetoothProto::socket socket_;
    BluetoothProto::endpoint endpoint_;

    std::deque<MsgPtr> messageQueue_;
    std::deque<RawData> readQueue_;
    RawData recv_buffer_;
    std::thread iothread_;
    std::mutex readQueueMutex;

public:
    /**
     * Default c-tor instantiates and readies a socket for
     * bluetooth communications.
     */
    BtConnection() :
        connected_(false), shutdown_(false), socket_(io_) {
        socket_.open(proto_.streamProtocol());
        recv_buffer_.fill('\0');
    }

    /**
     * Instantiates and readies a socket for bluetooth
     * communications to the specified bluetooth device.
     */
    BtConnection(const SOCKADDR_BTH & socket_address) :
        connected_(false), shutdown_(false), socket_(io_) {
        socket_.open(proto_.streamProtocol());
        recv_buffer_.fill('\0');

        setEndpoint(socket_address);
    }

    /**
     * Close socket to a bluetooth devices and cleans up thread resources.
     */
    ~BtConnection() {
        close();
        if (iothread_.joinable())
            iothread_.join();
    }

    /**
     * Connect to the already-provided device.
     */
    void connect() {
        socket_.connect(endpoint_);
        connected_ = true;

        readAsync();
        iothread_ = std::thread([this]() { io_.run(); });
    }

    void close() {
        shutdown_ = true;
        if (socket_.is_open()) {
            io_.post([this]() {
                socket_.close();
                // socket_.shutdown(boost::asio::socket_base::shutdown_both);
            });
        }
    }

    /**
     * Read incomming data of a specific type from the input queue.
     * This method blocks until data is available.
     *
     * @note This method expects that the current state of this
     *       object has an active connection, otherwise an exception
     *       is thrown.
     */
    template <class T=MSGTYPE>
    std::shared_ptr<T> read() {
        if (!connected_)
            throw NotConnectedException("Unable to perform a read - Not connected");

        while (readQueue_.empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::lock_guard<std::mutex> readQueueLock(readQueueMutex);
        auto ret = std::shared_ptr<T>(new T(readQueue_.back()));
        readQueue_.pop_back();

        return (ret);
    }

    /**
     * Send a message to the connected device.
     *
     * @note This method expects that the current state of this
     *       object has an active connection, otherwise an exception
     *       is thrown.
     */
    void send(MsgPtr msg) {
        if (!connected_)
            throw NotConnectedException("Unable to perform a write - Not connected");

        io_.post([this, msg]() {
            bool qEmpty = messageQueue_.empty();
            messageQueue_.push_back(msg);
            if (qEmpty) doSend();
        });
    }

    bool isConnected() const {
        return connected_;
    }
    bool isShuttingDown() const {
        return shutdown_;
    }
    void shutdown() {
        shutdown_ = true;
    }

    /**
     * Use a copy of the socket address to the bluetooth device
     * as the communications endpoint.
     */
    void setEndpoint(const SOCKADDR_BTH & socket_address) {
        endpoint_ = BluetoothProto::endpoint(
            static_cast<const void*>(&socket_address),
            sizeof(SOCKADDR_BTH));
    }

private:

    void doSend() {
        auto buf = boost::asio::buffer(
            (void *)messageQueue_.front()->data(),
            messageQueue_.front()->length());

        async_write(socket_, buf,
            [this](boost::system::error_code ec, size_t bytes) {
                if (!ec) {
                    BtLogger::log()
                        << "Successfully sent out 0x" << std::hex
                        << bytes << " bytes" << std::endl;

                    messageQueue_.pop_front();
                    if (!messageQueue_.empty() && !shutdown_) doSend();
                } else {
                    BtLogger::log()
                        << "Closing connection on doSend due to err "
                        << ec << std::endl;
                    close();
                }
            });
    }

    void readAsync() {
        BtLogger::log() << "Sending socket_.async_read_some..." << std::endl;

        if (shutdown_) return;

        socket_.async_read_some(boost::asio::buffer(recv_buffer_),
            [this](const boost::system::error_code & err, std::size_t bytes) {
                if (shutdown_) return;

                // TODO: Don't know if single responses can span multiple reads.
                // TODO: If so, need to rethink how async reads should be done.
                if (!err) {
                    BtLogger::log()
                        << "Successfully read 0x" << std::hex
                        << bytes << " bytes." << std::endl;

                    readHandler(bytes);
                } else {
                    BtLogger::log()
                        << "Caught error while attempting to read data - "
                        << err.message() << std::endl;

                    readAsync();
                }
            });
    }

    void readHandler(size_t len) {
        std::lock_guard<std::mutex> readQueueLock(readQueueMutex);
        readQueue_.push_back(RawData()); // Allocate array and assign to end of readQueue

        readQueue_.back().fill((const unsigned char)'\0');

        // TODO: 0xff reading is probably too sphero-specific and should be moved.

        // 0xff marks the beginning of a message...
        auto beginning = recv_buffer_.begin();
        size_t offset = 0;
        while ((*beginning) != 0xff && beginning != recv_buffer_.end()) {
            ++beginning; ++offset;
        }

        std::copy(beginning, beginning + (len - offset),
                  readQueue_.back().begin());

        recv_buffer_.fill('\0');
        if (!shutdown_) readAsync();
    }
};

}
