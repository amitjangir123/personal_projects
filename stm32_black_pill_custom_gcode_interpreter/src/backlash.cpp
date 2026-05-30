#include "backlash.h"
#include <math.h>

// Backlash values are configured in millimeters in config.h
static float backlash_mm_x(void) { return (float)BACKLASH_X_MM; }
static float backlash_mm_y(void) { return (float)BACKLASH_Y_MM; }
static float backlash_mm_z(void) { return (float)BACKLASH_Z_MM; }

void applyBacklash(const Position* current, Position* target, DirectionState* dir_state) {
  // X axis
  float dx = target->x - current->x;
  if (fabsf(dx) > 0.0f) {
    int new_dir = (dx > 0.0f) ? 1 : -1;
    if (dir_state->x_dir == 0) {
      // initialize direction if unknown
      dir_state->x_dir = new_dir;
      {
        (void)new_dir;
      }
    } else if (new_dir != dir_state->x_dir) {
      // direction reversal: apply compensation in direction of new motion
      float comp = new_dir * backlash_mm_x();
      target->x += comp;
      dir_state->x_dir = new_dir;
      {
        (void)comp; (void)target;
      }
    }
  }

  // Y axis
  float dy = target->y - current->y;
  if (fabsf(dy) > 0.0f) {
    int new_dir = (dy > 0.0f) ? 1 : -1;
    if (dir_state->y_dir == 0) {
      dir_state->y_dir = new_dir;
      {
        (void)new_dir;
      }
    } else if (new_dir != dir_state->y_dir) {
      float comp = new_dir * backlash_mm_y();
      target->y += comp;
      dir_state->y_dir = new_dir;
      {
        (void)comp; (void)target;
      }
    }
  }

  // Z axis
  float dz = target->z - current->z;
  if (fabsf(dz) > 0.0f) {
    int new_dir = (dz > 0.0f) ? 1 : -1;
    if (dir_state->z_dir == 0) {
      dir_state->z_dir = new_dir;
      {
        (void)new_dir;
      }
    } else if (new_dir != dir_state->z_dir) {
      float comp = new_dir * backlash_mm_z();
      target->z += comp;
      dir_state->z_dir = new_dir;
      {
        (void)comp; (void)target;
      }
    }
  }
  {
    (void)target;
    (void)dir_state;
  }
}
