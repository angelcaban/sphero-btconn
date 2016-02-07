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

#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define INQ_MAX_RETRIES 10
#define INQ_RETRY_SLEEP 5

namespace BTUT
{
    TEST_CLASS(SpheroCommandTest)
    {
        std::vector<BluetoothInqItem> btItems;
        SpheroHandler robot;

        SOCKADDR_BTH* getSpheroBtAddr() {
            for (int i = 0; i < INQ_MAX_RETRIES; ++i) {
                BluetoothProto::bluetoothInquire(btItems);
                for (auto & item : btItems) {
                    if (item.name.find("Sphero") != std::string::npos) {
                        BtLogger::log() << std::endl << "Found Sphero Bluetooth Address..." << std::endl;
                        return &(item.address);
                    }
                }

                BtLogger::log() << "Sphero not found. Sleeping an trying again...." << std::endl;
                Sleep(INQ_RETRY_SLEEP * 1000);
            }

            return (NULL);
        }

        LPWSTR allocateWideMessageString(std::string const& msg) {
            size_t respMbLen = msg.length();
            LPWSTR wMsgResp = (LPWSTR)calloc(respMbLen * 2 + 1, sizeof(wchar_t));
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
                msg.c_str(), -1, wMsgResp, msg.length());
            return wMsgResp;
        }

    public:

        SpheroCommandTest() : robot(getSpheroBtAddr()) {
        }
        ~SpheroCommandTest() {
            Sleep(5);
        }

        TEST_METHOD(testPingCommandNotConnected) {
            Assert::ExpectException<NotConnectedException>([]() {
                SpheroHandler no_sphero(NULL);
                no_sphero.sendCommand(PingCommand());
            });
        }

        TEST_METHOD(int_testPingCommand) {
            robot.sendCommand(PingCommand());

            SpheroResponsePtr response = robot.readResponse();
            Assert::IsTrue(response.get() != nullptr);
            
            size_t respMbLen = response->messageResponseToString().length();
            std::shared_ptr<wchar_t> wMsgResp(
                allocateWideMessageString(response->messageResponseToString()));
            Assert::AreEqual((unsigned char) 0x00, response->messageResponse(), wMsgResp.get());

            Assert::AreEqual((unsigned char) 0x01, response->dataLength());
        }

        TEST_METHOD(int_testGetVersioningCommand) {
            robot.sendCommand(VersionCommand());

            SpheroResponsePtr response = robot.readResponse();
            Assert::IsTrue(response.get() != nullptr);

            size_t respMbLen = response->messageResponseToString().length();
            std::shared_ptr<wchar_t> wMsgResp(
                allocateWideMessageString(response->messageResponseToString()));
            Assert::AreEqual((unsigned char)0x00, response->messageResponse(), wMsgResp.get());

            Assert::AreEqual((unsigned char)0x09, response->dataLength());

            BtLogger::log() << "    Record #: " << std::hex <<
                (unsigned) response->msgData()[0] << std::endl;
            BtLogger::log() << "     Model #: " << std::hex <<
                (unsigned) response->msgData()[1] << std::endl;
            BtLogger::log() << "        HW #: " << std::hex <<
                (unsigned) response->msgData()[2] << std::endl;
            BtLogger::log() << "       App #: " << std::hex <<
                (unsigned) response->msgData()[3] << "." <<
                (unsigned) response->msgData()[4] << std::endl;
            BtLogger::log() << "  BootLoader: " << std::hex <<
                (unsigned) response->msgData()[5] << std::endl;
            BtLogger::log() << "   Orb Basic: " << std::hex <<
                (unsigned) response->msgData()[6] << std::endl;
            BtLogger::log() << "       Macro: " << std::hex <<
                (unsigned) response->msgData()[7] << std::endl;
        }

