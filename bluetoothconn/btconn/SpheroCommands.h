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

enum DeviceId {
    DID_CORE = 0x00,
    DID_SPHERO = 0x02
};

enum CoreCommandId {
    CMD_PING = 0x01,
    CMD_VERSION = 0x02,
    CMD_SET_BT_NAME = 0x10,
    CMD_GET_BT_NAME = 0x11,
    CMD_SET_AUTO_RECONNECT = 0x12,
    CMD_GET_AUTO_RECONNECT = 0x13,
    CMD_GET_PWR_STATE = 0x20,
    CMD_SET_PWR_NOTIFY = 0x21,
    CMD_SLEEP = 0x22,
    SET_INACTIVE_TIMER = 0x25,
    CMD_GOTO_BL = 0x30,
    CMD_RUN_L1_DIAGS = 0x40,
    CMD_RUN_L2_DIAGS = 0x41,
};

enum SpheroCommandId {
    CMD_SET_CAL = 0x01,
    CMD_SET_STABILIZ = 0x02,
    CMD_SET_ROTATION_RATE = 0x03,
    CMD_REENABLE_DEMO = 0x06,
    CMD_SELF_LEVEL = 0x09,
    CMD_SET_DATA_STREAMING = 0x11,
    CMD_SET_COLLISION_DET = 0x12,
    CMD_LOCATOR = 0x13,
    CMD_SET_ACCELERO = 0x14,
    CMD_READ_LOCATOR = 0x15,
    CMD_SET_RGB_LED = 0x20,
    CMD_SET_BACK_LED = 0x21,
    CMD_GET_RGB_LED = 0x22,
    CMD_ROLL = 0x30,
    CMD_BOOST = 0x31,
    CMD_MOVE = 0x32,
    CMD_SET_RAW_MOTORS = 0x33,
    CMD_SET_MOTION_TO = 0x34,
    CMD_SET_OPTIONS_FLAG = 0x35,
    CMD_GET_OPTIONS_FLAG = 0x36,
    CMD_SET_TEMP_OPTIONS_FLAG = 0x37,
    CMD_GET_TEMP_OPTIONS_FLAG = 0x38,
    CMD_RUN_MACRO = 0x50,
    CMD_SAVE_TEMP_MACRO = 0x51,
    CMD_SAVE_MACRO = 0x52,
    CMD_INIT_MACRO_EXECUTIVE = 0x54,
    CMD_ABORT_MACRO = 0x55,
    CMD_MACRO_STATUS = 0x56,
    CMD_SET_MACRO_PARAM = 0x57,
    CMD_APPEND_TEMP_MACRO_CHUNK = 0x58,
    CMD_ERASE_ORBBAS = 0x60,
    CMD_APPEND_FRAG = 0x61,
    CMD_EXEC_ORBBAS = 0x62,
    CMD_ABORT_ORBBAS = 0x63,
    CMD_ANSWER_INPUT = 0x64,
};

enum MessageResponseCodes {
    ORBOTIX_RSP_CODE_OK = 0x00,         // Command succeeded
    ORBOTIX_RSP_CODE_EGEN = 0x01,       // General, non-specific error
    ORBOTIX_RSP_CODE_ECHKSUM = 0x02,    // Received checksum failure
    ORBOTIX_RSP_CODE_EFRAG = 0x03,      // Received command fragment
    ORBOTIX_RSP_CODE_EBAD_CMD = 0x04,   // Unknown command ID
    ORBOTIX_RSP_CODE_EUNSUPP = 0x05,    // Command currently unsupported
    ORBOTIX_RSP_CODE_EBAD_MSG = 0x06,   // Bad message format
    ORBOTIX_RSP_CODE_EPARAM = 0x07,     // Parameter value(s) invalid
    ORBOTIX_RSP_CODE_EEXEC = 0x08,      // Failed to execute command
    ORBOTIX_RSP_CODE_EBAD_DID = 0x09,   // Unknown Device ID
    ORBOTIX_RSP_CODE_MEM_BUSY = 0x0A,   // Generic RAM access needed but it is busy
    ORBOTIX_RSP_CODE_BAD_PASSWORD = 0x0B, // Supplied password incorrect
    ORBOTIX_RSP_CODE_POWER_NOGOOD = 0x31, // Voltage too low for reflash operation
    ORBOTIX_RSP_CODE_PAGE_ILLEGAL = 0x32, // Illegal page number provided
    ORBOTIX_RSP_CODE_FLASH_FAIL = 0x33,   // Page did not reprogram correctly
    ORBOTIX_RSP_CODE_MA_CORRUPT = 0x34,   // Main Application corrupt
    ORBOTIX_RSP_CODE_MSG_TIMEOUT = 0x35,  // Msg state machine timed out
};

class SpheroMessage;

template<unsigned char DID, unsigned char CID>
class SpheroCommand {
    std::string description;
    unsigned char did_;
    unsigned char cid_;

public:
    SpheroCommand() :
        did_(DID),
        cid_(CID) {
    }

