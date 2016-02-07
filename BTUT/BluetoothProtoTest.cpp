#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace bt {
using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace std;

typedef std::shared_ptr<string> MockMsgPtr;

template<>
template<> inline
MockMsgPtr BtConnection<string>::read<string>() {
    MsgPtr ret;
    return ret;
}

template<> inline
void BtConnection<string>::doSend() {
}

template<> inline
void BtConnection<string>::connect() {
}

} // namespace bt

namespace BTUT
{
    typedef bt::BtConnection<std::string> MockBt;

    TEST_CLASS(BluetoothProtoTest)
    {
        std::vector<BluetoothInqItem> btItems;
    public:

        BluetoothProtoTest() {
            BluetoothProto::bluetoothInquire(btItems);
        }

        TEST_METHOD(testBluetoothInquire)
        {
            Assert::IsTrue(btItems.size() > 0);
        }

        TEST_METHOD(testBluetoothConnect)
        {
            try {
                MockBt conn;
                conn.setEndpoint(btItems.at(0).address);
                conn.connect();
                Assert::IsTrue(true);
            } catch (std::exception & exc) {
                wchar_t excMsg[4092] = { 0 };
                MultiByteToWideChar(CP_ACP, 0, exc.what(),
                    (int) strlen(exc.what()), excMsg, 4092);
                Assert::Fail(excMsg);
            }
        }
    };
}
