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

#ifndef _XSOCKETSERVER_H_
#include "XSocketServer.h"
#endif

int XSocketServer :: init( int port ) {
	int yes=1;        // for setsockopt() SO_REUSEADDR, below
	this->err0r = false;
	
	this->set_port( port );
	this->create_socket( this->servSock );
	

	// lose the pesky "address already in use" error message
        if (setsockopt( this->servSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            error( __FILE__, __LINE__, "Failed to Force Port Binding" );
        }
	
	this->reset_address( this->ServAddr );
	this->set_protocol( this->ServAddr );
	this->set_device( this->ServAddr );
	this->set_port( this->ServAddr );
	this->set_buffer_size();
	
	this->bind_socket( this->servSock, this->ServAddr );
	this->listen_socket( this->servSock );
}

int XSocketServer :: init( char* ip_addr, int port ) {
	int yes=1;        // for setsockopt() SO_REUSEADDR, below
	this->err0r = false;
	
	this->set_port( port );
	this->create_socket( this->servSock );
	

	// lose the pesky "address already in use" error message
        if (setsockopt( this->servSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            error( __FILE__, __LINE__, "Failed to Force Port Binding" );
        }
	
	this->reset_address( this->ServAddr );
	this->set_protocol( this->ServAddr );
	this->set_device( this->ServAddr, *ip_addr );
	this->set_port( this->ServAddr );
	this->set_buffer_size();
	
	this->bind_socket( this->servSock, this->ServAddr );
	this->listen_socket( this->servSock );
}

void XSocketServer :: begin ( int (*fncptr)( XSocketServer, int )) {
	int tempSock;
	this->nonBlocking = false;
	
	for(;;) {
		this->clntLen = sizeof( this->ClntAddr );

		if ((tempSock = accept( this->servSock, (struct sockaddr *) &this->ClntAddr, &this->clntLen)) < 0) {
			error(__FILE__, __LINE__, "XSERVER:BEGIN.accept");
		}

                if ( this->debug )  cout <<  "Handling cleint " << inet_ntoa( this->ClntAddr.sin_addr) << endl;
		cout << this->servSock << " << Socket" << endl;
		fncptr( *this, this->servSock );
	}
}

void XSocketServer :: non_blocking( int (*accptFncPtr)( XSocketServer, int ), int (*fncptr)( XSocketServer, int )) {
	int i=0;
	std::size_t addrlen;
	int newfd;
	
	this->nonBlocking = true;
	FD_ZERO( &this->fd_master );    // clear the master and temp sets
	FD_ZERO( &this->read_fds );

	// add the listener to the master set
	FD_SET( this->servSock, &this->fd_master );
	
	// keep track of the biggest file descriptor
	this->fd_max = this->servSock; // so far, it's this one

	/// main loop
	for (;;) {
		this->read_fds = this->fd_master; // copy it
		if (select( this->fd_max+1, &this->read_fds, 0, 0, 0) == -1) {
			error( __FILE__, __LINE__, "Select" );
			begin( fncptr );
		}
		
		for (i=0; i<=this->fd_max; i++) {
			if ( FD_ISSET(i, &this->read_fds) ) {  /// GOT ONE!
				if (i==this->servSock) {  /// new connection
					addrlen = sizeof( this->ClntAddr );
					if (( newfd = accept( this->servSock, (struct sockaddr *) &this->ClntAddr, &addrlen)) == -1) {
						error( __FILE__, __LINE__, "accepting new connection" );
					} else {
						FD_SET( newfd, &this->fd_master ); // add to master set
						if (newfd > this->fd_max)  this->fd_max = newfd;  // keep track of max
						
						printf("NOTICE: new connection from %s on socket %d\n", inet_ntoa( this->ClntAddr.sin_addr ), newfd);
					}
				} else {  // hand data from a client

					fncptr( *this, i );
				}
			}
		}
	}
}


void XSocketServer :: listen_socket( int &xx) {
	if ((listen (xx, this->maxpending)) == -1) {
		error( __FILE__, __LINE__, "Listen" );
	}
}

void XSocketServer :: set_maxpending( int xx ) {
	this->maxpending = xx;
}

template < class T >
int XSocketServer :: recv( T &xx, std::size_t len, int &clntID ) {
	
	len = this->recv_buffer( xx, len, clntID );

	if (!len || this->err0r) {  /// an error occured with this connection, lets kill it :P
		this->print(__FILE__, __LINE__, "Socket might have hung up");
		
		if (this->nonBlocking) {  /// is this a non blocking server?
			FD_CLR( clntID, &this->fd_master );
		}
		close( (int)clntID );
	}
	
	
	return len;
}


template < class T >
int XSocketServer :: recv(T *xx, std::size_t buff_s) {
	return this->recv( &xx, buff_s, this->servSock );
}

template < class T >
T XSocketServer :: recv( std::size_t buff_s ) {
	T ret;
	
	int len = this->recv( &ret, buff_s, this->servSock );
	return ret;
}

/**
	SEND
			**/

template < class T >
int XSocketServer :: send( T &xx, std::size_t len, int &clntID ) {
	return this->send_buffer( &xx, len, clntID );
}

template < class T >
int XSocketServer :: send( T *xx, std::size_t len, int &clntID ) {
	return this->send_buffer( &xx, len, clntID );
}

template < class T >
int XSocketServer :: send( T *xx, int &clntID ) {
	return this->send( xx, 512, clntID );
}

template < class T >
int XSocketServer :: send( T *xx, std::size_t len ) {
	return this->send_buffer( reinterpret_cast< const void* >(&xx), len, this->servSock );
}

/**
 *	Overloaded Operators
**/

template < class T >
int XSocketServer :: operator>> ( T* xx ) {
	return this->send( reinterpret_cast< const void* >(&xx), 512 );
}

template < class T >
T* XSocketServer :: operator<< ( std::size_t buff_s ) {
	return this->recv( buff_s );
}
