#include "bed_leveling.h"
// Simple in-memory probe grid (3x3) for basic bed leveling support.
// This is a small, test-friendly stub implementation. Real probe
// hardware integration should update the grid by probing physical points.

static float grid_x[3] = {0.0f, 50.0f, 100.0f};
static float grid_y[3] = {0.0f, 50.0f, 100.0f};
static float z_offsets[3][3] = {{0}};

void probePoint(float x, float y) {
  // Stub: store zero offset for now. Real implementation would read a sensor
  // and store measured Z offset in z_offsets[][] using nearest-grid indexing.
  int ix = 0, iy = 0;
  if (x >= 75.0f) ix = 2;
  else if (x >= 25.0f) ix = 1;
  if (y >= 75.0f) iy = 2;
  else if (y >= 25.0f) iy = 1;
  z_offsets[ix][iy] = 0.0f;
}

float getZOffset(float x, float y) {
  // Return nearest-grid Z offset (no interpolation for simplicity)
  int ix = 0, iy = 0;
  if (x >= 75.0f) ix = 2;
  else if (x >= 25.0f) ix = 1;
  if (y >= 75.0f) iy = 2;
  else if (y >= 25.0f) iy = 1;
  return z_offsets[ix][iy];
}
