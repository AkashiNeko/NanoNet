// nano_def.hpp

#pragma once
#ifndef __NANO_DEF_HPP__
#define __NANO_DEF_HPP__

// This file defines some nanonet type
//  1) addr_t: IP Address
//  2) port_t: Port number

// C
#include <cstdint>

namespace nanonet {

typedef uint32_t addr_t;
typedef uint16_t port_t;

} // namespace nanonet


#endif // __NANO_DEF_HPP__