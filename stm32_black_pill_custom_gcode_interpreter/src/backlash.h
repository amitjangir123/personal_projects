#ifndef BACKLASH_H
#define BACKLASH_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Apply Cartesian backlash compensation to `target` based on `current` position
// and the last known movement directions in `dir_state`.
// - current: current Cartesian position (mm)
// - target: desired Cartesian target (mm). This function may modify `target` to
//   include additional offsets to overcome mechanical backlash.
// - dir_state: tracks last movement directions per axis (1 or -1). The function
//   updates dir_state when a reversal is detected.
void applyBacklash(const Position* current, Position* target, DirectionState* dir_state);

#ifdef __cplusplus
}
#endif

#endif
