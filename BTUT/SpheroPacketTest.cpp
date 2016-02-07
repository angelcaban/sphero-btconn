#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BTUT {
    TEST_CLASS(SpheroPacketTest) {
    
    public:
        SpheroPacketTest() {
        }

        TEST_METHOD(testResponsePacket) {
            SpheroServerResponse packet(
            {
                0xFF,
                0xFF,
                0x00,
                0x01,
                0x01,
                0xFF
            }); // Valid plain old response...

            packet.parseFromInternalData();

            Assert::AreEqual((unsigned char) 0xFF, packet.startOfPacket());
            Assert::AreEqual((unsigned char) 0x00, packet.messageResponse());
            Assert::AreEqual((unsigned char) 0x01, packet.sequenceNum());
            Assert::AreEqual((unsigned char) 0x01, packet.dataLength());
            Assert::AreEqual((unsigned char) 0xFF, packet.msgData()[0]);
        }

        TEST_METHOD(testErrorResponse) {
            SpheroServerResponse packet(
            {
                0xFF,
                0xFF,
                0x09,
                0x01,
                0x01,
                0xFF
            }); // Valid plain old response...

            packet.parseFromInternalData();

            Assert::AreEqual("Unknown Device ID", packet.messageResponseToString().c_str());
        }

        TEST_METHOD(testNumericalDataUShort) {
            unsigned short expected_1 = 10000;
            unsigned short expected_2 = 50000;
            unsigned short expected_3 = 0x7FFF;
            unsigned short expected_4 = 0xFFFF;
            unsigned short expected_5 = 0x0;
            unsigned short expected_6 = 0xF000;

            SpheroServerResponse packet1(
            {
                0xFF,
                0xFF,
                0x00,
                0x01,
                0x02,
                0x27,
                0x10
            }); // Only two bytes in msg packet are 10000
            SpheroServerResponse packet2(
            {
                0xFF,
                0xFF,
                0x00,
                0x01,
                0x9,
                0x01,
                0x02,
                0x03,
                0x04,
                0x05,
                0x06,
                0x07,
                0xC3,
                0x50,
            }); // Last two bytes of msg packet are 50000
            SpheroServerResponse packet3(
            {
                0xFF,
                0xFF,
                0x00,
                0x01,
                0x4,
                0x01,
                0x7F,
                0xFF,
                0x04
            }); // Middle bytes of msg packet are 32767
            SpheroServerResponse packet4(
            {
                0xFF,
                0xFF,
                0x00,
                0x01,
                0x2,
                0xFF,
                0xFF
            }); // Only bytes of msg packet are 65535
            SpheroServerResponse packet5(
            {
                0xFF,
                0xFF,
                0x00,
                0x64,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }); // A bunch of zeros with a Waldo 1
            SpheroServerResponse packet6(
            {
                0xFF,
                0xFF,
                0x00,
                0x01,
                0x1,
                0xF0
            }); // One byte 0xF0

            packet1.parseFromInternalData();
            packet2.parseFromInternalData();
            packet3.parseFromInternalData();
            packet4.parseFromInternalData();
            packet5.parseFromInternalData();
            packet6.parseFromInternalData();

            unsigned short actual_1 = packet1.dataToNumerical<unsigned short>();
            unsigned short actual_2 = packet2.dataToNumerical<unsigned short>(7);
            unsigned short actual_3 = packet3.dataToNumerical<unsigned short>(1);
            unsigned short actual_4 = packet4.dataToNumerical<unsigned short>();
            unsigned short actual_5 = packet5.dataToNumerical<unsigned short>();
            unsigned short actual_6 = packet6.dataToNumerical<unsigned short>();

            Assert::AreEqual((unsigned) expected_1, (unsigned) actual_1);
            Assert::AreEqual((unsigned) expected_2, (unsigned) actual_2);
            Assert::AreEqual((unsigned) expected_3, (unsigned) actual_3);
            Assert::AreEqual((unsigned) expected_4, (unsigned) actual_4);
            Assert::AreEqual((unsigned) expected_5, (unsigned) actual_5);
            Assert::AreEqual((unsigned) expected_6, (unsigned) actual_6);
        }
    };
}
