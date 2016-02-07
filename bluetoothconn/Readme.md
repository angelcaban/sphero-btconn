Sphero Bluetooth Communications Library
====================================================

A native C++ library that uses Boost's ASIO to initiate an RFCOMM
connection to a known Bluetooth Device Address.

This library was created as a statically-linked backbone to
another library, specially created to connect and communicate
with Sphero Robots.


Requirements
------------

- Visual Studio (Tested with VS2015)
- Boost Library (Tested with v1.60)
  In CMake, define `BOOST_ROOT`
- CMake v2.6

Known Issues
------------

None so far

Installation
------------

Use the CMake Utility to specify an installation prefix.

Raw Bluetooth Usage
-------------------

Firstly, find the bluetooth address for the device that you want to use:
    #include <btconn.h>
    
    SOCKADDR_BTH bluetoothAddress;
    std::vector<BluetoothInqItem> btItems;

    BluetoothProto::bluetoothInquire(btItems);
    for (auto & btItem : btItems) {
        if (btItem.name == "my device") {
            bluetoothAddress = btItem.address;
        }
    }

Then, use the address to initialize the connection:
    bt::BtConnection<message_type> connection;
    try {
        connection.setEndpoint(bluetoothAddress);
        connection.connect();
    } catch (std::exception & exc) {
        std::cerr << "Unable to connect - " << exc.what() << std::endl;
    }

Now, requests can be performed and responses read:
    std::shared_ptr<message_type> request = message_type::assemble();
    connection.send(request);

    // . . .

    auto responsePtr = connection.read();
    if (responsePtr) {
        return responsePtr->parse();
    }


Sphero Bluetooth Usage
----------------------

First, find the correct bluetooth address as described previously.

Secondly, instantiate a new SpheroHandler object with this address:
    try {
        SpheroHandler robot(bluetoothAddress);
    } catch (NotConnectedException & noConn) {
        std::cerr << "Unable to connect - " << noConn.what() << std::endl;
        exit(-1);
    }

Now, commands can be sent to the device:
    robot.sendCommand(PingCommand());

    auto responsePtr = robot.readResponse();
    if (responsePtr) {
        // Message Response of 0x00 signifies success
        if (0U != responsePtr->messageResponse()) {
            std::cerr << "Unable to ping - " <<
                responsePtr->messageReponseToString() << std::endl;
            exit(-1);
        }

        robot.sendCommand(GetBTNameCommand());
        responsePtr = robot.readResponse();
        if (responsePtr) {
            if (0U != responsePtr->messageResponse()) {
                std::cerr << "Unable to get name - " <<
                    responsePtr->messageResponseToString() << std::endl;
                exit(-1);
            }

            std::string asciiName(
                (char *) (responsePtr->msgData()),
                (char *) (responsePtr->msgData() + 16));
            cout << "Connected to " << asciiName << std::endl;
        }
    }

-- 
Angel Caban <acaban at mail dot angelcaban dot net>

