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

boost::atomic<size_t> BtLogger::counter_(0);

std::fstream & BtLogger::logStream() {
    static std::fstream log_stream_("sphero.log", std::ios::app | std::ios::out);

    counter_.fetch_add(1);
    return log_stream_;
}

void BtLogger::releaseStream() {
    counter_.fetch_sub(1);
    if (counter_.load(boost::memory_order_relaxed) == 0)
        logStream_.close();
}
