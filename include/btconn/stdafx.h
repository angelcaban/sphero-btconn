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

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


#include <winsock2.h>
#include <ws2bth.h>
#include <boost/asio.hpp>
#include <boost/asio/generic/stream_protocol.hpp>
#include <boost/throw_exception.hpp>
#include <boost/atomic.hpp>

#include <limits>
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
