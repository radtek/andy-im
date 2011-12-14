//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// IOCPOverlapped.h: overlappedµÄ·â×°Àà
//
// Author: Leo
// Date: 08/04/2008
//
////////////////////////////////////////////////////////////////////// 
#ifndef _IOCP_OVERLAPPED_H_
#define _IOCP_OVERLAPPED_H_

typedef struct tagMyOverlapped
{
    WSAOVERLAPPED wsaOvLap;

    SOCKET skclient;       // Used for AcceptEx client socket
    HANDLE hPostAccept;

    int buflen;        // Length of the buffer
	char *buf;           // Buffer for recv/send/AcceptEx
    
    Operation eOperation;     // Type of operation issued
	bool bStatus;  // 0 idle status, 1 busy status

    SOCKADDR addr;
    int iaddrlen;

}MyOverlapped;


//
typedef struct _LISTEN_OBJ
{
    SOCKET          s;
	
    int             AddressFamily;
	
    MyOverlapped     *PendingAccepts; // Pending AcceptEx buffers 
	
    volatile long   PendingAcceptCount;
	
    int             HiWaterMark;
	int 	        LoWaterMark;
	
    HANDLE          AcceptEvent;
    HANDLE          RepostAccept;
    volatile long   RepostCount;
	
    // Pointers to Microsoft specific extensions.
    LPFN_ACCEPTEX lpfnAcceptEx;
    LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs;
	
    CRITICAL_SECTION ListenCritSec;
	
    struct _LISTEN_OBJ *next;
} LISTEN_OBJ;


#endif