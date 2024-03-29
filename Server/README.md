# DLockers  Server


This application can be seen as a decentralized server that allows you to book a locker by paying with crypto.
The Proof of Work has to be performed by the node.
This application is meant to be used on the testnet.
If using the mainnet **you are the ONLY responsible for the eventual loss of your funds**.

## How to use it

To book a box one can use the [client](https://eddytheco.github.io/DLockers/Client).
The communication between server and client relies on creating outputs on the ledger.
Because of that, the server needs an initial amount of funds to be able to publish its state on the ledger. 
These initial funds are always owned by the server.


The server looks for new bookings sent by the clients and checks the validity of these bookings.
A booking is valid if it has an allowed  start and finish time and the price paid for the booking is correct.
If everything is fine the server collects the money from the client, updates its internal state, and sends an NFT to the client.


The server allows you to open the locker if you own an NFT signed by the server.
The signed NFT has immutable metadata that references certain time intervals of the bookings the client has paid.

## Platforms

The server is configured to run on a raspberry-pi, with a 4g and GPS module by using the [evt yocto Layer](https://github.com/EddyTheCo/meta-evt)
The server will set the pin `17`(this can be configured) up when a valid NFT is received.
The latter will open the electrical locker.
If using the GPS, the geographical position of the server will be public. 
The GPS is configured to comunicate using the serial port `/dev/ttyUSB1`.
One can also set up a [mock-up Server](https://eddytheco.github.io/DLockers/MockupServer) for testing just by using the browser.
The releases from this repo are also a mock-up Server.


## Getting the Server app

### From source code

```
git clone https://github.com/EddyTheCo/DLockers.git 

mkdir build
cd build
qt-cmake -G Ninja -DCMAKE_INSTALL_PREFIX=installDir -DCMAKE_BUILD_TYPE=Release -DQTDEPLOY=ON -DVAULT_PASS="EstervDlockers" -DRPI_SERIAL_PN="serial:/dev/ttyUSB1" -DRPI_SERVER_GPIO=17 -DRPI_SERVER=ON -DBUILD_SHARED_LIBS=ON  -DBUILD_CLIENT=OFF ../Dlockers

cmake --build . 

cmake --install . 
```
where `installDir` is the installation path, `QTDEPLOY` install Qt dependencies(To be used only if compiling the desktop app). 
The `-DBUILD_CLIENT=OFF` does not configure the Client app.
The `RPI_SERVER` variable controls configuring a real Server(ON) or a MockupServer(OFF).
The Server App creates a vault to store the random seed when is first created.
The `VAULT_PASS` variable sets the value of the password, by default `EstervDlockers` is used.
The serial port for GPS is set by the `RPI_SERIAL_PN`, by default `serial:/dev/ttyUSB1`  is used.
The pin `17` is used by default to open the locker, but this can be configured by the  `RPI_SERIAL_PN` variable.

The application major dependencies are the  [Qt](https://doc.qt.io/) and [openssl](https://github.com/openssl/openssl) libraries.

For more information on how to compile for the different platforms refer to the GitHub workflows of the repo and the [evt yocto Layer](https://github.com/EddyTheCo/meta-evt/releases/tag/DlockersImageV0.0.1).
 
### From GitHub releases

Download the releases from this repo. 
The releases of the Server app from this repo are mock-up servers.
### Yocto image

One can get the image I used for a raspberry pi3 in the [showcase video](https://www.youtube.com/watch?v=JoLe93Gzwho).
The image was created using this tag of the [evt yocto Layer](https://github.com/EddyTheCo/meta-evt/releases/tag/DlockersImageV0.0.1).
The image can be download from this [link](https://drive.google.com/file/d/1xcjNCxDmNiy2UezKV6mg3VdZ25qZXl_1/view?usp=drive_link).
Please take care of checking the different licenses of the components used on the image.



