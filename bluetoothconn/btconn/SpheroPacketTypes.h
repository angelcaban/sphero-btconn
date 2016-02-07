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

#include "SpheroCommands.h"

typedef std::vector<unsigned char> SpheroPacket;

/**
 * A generic representation for communciation to and from a Sphero.
 */
class SpheroMessage : public SpheroPacket
{
public:
    SpheroMessage() {
    }
    virtual ~SpheroMessage() {
    }
    SpheroMessage(unsigned char const* __s, size_type count) :
        SpheroPacket(__s, __s + count) {
    }
    SpheroMessage(std::initializer_list<unsigned char> init) :
        SpheroPacket(init) {
    }
    template <std::size_t SIZE>
    SpheroMessage(std::array<unsigned char, SIZE> & init) :
        SpheroPacket(init.begin(), init.begin() + SIZE) {
    }

    virtual void assemble() = 0;
    virtual unsigned char dataLength() = 0;
    virtual unsigned char* msgData() = 0;

    virtual SpheroPacket::size_type length() const {
        return this->size();
    }
};

/**
 * This class specializes responses from the Sphero Device.
 */
class SpheroServerResponse : public SpheroMessage {
    unsigned char SOP1; // start-of-packet 1 (always set to 0xFF)
    unsigned char SOP2; // start-of-packet 2 (0xFF means awk sync, 0xFE means awk async)
    unsigned char MRSP; // Message Response
    unsigned char SEQ;  // Sequence # or 0x00 if async
    unsigned char DLEN; // Data segment length
    unsigned char DATA[256]; // Data segment
    unsigned char chksum;

public:
    SpheroServerResponse()
        : SOP1(0xFF),
          SOP2(0xFC),
          MRSP(0x00),
          SEQ(0x00),
          DLEN(0x00),
          chksum(0x00) {
        memset((void*)&DATA, 0, sizeof(DATA));
    }
    SpheroServerResponse(unsigned char const* __s, size_type count) :
        SpheroMessage(__s, count) {
    }
    SpheroServerResponse(std::initializer_list<unsigned char> init) :
        SpheroMessage(init) {
    }
    template <std::size_t SIZE>
    SpheroServerResponse(std::array<unsigned char, SIZE> & init) :
        SpheroMessage(init) {
    }

    /**
     * Gobble up the internal array and assign values to each
     * pertinent section of a Sphero Response Packet.
     */
    SpheroServerResponse const& parseFromInternalData() {
        SOP1 = (*this)[0];
        SOP2 = (*this)[1];
        MRSP = (*this)[2];
        SEQ = (*this)[3];
        DLEN = (*this)[4];
        memcpy_s(&DATA, sizeof(DATA), &(this->data()[5]), (size_t)DLEN);
        return (*this);
    }

    unsigned char startOfPacket() {
        return SOP2;
    }
    unsigned char messageResponse() {
        return MRSP;
    }
    unsigned char sequenceNum() {
        return SEQ;
    }
    unsigned char dataLength() {
        return DLEN;
    }
    unsigned char* msgData() {
        return (DATA);
    }
    void assemble() {
        this->insert(this->end(), { SOP1, SOP2, MRSP, SEQ, DLEN });
        this->insert(this->end(), DATA, DATA + DLEN);
    }

    virtual SpheroPacket::size_type length() const {
        return 6 + DLEN;
    }

    /**
     * Translate a chunk of the payload into a numerical.
     *
     * @param startIndex A number between 0 and dataLength(),
     *                   where 0 marks the start of the msgData().
     *
     * @return An unsigned short that represents a part of msgData.
     * @note   If startIndex >= dataLength() - 1, then we return 0.
     * @note   If startIndex == dataLength() - 2, then we return the
     *         last byte shifted left 8 bits.
     */
    template <typename T>
    T dataToNumerical(size_t startIndex = 0) {
        T ret = 0;

        for (int limiter = 0;
             startIndex < dataLength() && limiter < sizeof(T) - 1;
             ++startIndex, ++limiter) {
                ret |= msgData()[startIndex];
                ret <<= 8;
        }
        if (startIndex < dataLength()) ret |= msgData()[startIndex];

        return ret;
    }

    /**
     * Translate a chunk of the payload into a string.
     *
     * @param startIndex    The index for the first character.
     * @param len           The length of the expected string or the
     *                      remaining contents if len > dataLength().
     */
    std::string dataToString(size_t startIndex = 0,
                             size_t len = std::string::npos) {
        if (len > DLEN) len = DLEN;
        if (startIndex > DLEN) return ("");
        std::string ret(len, '\0');

        for (size_t i = startIndex; i < startIndex + len; ++i) {
            ret[i] = (char) msgData()[i];
        }
        return (ret);
    }

