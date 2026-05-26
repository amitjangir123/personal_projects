#!/usr/bin/env python3
"""
Custom upload script for STM32F411 using stm32flash via serial bootloader
"""
import subprocess
import sys
import os

# Get environment variables from PlatformIO
upload_port = os.environ.get('UPLOAD_PORT', '/dev/cu.usbserial-10')
upload_speed = os.environ.get('UPLOAD_SPEED', '115200')
firmware_file = sys.argv[1] if len(sys.argv) > 1 else '.pio/build/genericSTM32F411CE/firmware.bin'

# Use the full path to the newly built stm32flash
stm32flash_path = '/usr/local/bin/stm32flash'

try:
    # Attempt upload with stm32flash
    cmd = [
        stm32flash_path,
        '-b', upload_speed,
        '-w', firmware_file,
        '-v',
        '-g', '0x0',
        upload_port
    ]
    
    print(f"Uploading {firmware_file} to {upload_port} at {upload_speed} baud...")
    result = subprocess.run(cmd, check=False)
    
    if result.returncode == 0:
        print("Upload successful!")
        sys.exit(0)
    else:
        print(f"stm32flash failed with return code {result.returncode}")
        sys.exit(1)
        
except FileNotFoundError:
    print(f"stm32flash not found at {stm32flash_path}")
    sys.exit(1)