        TEST_METHOD(int_testGetBluetoothInfoCommand) {
            robot.sendCommand(GetBTNameCommand());

            SpheroResponsePtr response = robot.readResponse();
            Assert::IsTrue(response.get() != nullptr);

            size_t respMbLen = response->messageResponseToString().length();
            std::shared_ptr<wchar_t> wMsgResp(
                allocateWideMessageString(response->messageResponseToString()));
            Assert::AreEqual((unsigned char) 0x00, response->messageResponse(), wMsgResp.get());

            Assert::AreEqual((unsigned char) 0x21, response->dataLength());

            std::string asciiName(
                (char *) (response->msgData()),
                (char *) (response->msgData() + 16));
            std::string btAddress(
                (char *) (response->msgData() + 17),
                (char *) (response->msgData() + 29));

            BtLogger::log() << "     ASCII Name: " << asciiName << std::endl;
            BtLogger::log() << " Bluetooth Addr: " << btAddress << std::endl;
            BtLogger::log() << "          Color: [" <<
                " 0x" << std::hex << (unsigned) response->msgData()[30] <<
                " 0x" << std::hex << (unsigned) response->msgData()[31] <<
                " 0x" << std::hex << (unsigned) response->msgData()[32] <<
                " ] " << std::endl;
        }

        TEST_METHOD(int_testGetPowerStateCommand) {
            robot.sendCommand(GetPowerStateCommand());

            SpheroResponsePtr response = robot.readResponse();
            Assert::IsTrue(response.get() != nullptr);

            size_t respMbLen = response->messageResponseToString().length();
            std::shared_ptr<wchar_t> wMsgResp(
                allocateWideMessageString(response->messageResponseToString()));
            Assert::AreEqual((unsigned char) 0x00, response->messageResponse(), wMsgResp.get());

            Assert::AreEqual((unsigned char) 0x9, response->dataLength());

            std::string powerState("123245678901232457890");
            switch (response->msgData()[1]) {
                case 0x01:
                    powerState.assign("Battery Charging");
                    break;
                case 0x02:
                    powerState.assign("Battery OK");
                    break;
                case 0x03:
                    powerState.assign("Battery LOW");
                    break;
                case 0x04:
                    powerState.assign("Battery CRITICAL");
                    break;
                default:
                    powerState.assign("Bad Response");
                    break;
            }

            float voltage = (float) (response->dataToNumerical<unsigned short>(2));
            voltage /= 100.f;

            BtLogger::log() << "  Record Ver: " << std::hex <<
                (unsigned) response->msgData()[0] << std::endl;
            BtLogger::log() << " Power State: " << powerState << std::endl;
            BtLogger::log() << "     Voltage: " << voltage << std::endl;
            BtLogger::log() << " Num Charges: " << std::hex <<
                (unsigned) response->dataToNumerical<unsigned short>(4) <<
                std::endl;
            BtLogger::log() << "Since Charge: " << std::hex <<
                (unsigned) response->dataToNumerical<unsigned short>(6) <<
                std::endl;
        }

        TEST_METHOD(int_testReadLocatorCommand) {
            robot.sendCommand(ReadLocatorCommand());
            SpheroResponsePtr response = robot.readResponse();
            Assert::IsTrue(response.get() != nullptr);

            size_t respMbLen = response->messageResponseToString().length();
            std::shared_ptr<wchar_t> wMsgResp(
                allocateWideMessageString(response->messageResponseToString()));
            Assert::AreEqual((unsigned char) 0x00, response->messageResponse(), wMsgResp.get());

            Assert::AreEqual((unsigned char) 0xB, response->dataLength());

            short xPos = response->dataToNumerical<short>();
            short yPos = response->dataToNumerical<short>(2);
            short xVel = response->dataToNumerical<short>(4);
            short yVel = response->dataToNumerical<short>(6);
            short speedOverGround = response->dataToNumerical<short>(8);

            BtLogger::log() << "    X-Position: " << xPos << std::endl;
            BtLogger::log() << "    Y-Position: " << yPos << std::endl;
            BtLogger::log() << "    X-Velocity: " << xVel << std::endl;
            BtLogger::log() << "    Y-Velocity: " << yVel << std::endl;
            BtLogger::log() << "  Ground Speed: " << speedOverGround << std::endl;
        }
    };
}