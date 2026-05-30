#ifndef BED_LEVELING_H
#define BED_LEVELING_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Probe a point and store Z offset
void probePoint(float x, float y);

// Get Z offset for a given X, Y position
float getZOffset(float x, float y);

#ifdef __cplusplus
}
#endif

#endif
