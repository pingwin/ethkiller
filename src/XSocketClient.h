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
#define _XSOCKETCLIENT_H_


class XSocketClient : public XSocket {
	public:
		/*
			INIT
					*/
		int init( char *, int );
		int init( char *, int, char * );
		
		/*
			RECV
					*/
		template < class T >
		int recv( T &, std::size_t, int & );
		
		template < class T >
		int recv( T*, std::size_t );
		
		template < class T >
		T* recv( std::size_t );
		
		/*
			SEND
					*/
		template < class T >
		int send( T&, std::size_t, int& );
		
		template < class T >
		int send( T&, std::size_t );
		
		template < class T >
		int send(	T*, std::size_t );

		template < class T >
		int send( T* );

		/*
			OPERATORS
					*/
		template < class T >
		int operator>> ( T* );
		
		
		template < class T >
		int operator<< ( T* );
		
}; // end XSocketClient Definition

#include "XSocketClient.cpp"

#endif
