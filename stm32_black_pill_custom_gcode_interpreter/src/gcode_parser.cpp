#include "gcode_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * parseGcode - Parse a single line of G-code
 * 
 * This function takes a G-code line and extracts:
 * - Command type (G or M)
 * - Command code (0, 1, 90, 92, etc.)
 * - Parameters (X, Y, Z, F coordinates and feedrate)
 * 
 * Examples:
 *   "G00 X1.1978 Y4.1741"    → G0 rapid move to X=1.1978, Y=4.1741
 *   "G01 Z0.0000 F1000.00"   → G1 linear move to Z=0, feedrate=1000 mm/min
 *   "G92 X0 Y0 Z0"           → Set current position as origin
 *   "M03"                    → Spindle on
 *   "(comment here)"         → Ignored (comment line)
 * 
 * @param line: G-code line string (e.g., "G01 X10 Y20")
 * @return: GcodeCommand struct with parsed values and flags
 */
GcodeCommand parseGcode(const char* line) {
  GcodeCommand cmd = {0};
  
  // Initialize all flags to 0 (parameter not provided)
  // This helps us know which parameters were actually in the line
  cmd.hasX = cmd.hasY = cmd.hasZ = cmd.hasF = 0;
  cmd.x = cmd.y = cmd.z = cmd.f = 0.0;
  
  // Skip leading whitespace (spaces, tabs)
  // Example: "  G01 X10" → skip the two spaces
  while (*line && isspace(*line)) {
    line++;
  }
  
  // Ignore empty lines and comments
  // Lines starting with ( or ; are comments and should be skipped
  // Example: "(This is a comment)" or "; Another comment"
  if (!*line || *line == '(' || *line == ';') {
    cmd.command = 0;  // Invalid/comment command
    return cmd;
  }
  
  // Parse command letter (G, M, etc.)
  // Example: "G01 ..." → cmd.command = 'G'
  cmd.command = *line;
  line++;
  
  // Parse command number (0, 1, 90, 92, etc.)
  // Example: "G01 ..." → cmd.code = 1
  // Example: "G92 ..." → cmd.code = 92
  if (isdigit(*line)) {
    cmd.code = atoi(line);  // Convert string to integer
    while (*line && isdigit(*line)) {
      line++;  // Move past all digits
    }
  }
  
  // Parse parameters (X, Y, Z, F)
  // Each parameter is a letter followed by a number
  // Example: "G01 X10.5 Y20 Z0" has 3 parameters
  while (*line) {
    // Get the parameter letter and convert to uppercase
    // Example: "X10" → param = 'X'
    char param = toupper(*line);
    line++;
    
    // Skip whitespace after parameter letter
    // Example: "X  10" → skip the two spaces
    while (*line && isspace(*line)) {
      line++;
    }
    
    // Parse the numeric value
    // Example: "X10.5" → value = 10.5
    // Example: "Z-1.5" → value = -1.5 (negative)
    if (isdigit(*line) || *line == '-' || *line == '.') {
      float value = atof(line);  // Convert string to float
      
      // Store the value in the appropriate field
      // Set the corresponding flag to 1 (parameter was provided)
      switch (param) {
        case 'X':
          cmd.x = value;
          cmd.hasX = 1;  // Mark that X was provided
          break;
        case 'Y':
          cmd.y = value;
          cmd.hasY = 1;  // Mark that Y was provided
          break;
        case 'Z':
          cmd.z = value;
          cmd.hasZ = 1;  // Mark that Z was provided
          break;
        case 'F':
          cmd.f = value;
          cmd.hasF = 1;  // Mark that F was provided (feedrate)
          break;
      }
      
      // Skip past the number (all digits, decimal point, minus sign)
      if (*line == '-') line++;  // Skip minus sign if present
      while (*line && (isdigit(*line) || *line == '.')) {
        line++;  // Move past each character of the number
      }
    }
    
    // Skip whitespace to prepare for next parameter
    // Example: "X10 Y20" → after parsing X10, skip space before Y
    while (*line && isspace(*line)) {
      line++;
    }
  }
  
  return cmd;
}

/**
 * Usage Example in main():
 * 
 *   char line[] = "G01 X10.5 Y20 Z0 F300";
 *   GcodeCommand cmd = parseGcode(line);
 *   
 *   // Check what was parsed:
 *   if (cmd.command == 'G') {
 *     if (cmd.code == 1) {
 *       // This is a G01 (linear move)
 *     }
 *   }
 *   
 *   // Check if parameters were provided:
 *   if (cmd.hasX) {
 *     // Use cmd.x value
 *   }
 *   if (cmd.hasY) {
 *     // Use cmd.y value
 *   }
 *   if (cmd.hasZ) {
 *     // Use cmd.z value
 *   }
 *   if (cmd.hasF) {
 *     // Use cmd.f value for feedrate
 *   }
 */