    std::string messageResponseToString() {
        static std::string resp;
        switch (MRSP) {
            case ORBOTIX_RSP_CODE_OK:
                resp.assign("Command Succeeded");
                break;
            case ORBOTIX_RSP_CODE_EGEN:
                resp.assign("Non-Specific/Unknown Error");
                break;
            case ORBOTIX_RSP_CODE_ECHKSUM:
                resp.assign("Received Checksum Failed");
                break;
            case ORBOTIX_RSP_CODE_EFRAG:
                resp.assign("Received Command Fragment");
                break;
            case ORBOTIX_RSP_CODE_EBAD_CMD:
                resp.assign("Unknown Command ID");
                break;
            case ORBOTIX_RSP_CODE_EUNSUPP:
                resp.assign("Command is currently Unsupported");
                break;
            case ORBOTIX_RSP_CODE_EBAD_MSG:
                resp.assign("Bad Message Format");
                break;
            case ORBOTIX_RSP_CODE_EPARAM:
                resp.assign("Parameter Value is invalid");
                break;
            case ORBOTIX_RSP_CODE_EEXEC:
                resp.assign("Failed to Execute Command");
                break;
            case ORBOTIX_RSP_CODE_EBAD_DID:
                resp.assign("Unknown Device ID");
                break;
            case ORBOTIX_RSP_CODE_MEM_BUSY:
                resp.assign("RAM Access Failed -- Ram Busy");
                break;
            case ORBOTIX_RSP_CODE_BAD_PASSWORD:
                resp.assign("Password is Incorrect");
                break;
            case ORBOTIX_RSP_CODE_POWER_NOGOOD:
                resp.assign("Voltage Too Low for Re-Flash Operation.");
                break;
            case ORBOTIX_RSP_CODE_PAGE_ILLEGAL:
                resp.assign("Illegal Page Number");
                break;
            case ORBOTIX_RSP_CODE_FLASH_FAIL:
                resp.assign("Page did not Reprogram Correctly");
                break;
            case ORBOTIX_RSP_CODE_MA_CORRUPT:
                resp.assign("Main Application is Corrupt");
                break;
            case ORBOTIX_RSP_CODE_MSG_TIMEOUT:
                resp.assign("Messaging State Machine Timed Out");
                break;
        }
        return (resp);
    }
};

/**
 * This class specializes a request to a Sphero Device.
 */
class SpheroClientCommand : public SpheroMessage {
    unsigned char SOP1; // start-of-packet 1 (always set to 0xFF)
    unsigned char SOP2; // start-of-packet 2 (11RA where, R = Reset Timeout & A = Answer)
    unsigned char DID;  // device id
    unsigned char CID;  // command id
    unsigned char SEQ;  // Sequence #
    unsigned char DLEN; // Data segment length
    unsigned char DATA[256]; // Data segment
    unsigned char chksum;

public:
    SpheroClientCommand()
        : SOP1(0xFF),
          SOP2(0xFE),
          DID(0x00),
          CID(0x00),
          SEQ(0x00),
          DLEN(0x01),
          chksum(0x00) {
        memset((void*)&DATA, 0, sizeof(DATA));
    }

    SpheroClientCommand(bool resetTimeout, bool answer)
        : SOP1(0xFF),
          SOP2(0xFC),
          DID(0x00),
          CID(0x00),
          SEQ(0x00),
          DLEN(0x00),
          chksum(0x00) {
        if (resetTimeout) SOP2 |= 0x2;
        if (answer) SOP2 |= 0x1;
        memset((void*)&DATA, 0, sizeof(DATA));
    }
    template <std::size_t SIZE>
    SpheroClientCommand(std::array<unsigned char, SIZE> & init) :
        SpheroMessage(init) {
    }

    unsigned char startOfPacket() {
        return SOP2;
    }
    unsigned char deviceId() {
        return DID;
    }
    unsigned char commandId() {
        return CID;
    }
    unsigned char sequenceNum() {
        return SEQ;
    }
    unsigned char dataLength() {
        return DLEN;
    }
    unsigned char* msgData() {
        return (DATA);
    }
    unsigned char calculateChecksum() {
        /*
         Here is an example of computing a checksum to transmit a Ping packet.
         The bytes for the packet (with a sequence number of 52h) are:
         SOP1 | SOP2 | DID | CID | SEQ | DLEN | DATA | CHK
         FFh  | FFh  | 00h | 01h | 52h | 01h  |  00h | <chk>
                      ------------------------------
         The checksum equals the sum of the underlined bytes (54h) modulo 256 (still 54h) and then bit inverted (ABh).
         NOTE: Since DLEN starts at 0 (because it doesn't yet consider chksum), add one to the sum.
         */
        unsigned int sum = DID + CID + SEQ + DLEN + 1;
        for (size_t i = 0; i < DLEN; ++i) {
            sum += DATA[i];
        }
        unsigned int mod = sum % 256;
        mod = ~mod;
        chksum = (unsigned char)(mod);
        return chksum;
    }

    void setDeviceId(unsigned char did) {
        DID = did;
    }
    void setCommandId(unsigned char cid) {
        CID = cid;
    }
    void setSeqNum(unsigned char seq) {
        SEQ = seq;
    }
    void setData(unsigned char *_data, size_t data_len) {
        if (data_len > 255)
            throw std::out_of_range("Length of data must be < 256 bytes");

        memcpy_s((void*)DATA, sizeof(DATA), (void*)_data, data_len);
        DLEN = (unsigned char)(data_len);
    }

    void assemble() {
        unsigned char dlen = DLEN + ((unsigned char)1);
        this->push_back(SOP1);
        this->push_back(SOP2);
        this->push_back(DID);
        this->push_back(CID);
        this->push_back(SEQ);
        this->push_back(dlen);
        for (size_t i = 0; i < DLEN; ++i)
            this->push_back(DATA[i]);
        this->push_back(calculateChecksum());
    }

    virtual SpheroPacket::size_type length() const {
        return 7 + DLEN;
    }
};

class SpheroAsyncClientCommand : public SpheroClientCommand {
public:
    SpheroAsyncClientCommand() :
        SpheroClientCommand(true, false) {
    }
    SpheroAsyncClientCommand(bool resetTimeout) :
        SpheroClientCommand(resetTimeout, false) {
    }
    template <std::size_t SIZE>
    SpheroAsyncClientCommand(std::array<unsigned char, SIZE> & init) :
        SpheroClientCommand(init) {
    }
};
