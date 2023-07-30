// nano_def.h

#pragma once
#ifndef __NANO_DEF_H__
#define __NANO_DEF_H__

// This file defines some nanonet type
//  1) addr_t: IP Address
//  2) port_t: Port number

#include <cstdint>

namespace nanonet {

typedef uint32_t addr_t;
typedef uint16_t port_t;

} // namespace nanonet


#endif // __NANO_DEF_H__