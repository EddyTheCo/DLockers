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
The server will set the pin 17(this can be configured) up when a valid NFT is received.
The latter will open the electrical locker.
If using the GPS, the geographical position of the server will be public. 

One can also set up a [mock-up Server](https://eddytheco.github.io/DLockers/MockupServer) for testing just by using the browser.
The releases from this repo are also a mock-up Server.
