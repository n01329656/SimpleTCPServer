# SimpleTCPServer
This is a simple multithread TCP-based server that displays messages. 
It is written with boost::asio network library.
It uses a simple synchronous write() method to retrieve client messages. 
The second argument supplied to the Server constructor specifies the port number and the last argument tells the number of active sockets(sockets that are used to communicate with clients).
Inside the main program, you can see a possible usage of the Server class. 
Also, to terminate the server press CTRL + BREAK, it will terminate the program and return EXIT_SUCCESS status. 