    unsigned char did() {
        return did_;
    }
    unsigned char cid() {
        return cid_;
    }

    template <class T = SpheroMessage>
    std::shared_ptr<T> generateCommandMessage(unsigned char seq) {
        std::shared_ptr<T> ret(new T(true, true));
        ret->setDeviceId(did());
        ret->setCommandId(cid());
        ret->setSeqNum(seq);
        return ret;
    }
};


typedef SpheroCommand<DID_CORE, CMD_PING> PingCommand;
typedef SpheroCommand<DID_CORE, CMD_VERSION> VersionCommand;
typedef SpheroCommand<DID_CORE, CMD_SET_BT_NAME> SetBTNameCommand;
typedef SpheroCommand<DID_CORE, CMD_GET_BT_NAME> GetBTNameCommand;
typedef SpheroCommand<DID_CORE, CMD_SET_AUTO_RECONNECT> SetAutoReconnectCommand;
typedef SpheroCommand<DID_CORE, CMD_GET_AUTO_RECONNECT> GetAutoReconnectCommand;
typedef SpheroCommand<DID_CORE, CMD_GET_PWR_STATE> GetPowerStateCommand;
typedef SpheroCommand<DID_CORE, CMD_SET_PWR_NOTIFY> SetPowerNotifyCommand;
typedef SpheroCommand<DID_CORE, CMD_SLEEP> SleepCommand;
typedef SpheroCommand<DID_CORE, SET_INACTIVE_TIMER> SetInactiveTimerCommand;
typedef SpheroCommand<DID_CORE, CMD_GOTO_BL> GotoBLCommand;
typedef SpheroCommand<DID_CORE, CMD_RUN_L1_DIAGS> RunL1DiagnosticsCommand;
typedef SpheroCommand<DID_CORE, CMD_RUN_L2_DIAGS> RunL2DiagnosticsCommand;

typedef SpheroCommand<DID_SPHERO, CMD_SET_CAL> SetCalibrationCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_STABILIZ> SetStabilizeCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_ROTATION_RATE> SetRotationRateCommand;
typedef SpheroCommand<DID_SPHERO, CMD_REENABLE_DEMO> SetReenableDemoCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SELF_LEVEL> SetSelfLevelCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_DATA_STREAMING> SetDataStreamingCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_COLLISION_DET> SetCollisionDetectionCommand;
typedef SpheroCommand<DID_SPHERO, CMD_LOCATOR> SetLocatorCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_ACCELERO> SetAccelerometerCommand;
typedef SpheroCommand<DID_SPHERO, CMD_READ_LOCATOR> ReadLocatorCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_RGB_LED> SetRGBCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_BACK_LED> SetBackLedCommand;
typedef SpheroCommand<DID_SPHERO, CMD_GET_RGB_LED> GetRGBCommand;
typedef SpheroCommand<DID_SPHERO, CMD_ROLL> RollCommand;
typedef SpheroCommand<DID_SPHERO, CMD_BOOST> BoostCommand;
typedef SpheroCommand<DID_SPHERO, CMD_MOVE> MoveCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_RAW_MOTORS> SetRawMotorsCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_MOTION_TO> SetMotionToCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_OPTIONS_FLAG> SetOptionsFlagCommand;
typedef SpheroCommand<DID_SPHERO, CMD_GET_OPTIONS_FLAG> GetOptionsFlagCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_TEMP_OPTIONS_FLAG> SetTempOptionsFlagCommand;
typedef SpheroCommand<DID_SPHERO, CMD_GET_TEMP_OPTIONS_FLAG> GetTempOptionsFlagCommand;
typedef SpheroCommand<DID_SPHERO, CMD_RUN_MACRO> RunMacroCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SAVE_TEMP_MACRO> SaveTempMacroCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SAVE_MACRO> SaveMacroCommand;
typedef SpheroCommand<DID_SPHERO, CMD_INIT_MACRO_EXECUTIVE> InitMacroExecutiveCommand;
typedef SpheroCommand<DID_SPHERO, CMD_ABORT_MACRO> AbortMacroCommand;
typedef SpheroCommand<DID_SPHERO, CMD_MACRO_STATUS> MacroStatusCommand;
typedef SpheroCommand<DID_SPHERO, CMD_SET_MACRO_PARAM> SetMacroParamCommand;
typedef SpheroCommand<DID_SPHERO, CMD_APPEND_TEMP_MACRO_CHUNK> AppendTempMacroChunkCommand;
typedef SpheroCommand<DID_SPHERO, CMD_ERASE_ORBBAS> EraseOrbbasCommand;
typedef SpheroCommand<DID_SPHERO, CMD_APPEND_FRAG> AppendFragCommand;
typedef SpheroCommand<DID_SPHERO, CMD_EXEC_ORBBAS> ExecOrbbasCommand;
typedef SpheroCommand<DID_SPHERO, CMD_ABORT_ORBBAS> AbortOrbbasCommand;
typedef SpheroCommand<DID_SPHERO, CMD_ANSWER_INPUT> AnswerInputCommand;

