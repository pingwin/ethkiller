/**
 *   Author:	"Brian Smith" <pingwin@gmail.com>
 *   Website:	http://www.pingwin3k.net
 *   Licence:	GNU/GPL
 *
 *   This program is free software; you can redistribute it and/or modify 
 *   it under the terms of the GNU General Public License as published by 
 *   the Free Software Foundation; either version 2 of the License, or   
 *   (at your option) any later version.                                   
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *   GNU General Public License for more details.                          
 *                                                                         
 *   You should have received a copy of the GNU General Public License     
 *   along with this program; if not, write to the                         
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
**/

#ifndef _XSOCKET_H_
#include "XSocket.h"
#endif

#include <iostream>

#include <netdb.h>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>  //  These are for TCP/IP
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

using namespace std;


void XSocket :: set_debug( bool xx ) { this->debug = xx; }
void XSocket :: set_buffer_size() { this->BUFFER_SIZE = 2000; }

void XSocket :: set_protocol( sockaddr_in &sockAddr) { sockAddr.sin_family = AF_INET; }
void XSocket :: set_port ( sockaddr_in &sockAddr ) { sockAddr.sin_port = htons( this->port ); }
void XSocket :: reset_address( sockaddr_in &sockAddr ) { memset( &sockAddr, 0, sizeof( sockAddr) ); }
void XSocket :: set_destination( sockaddr_in &sockAddr ) { sockAddr.sin_addr.s_addr = inet_addr( this->dest_addr ); }

void XSocket :: set_device( sockaddr_in &sockAddr ) { sockAddr.sin_addr.s_addr = htonl( INADDR_ANY ); }

void XSocket :: set_device( sockaddr_in &sockAddr, char &ip_addr ) {
	this->hostname( &ip_addr, this->dest_addr );
	if (!this->err0r) {
		this->set_destination( sockAddr );
	} else {
		this->set_device( sockAddr );
		print(__FILE__, __LINE__, "Local Binding did not Work, Resorting to Binding on All Interfaces");
	}
}

template< typename T, std::size_t N >
std::size_t XSocket :: nElements( T const (&)[N] ) {
	return N;
}

void XSocket :: create_socket( int &xx ) {
	if ( (xx = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		this->error(__FILE__, __LINE__, "Socket() Failed");
	}
}

void XSocket :: set_buffer_size( size_t s ) {
	if ( s > 0 ) {
		this->BUFFER_SIZE = s;
	} else {
		this->BUFFER_SIZE = 2500;
	}
};

void XSocket :: bind_socket( int &xx, sockaddr_in &sock ) {
	if ( bind( xx, (struct sockaddr *) &sock, sizeof( sock ) ) < 0) {
		perror("bind() failed");
	}
}

int XSocket :: set_port( int xx ) {
	if ( this->validate_port( xx ) ) {
		this->port = xx;
		return true;
	}

	return false;
}	// end set_port()

bool XSocket :: validate_port( int port_num ) {  //  verify port number is between 1 and 65535 <valid port numbers>
	if ( !(port_num < 0) && !(port_num >= 65535) ) {
		this->port = port_num;
	} else {
		this->err0r = true;
	}
	return this->err0r;
}	 // end validate_port()

void XSocket :: hostname( char ip_addr[], char *(&dest) ) {
	struct hostent *he;  //  resolve name to ip
	if ((he = gethostbyname(ip_addr)) == NULL) {
		perror("gethostbyname()");
		this->err0r = true;
	}
	dest = inet_ntoa( *((struct in_addr *) he->h_addr) );
}	// end gethostname()

void XSocket :: Xclose() {
	if (this->servSock>0)
	if (shutdown(this->servSock, 2)) {
		perror("Shutdown Server");
	}
	
	if (this->clntSock>0)
	if (shutdown(this->clntSock, 2)) {
		perror("Shutdown Client");
	}
}	// close()

void XSocket :: connect_socket( int &xx, sockaddr_in &sock ) {
	if (connect( xx, (struct sockaddr *) &sock, sizeof( sock )) < 0 ) {
		perror("connect()");
		this->err0r = true;
	}
}

int XSocket :: recv_buffer( void *buff, std::size_t &maxlen, int &sock ) {
	int bytes_recv = 0;
	int total_recvd = 0;
	
	int stopAt = static_cast<int>(.15 * this->BUFFER_SIZE);
	void *temp_buffer;
	
	void *p = buff;
	
	if (!sock) {
		error( __FILE__, __LINE__, "Bad Socket" );
		return -1;
	}
	
	try {
		
		for(;;) {	/// be weary... this is an infinate loop.
			temp_buffer = new char[ this->BUFFER_SIZE ];
			bytes_recv = 0;

			if ((bytes_recv = recv(sock, temp_buffer, this->BUFFER_SIZE, 0)) < 0)  throw -1;
			total_recvd += bytes_recv;
			
			if (this->debug) {
				cout << "~\nRECIEVED::" << endl
					<< "total: " << total_recvd << "\trecv: " << bytes_recv << "\ttb: " << static_cast< char* >( temp_buffer ) << endl
					<< "stop: " << stopAt << "\tmax: " << maxlen << endl << '~' << endl;
			}
			if ( bytes_recv > 0)  memcpy( p, temp_buffer, bytes_recv);
			
			delete[] temp_buffer;  /// free up the memory

			if ((bytes_recv <= stopAt) || (total_recvd>=maxlen)) {
				if (!bytes_recv)  throw -2;
				if (total_recvd>=maxlen) throw -4;
				
				break;
			}
		}
	}
	
	catch( int i ) {
		switch( i ) {
			case -1:
				error( __FILE__, __LINE__, strerror( errno ) );
				errno = 0;
				break;
				
			case -2:
				error( __FILE__, __LINE__, "Connection Terminated by Foreign Host" );
				return total_recvd;
				break;
				
			case -3:
				error( __FILE__, __LINE__, "Pointer was null, trying again" );
				this->recv_buffer( buff, maxlen, sock );
				break;
				
			case -4:
				print( __FILE__, __LINE__, "Max Limit Hit" );
				return total_recvd;
				break;
				
			default:
				error( __FILE__, __LINE__, "I Dunno, something went wrong :" );
				errno = -1;
				break;
		}
	}

	/* done recv'ing */
	return total_recvd;
}; // end recv_buff()

int XSocket :: send_buffer( const void *buffer, std::size_t &maxLen, int &sock ) {
	int bytes_sent = 0;
	const void *p = buffer;
	
	if (this->debug) {
		cout << "~\nSENDING::" << endl
			<< "Len: " << maxLen << "\ta " << &p << "\td " << static_cast< const char* >( p ) << endl << '~' << endl;
	}
	
	if (!sock) {
		error( __FILE__, __LINE__, "Bad Socket" );
		return -1;
	}

	try {
		while ( bytes_sent < maxLen ) {
			if ((bytes_sent = send(sock, p, maxLen, 0)) == -1)  throw -1;
		}
	}
	
	catch( int e ) {
		error( __FILE__, __LINE__, strerror( errno ) );
		errno = 0;
	}

	return bytes_sent;
}; // end send_buff()




/** ERRORS  **/

void XSocket :: error( const char* file, const int line, const char* msg ) {
	cerr << endl << "[ERR][" << file << ':' << line << " :: " << msg << ']' << endl << endl;
	this->err0r = true;
}

void XSocket :: print( const char* file, const int line, const char* msg ) {
	cout << '*' << file << ':' << line << " :: " << msg << '*' << endl;
}

void XSocket :: print( const char* msg ) {
	cout << "Notice: " << msg << endl;
}
