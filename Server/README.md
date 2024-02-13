# DLockers  Server


This application can be seen as a decentralized server that allows you to book a locker by paying with crypto.
The Proof of Work has to be performed by the node.
This application is meant to be used on the testnet.
If using the mainnet **you are the ONLY responsible for the eventual loss of your funds**.

## How to use it

In order to book a box one can use the [client](https://eddytheco.github.io/DLockers/Client).
The communication between server and client relies on creating outputs on the ledger.
Because of that, the server needs an initial amount of funds to be able to publish its state on the ledger. 
These initial funds are always own by the server.


The server looks for new bookings sent by the clients and checks the validity of these bookings.
A booking is valid if it has an allowed  start and finish time and the price payed for the booking is correct.
If everything is fine the server collect the money from the client, update its internal state and send a NFT to the client.


The server allows you to open the locker if you own a NFT signed by the server.
The signed NFT has immutable metadata that reference certain time interval of the bookings the client has paid.

The server is configured to run on a raspberry-pi, with a 4g and GPS module.
The server will set the pin 17(this can be configured) up when a valid NFT is received.
The later will open the electrical locker.

One using the GPS, the geographical position of the server will be public. 
One can also setup a [mock-up Server](https://eddytheco.github.io/DLockers/MockupServer) for testing.
