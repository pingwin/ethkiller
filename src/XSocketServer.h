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

#ifndef _XSOCKETSERVER_H_
#define _XSOCKETSERVER_H_

#define LISTEN_SELECT 1
 

class XSocketServer : public XSocket {
	public:
		/*
			INIT
					*/
		int init( int );
		int init( char *, int );
		
		/*
			MODES
					*/
		void begin( int (*fncptr)( XSocketServer, int ) );
		void non_blocking( int (*)(XSocketServer, int), int (*)(XSocketServer, int) );
		
		/*
			SET
					*/
		void set_maxpending( int ); 

		/*
			RECV
					*/
		template < class T >
		T recv( std::size_t );

		template < class T >
		int recv( T *, std::size_t );
		
 		template < class T >
 		int recv( T &, std::size_t, int &);


		/*
			SEND
					*/
		template < class T >
		int send( T*, std::size_t );
		
		template < class T >
		int send( T&, std::size_t, int & );
		
		template < class T >
		int send( T*, std::size_t, int & );
		
		template < class T >
		int send( T*, int & );
		
		/*
			OPERATOR
					*/
		template < class T > T* operator<< ( std::size_t );
		template < class T > int operator>> ( T* );
		
	private: ///
		void listen_socket(int &xx);
		
		/*
			GLOBALS
					*/
		bool nonBlocking;
		
		std::size_t clntLen;
		int maxpending;
		
		// for non-blocking
		fd_set fd_master;  // master file descriptor list
		fd_set read_fds;   // temp file descriptor list for select()
		int fd_max;	   // maximum file descriptor number
};

#include "XSocketServer.cpp"
#endif
