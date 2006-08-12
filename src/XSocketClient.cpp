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

#ifndef _XSOCKETCLIENT_H_
#include "XSocketClient.h"
#endif

/**
	INIT
			**/
int XSocketClient :: init(char *ip_addr, int port) {
	this->err0r = false;
	this->create_socket(this->clntSock);

	this->reset_address( this->ClntAddr );
	
	this->hostname(ip_addr, this->dest_addr);
	
	this->set_protocol( this->ClntAddr );
	this->set_device( this->ClntAddr );
	this->set_destination( this->ClntAddr );
	this->set_port( port );
	this->set_port( this->ClntAddr );
	
	this->set_buffer_size();
	
	this->connect_socket( this->clntSock, this->ClntAddr );
} // end XSocketClient()

int XSocketClient :: init(char *ip_addr, int port, char *bind_ip_addr) {
	this->err0r = false;
	this->create_socket(this->clntSock);

	this->reset_address( this->ClntAddr );
	
	this->hostname(ip_addr, this->dest_addr);
	
	this->set_protocol( this->ClntAddr );
	this->set_device( this->ClntAddr, *bind_ip_addr );
	this->set_destination( this->ClntAddr );
	this->set_port( port );
	this->set_port( this->ClntAddr );
	
	this->set_buffer_size();
	
	this->connect_socket( this->clntSock, this->ClntAddr );
} // end XSocketClient()

/**
	RECV
			**/
template < class T >
int XSocketClient :: recv( T &xx, std::size_t len, int &clntSock ) {
	return this->recv_buffer( xx, len, clntSock );
}

template < class T >
int XSocketClient :: recv( T *xx, std::size_t len ) {
	return this->recv( xx, len, this->clntSock );
}	// end recv()

template < class T >
T* XSocketClient :: recv( std::size_t len ) {
	T *xx;
	this->recv( xx, len, this->clntSock );
	return xx;
}

/**
	SEND
			**/
template < class T >
int XSocketClient :: send( T &xx, std::size_t len, int &clntSock ) {
	return this->send_buffer( xx, len, clntSock );
}

template < class T >
int XSocketClient :: send( T &xx, std::size_t len ) {
	return this->send( *xx, len, this->clntSock );
}

template < class T >
int XSocketClient :: send( T *xx, std::size_t len ) {
	return this->send( xx, len, this->clntSock );
}


/**
 *	Overloaded Operators
**/


template < class T >
int XSocketClient :: operator<< ( T *xx ) {
	return this->send( xx, 512, this->clntSock );
}

template < class T >
int XSocketClient :: operator>> ( T *xx ) {
	return this->recv( xx, sizeof( xx ), this->clntSock );
}
