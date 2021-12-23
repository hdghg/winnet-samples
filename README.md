# winnet-samples
Sample code that uses windows network

### Observations:

* When calling recv in blocking mode, the operation would block
until socket is closed on the other side, or own buffer is full.
* If socket has been closed on the other side, recv can be called
many times without any error, each time 0 would be returned
