# winnet-samples
A PoC chat application that runs over SPX/IPX network using Winsock2 API

### How to run
* Run `spxserv.exe` on a server machine. It will listen for incoming
connection on all available interfaces. The list of the interfaces
will be displayed on startup in case your sustem supports it (Windows NT)
* Run `spxclient.exe -n address` on client machines, where address is
the appropriate SPX address printed by the server. Example SPX address
looks like this: `1234ABCD.123456ABCDEF`
* The server works on endpoint `7171`, if you face a conflict
during server startup, make sure the endpoint is not used by another app

### Requirements

#### Build requirements

* Visual Studio 2005 to build executables supported by Windows 9x
* Higher version of visual studio can be used to build executables
without support of Windows 9x


#### Execution requirements
* Windows system with support of SPX/IPX protocols (Windows XP, Windows 2003 or lower)
* Winsock version 2.2 (Windows 95 OSR, Windows 98, Windows NT 4.0 or higher)

#### Tested on
* Windows XP
* Windows 98 SE

### Observations

* When calling recv in blocking mode, the operation would block
until socket is closed on the other side, or own buffer is full.
* If socket has been closed on the other side, recv can be called
many times without any error, each time 0 would be returned
* Create SPX/IPX datagram socket: socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX)
* Create SPX/IPX stream socket: socket(AF_IPX, SOCK_STREAM, NSPROTO_SPX)
