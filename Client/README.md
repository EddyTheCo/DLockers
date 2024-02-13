# DLockers Client

This application can be seen as a decentralized client that allows you to book a locker  by paying with crypto.
In order to use the application one needs to set the address of the node to connect.
The Proof of Work has to be performed by the node. 
This application is meant to be used on the testnet.
If using the mainnet **you are the ONLY responsible for the eventual loss of your funds**.

One can download the client application from the releases of this repo or use the [web app](https://eddytheco.github.io/Dlockers/Client/)

## How to use it

The client shows the different [Dlockers Server](https://github.com/EddyTheCo/DLockers/tree/main/Server) available in a map.
One can select the server and book a slot of time on the server.
In order to book, the client must have funds, one can ask for funds [here](https://faucet.testnet.shimmer.network/)
by entering the Client account.

If the book and payment is accepted by the server, the server will send the client a NFT signed by the server.
By presenting this NFT to the server you can open the locker.
To do this, this application send the NFT to a server address but with expiration time in the past, so you continue owning the NFT
and can reuse it as any times you want.


