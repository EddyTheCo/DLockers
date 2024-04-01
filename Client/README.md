# DLockers Client

This application can be seen as a decentralized client that allows you to book a locker  by paying with crypto.
To use the application one needs to set the address of the node to connect.
The Proof of Work has to be performed by the node. 
This application is meant to be used on the testnet.
If using the mainnet **you are the ONLY responsible for the eventual loss of your funds**.

One can download the client application from the releases of this repo or use the [web app](https://eddytheco.github.io/DLockers/Client/)

## How to use it

The client shows the different [Dlockers Server](https://github.com/EddyTheCo/DLockers/tree/main/Server) available in a map.
One can select the server and book a slot of time on the server.
To book, the client must have funds, one can ask for funds [here](https://faucet.testnet.shimmer.network/)
by entering the Client account.

If the book and payment are accepted by the server, the server will send the client an NFT signed by the server.
By presenting this NFT to the server you can open the locker.
To do this, this application sends the NFT to a server address but with an expiration time in the past, so you continue owning the NFT
and can reuse it as many times as you want.


## Getting the Client app

### From source code
```
git clone https://github.com/EddyTheCo/DLockers.git 

mkdir build
cd build
qt-cmake -G Ninja -DCMAKE_INSTALL_PREFIX=installDir -DCMAKE_BUILD_TYPE=Release -DQTDEPLOY=ON -DUSE_THREADS=ON -DBUILD_SERVER=OFF ../Dlockers

cmake --build . 

cmake --install . 
```
where `installDir` is the installation path, `QTDEPLOY` install Qt dependencies(To be used only if compiling the desktop app). 
The use of multithreading is controlled by the `USE_THREADS` variable.
The `-DBUILD_SERVER=OFF`  has to be set if compiling for Android, because the server is not meant to run on Android.

The application major dependencies are the  [Qt](https://doc.qt.io/), [OpenCV](https://github.com/opencv/opencv) and [openssl](https://github.com/openssl/openssl) libraries.

For more information on how to compile for the different platforms refer to the GitHub workflows of the repo. 
### From GitHub releases
Download the releases from this repo.  

