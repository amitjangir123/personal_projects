#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Parse a G-code line
GcodeCommand parseGcode(const char* line);

#ifdef __cplusplus
}
#endif

#endif
