/***************************************************************************
 *   Copyright (C) 2005 by Brian Smith                                     *
 *   root                                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>

#include <bitset>

#include "XSocketServer.h"
#include "XSocketClient.h"

#define MAX 99999
#define PORT 3200
#define RAND_LENGTH 65

using namespace std;

enum roles { SERVER, CLIENT }; 

struct connData {
	char *destination;
	char *bind_to;
	std::size_t length;
	std::size_t rand_length;
	std::size_t port;
	enum roles role;
};

template <class TYPE>
void binout( TYPE* in, int len ) {

        cout << "\n~==={ Binary Output }===~" << endl;
        for (int i=0; i<len; i++) {
                cout << "~\t" << bitset<8>( in[ i ] ) << " = " << in[ i ] << "\t~" << endl;
        }
        cout << "~==={  End  Output  }===~\n" << endl;
}

int client_handler(XSocketServer sock, int conn) {
	int i, k;
	i = k = 0;
	char *e = new char[ MAX ];
	memset( e, '\0', MAX );
	
	connData testtype;
	testtype.destination = "Success";
	cout << "Recvd: " << endl;
	k = sock.recv( e, MAX, conn);
 	cout << "\nSending" << endl;
// 	sock.send( e, k, conn);

	for (i=0; i<20; i++) {
		*testtype.bind_to = *e;
		testtype.bind_to++;
		e++;
	}
 	testtype.bind_to -= 20;
	e -= 20;
	sock.set_debug( true );
	cout << '\t' << testtype.bind_to << endl;
	sock.send( testtype, sizeof( testtype ), conn);
	
	
	delete[] e;
	exit( 0 );
	return 0;
}

char charMe( int xx ) {
	if ( xx>20 && xx<128 )  return (char)xx;
	
	while ( xx<25 || xx>127 ) {
		if ( xx<20 ) {
			xx *= 3;
		} else {
			xx /= 3;
		}
	}
	
	return (char)xx;
}

const char* rand_string( int len ) {
	char *ret = new char[ len ];
	for (int i=0; i<len; i++) {
		*ret = charMe(rand());
		ret++;
	}
	ret -= len;
	return ret;
}

connData populate_struct( int argc, char *argv[] ) {
	int i = 0;
	connData swarm;
	swarm.destination = new char[ MAX ];
	swarm.bind_to = new char[ MAX ];
	
	for (i=0; i<argc; i++) {
		if (argv[ i ][0] == '-') {
			switch( argv[ i ][1] ) {
				case 'p': // port
					swarm.port = atoi(argv[ i+1 ]);
					break;
					
				case 'd': // destination
					swarm.destination = argv[ i+1 ];
					break;
				
				case 'r': // random data string length
					swarm.rand_length = atoi(argv[ i+1 ]);
					break;
			
				case 'b': // what address to bind the client to
					swarm.bind_to = argv[ i+1 ];
					break;
							
				case 's':
					swarm.role = SERVER;
					break;
					
				case 'c':
					swarm.role = CLIENT;
					break;
					
			}
		}
	}
	if (swarm.port<1 || swarm.port>65535)  swarm.port = PORT;
	if (strlen(swarm.destination) <= 2)  swarm.destination = "localhost";
	if (swarm.rand_length<1 || swarm.rand_length>MAX)  swarm.rand_length = RAND_LENGTH;
	return swarm;
}

void print_usage(int argc, char *argv[]) {
	cout << "Usage:\r\n\t" << argv[0] << " -s ; Run Server\r\n\t" << argv[0] << " -c ; Run Client Against the Server\r\n\r\n\t-p <port> ; Port Number" << endl
		<< "\t-b <address> ; Local Address to Bind to" << endl
		<< "\r\n---------------Client Specific Arguements----------------\r\n|\t-d <destination> ; IP or Resolvable Name\t|" << endl
		<< "|\t-r <length> ; Length of Random Data\t\t|" << endl
		<< "---------------------------------------------------------\r\n\r\nExamples:\r\n\t" << argv[0] << " -s -p 1984 " << endl
		<< '\t' << argv[0] << " -c -d my_server.example.com -p 1984 -r 55 " << endl;
	exit( EXIT_FAILURE );
}

int main(int argc, char *argv[]) {
	XSocketServer server;
	XSocketClient client;
	server.set_debug( false );
	client.set_debug( false );
	
	string data;
	connData swarm;
	connData test;
	
	if ( argc<2 || strlen(argv[1])>2 )  print_usage( argc, argv );

	swarm = populate_struct( argc, argv );
	
	switch( swarm.role ) {
		case 0:
			if (strlen( swarm.bind_to ) > 1) {
				server.init( swarm.bind_to, swarm.port );
			} else {
				server.init( swarm.port );
			}
			
			cout << "Server Running" << endl;
			server.non_blocking( &client_handler, &client_handler );
			break;
			
		case 1:
			if (strlen( swarm.bind_to ) > 1) {
				client.init( swarm.destination, swarm.port, swarm.bind_to );
			} else {
 				client.init( swarm.destination, swarm.port );
			}
			
			cout << "Client Connecting" << endl;
			for(;;) {
				data = rand_string( swarm.rand_length );
				cout << "Sending:\t" << data << endl;
 				//client.send( data.data(), data.size() );
				client.send(data.data(), data.size());
//  				test = 
				client.recv( &test, 100 );
				cout << "!!!!!!!!!\nDesitination:\t" << test.destination << "\nBind_to:\t" << test.bind_to << "\n!!!!!!!!!" << endl;
				return EXIT_SUCCESS;
			}
			break;
		default: 
			print_usage( argc, argv );
			break;
	}
	delete[] swarm.destination;
	delete[] swarm.bind_to;
	return EXIT_SUCCESS;
}
