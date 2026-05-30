#!/usr/bin/env python3
"""
Serial Monitor for STM32 Black Pill
Reads from and writes to the UART serial port
"""

import serial
import sys
import threading
import time
import argparse

# Configuration
SERIAL_PORT = "/dev/cu.usbserial-110"
BAUD_RATE = 9600
TIMEOUT = 1

is_stm32_ready_to_receive = False
count = 0


gcode_list = [
    "G1 X20 Y0 F1000"
]

def read_from_serial(ser):
    """Continuously read from serial port and print"""
    print(f"[MONITOR] Listening on {SERIAL_PORT} at {BAUD_RATE} baud...")
    while True:
        try:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                #print(data)
                print(data.decode('utf-8', errors='replace'), end='', flush=True)
                    # Check for STM32 ready signal (e.g., "READY\n")
                if "READY" in data.decode('utf-8', errors='replace'):
                    global is_stm32_ready_to_receive
                    is_stm32_ready_to_receive = True
                    print("\n[MONITOR] STM32 is ready to receive data")
            else:
                time.sleep(0.1)  # Avoid busy waiting
        except Exception as e:
            print(f"[ERROR] Reading: {e}")
            break

def write_to_serial(ser):
    """Read from keyboard and send to serial port"""
    try:
        while True:
            # Read user input
            '''
            user_input = input()
            if user_input.lower() == 'quit':
                print("[MONITOR] Exiting...")
                break
                
            '''
            global is_stm32_ready_to_receive
            global count
            # Send to serial port (add newline)
            if is_stm32_ready_to_receive:
                if count >= len(gcode_list):
                    print("[MONITOR] All lines sent")
                    break
                print("Total Lines: " + str(len(gcode_list)) + " | Current Line: " + str(count + 1))
                print(f"[MONITOR] Sending: " + str(gcode_list[count]))
                ser.write((str(gcode_list[count]) + '\n').encode('utf-8'))
                ser.flush()
                is_stm32_ready_to_receive = False  # Reset until we get the next ready signal
                count += 1
                # Optionally buffer the input until STM32 is ready
                # For now, we just notify the user and discard the input
                # You could implement a queue to store inputs until STM32 is ready
                # and then send them once it is ready.
    except KeyboardInterrupt:
        print("\n[MONITOR] Interrupted by user")
    except Exception as e:
        print(f"[ERROR] Writing: {e}")

def main():
    parser = argparse.ArgumentParser(description='Serial monitor for STM32')
    parser.add_argument('-p', '--port', help='Serial port', default=SERIAL_PORT)
    parser.add_argument('-b', '--baud', help='Baud rate', type=int, default=BAUD_RATE)
    parser.add_argument('-f', '--file', help='G-code file to send, one command per line')

    args = parser.parse_args()

    # If a file was provided, load its non-empty lines into gcode_list
    if args.file:
        try:
            with open(args.file, 'r') as fh:
                lines = [ln.strip() for ln in fh.readlines()]
                # filter out empty and comment lines
                lines = [ln for ln in lines if ln and not ln.startswith('#')]
                if lines:
                    global gcode_list
                    gcode_list = lines
                    print(f"[MONITOR] Loaded {len(gcode_list)} lines from {args.file}")
        except Exception as e:
            print(f"[ERROR] Could not read file {args.file}: {e}")
            sys.exit(1)


    try:
        # Open serial port
        ser = serial.Serial(args.port, args.baud, timeout=TIMEOUT)
        print(f"[OK] Connected to {args.port} at {args.baud} baud")
        
        # Create threads for reading and writing
        read_thread = threading.Thread(target=read_from_serial, args=(ser,), daemon=True)
        write_thread = threading.Thread(target=write_to_serial, args=(ser,), daemon=True)
        
        # Start threads
        read_thread.start()
        write_thread.start()
        
        # Keep main thread alive
        read_thread.join()
        write_thread.join()
    except FileNotFoundError:
        print(f"[ERROR] Serial port {args.port} not found")
        print("Available ports:")
        import glob
        ports = glob.glob('/dev/cu.*')
        for port in ports:
            print(f"  {port}")
        sys.exit(1)
    except serial.SerialException as e:
        print(f"[ERROR] Serial connection failed: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"[ERROR] {e}")
        sys.exit(1)
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("[OK] Port closed")

if __name__ == "__main__":
    main()
