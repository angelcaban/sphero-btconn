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

/** \addtogroup bluetoothconn
 *  @{
 */

/** \file bluetoothconn.h
 * Include file for consumers of the Bluetooth Connection Library.
 */

#pragma once

#include <winsock2.h>
#include <ws2bth.h>
#include <boost/asio.hpp>
#include <boost/asio/generic/stream_protocol.hpp>
#include <boost/throw_exception.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <deque>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>

#include <btconn/BtLogger.h>
#include <btconn/NotConnectedException.h>
#include <btconn/BtConnection.h>
#include <btconn/SpheroHandler.h>

#define BTCONN_VERSION_MAJOR 0
#define BTCONN_VERSION_MINOR 1

/** @} */

