CoreXY minimal CNC firmware (plain C)

Overview
- Minimal CoreXY controller in plain C with G0/G1 parsing, Cartesian-space backlash compensation,
  Bresenham interpolation, and bit-banged stepper control.

Quick start (desktop unit tests)

Build (gcc-based unit tests):

```bash
# from repository root
gcc -I./src unit_test/gcode_executor_test.c src/*.c -o unit_test/gcode_executor_test
./unit_test/gcode_executor_test

# run expanded tests
gcc -I./src unit_test/expanded_tests.c src/*.c -o unit_test/expanded_tests
./unit_test/expanded_tests
```

Flashing to STM32 (PlatformIO)
- Open `platformio.ini` and build/upload via PlatformIO in VS Code.

Notes
- `src/main.cpp` contains a small Arduino-based serial G-code loop and wires `motor_control` helpers
  into the executor/driver.
- Backlash is applied in millimeters using `BACKLASH_*_MM` in `src/config.h`.
- Timed motion uses a simple trapezoidal profile controlled by `ACCEL_FRACTION` in `src/config.h`.

Next work
- Real probe integration for bed leveling
- More exhaustive unit tests and hardware validation
- Optional: replace bit-banged pulses with hardware timers for deterministic timing
