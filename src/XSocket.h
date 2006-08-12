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
#define _XSOCKET_H_

#include <netdb.h>  ///  for struct sockaddr_in

class XSocket {
	public:
 		int init();
		int set_port( int );
		void hostname( char* , char *(&) );
		void Xclose();

		bool err0r;
		template< typename T, std::size_t N >
		std::size_t nElements( const T (&)[N] );

		void set_debug( bool );
		
	protected:
		bool validate_port( int );
		void create_socket( int & );
		void bind_socket( int &, sockaddr_in &  );
		void connect_socket( int &, sockaddr_in & );
		
		
		void reset_address( sockaddr_in & );
		void set_protocol( sockaddr_in & );
		
		void set_device( sockaddr_in & );
		void set_device( sockaddr_in &, char & );
		
		void set_destination( sockaddr_in & );
		void set_port( sockaddr_in & );
		
		void set_buffer_size( size_t );
		void set_buffer_size();

		void error( const char*, const int, const char* );
		void print( const char*, const int, const char* );
		void print( const char* );
		
		int recv_buffer( void *, std::size_t &, int & );
		int send_buffer( const void *, std::size_t &, int & );
		
		int servSock;
		int clntSock;

		int port;
		std::size_t BUFFER_SIZE;

		bool debug;

		char *dest_addr;

		struct sockaddr_in ServAddr;
		struct sockaddr_in ClntAddr;
};	/// end XSocket Definition

#include "XSocket.cpp"
#endif
