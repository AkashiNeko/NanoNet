// nanonet.h

#pragma once
#ifndef __NANONET_H__
#define __NANONET_H__

#if __cplusplus < 201103L
    #error "Sorry, nanonet requires at least C++11"
#endif

// utility
#include "nanonet/utility/log.h"
#include "nanonet/utility/addr.h"
#include "nanonet/utility/port.h"
#include "nanonet/utility/addr_port.h"
#include "nanonet/utility/url.h"

// tcp
#include "nanonet/tcp/socket.h"
#include "nanonet/tcp/server_socket.h"

// udp
#include "nanonet/udp/udp_socket.h"

// http
#include "nanonet/http/http_request.h"
#include "nanonet/http/http_respond.h"
#include "nanonet/http/http_assembler.h"
#include "nanonet/http/requests.h"
#include "nanonet/http/server/web_server.h"

// exception
#include "nanonet/exception/nanoerr.h"

#endif // __NANONET_H__
