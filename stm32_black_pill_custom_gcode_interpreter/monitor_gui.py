#!/usr/bin/env python3
"""
Simple Tkinter GUI wrapper for monitor.py functionality

Features:
- Connect/disconnect serial port
- Select a G-code file and view its lines
- Send single lines or send-all (waits for READY)
- Jog controls (X/Y/Z +/- configurable distance)
- Log pane showing sent and received messages
"""

import tkinter as tk
from tkinter import filedialog, messagebox
from tkinter.scrolledtext import ScrolledText
import threading
import queue
import serial
import time
import os

DEFAULT_PORT = "/dev/cu.usbserial-110"
DEFAULT_BAUD = 9600
DEFAULT_TIMEOUT = 1


class SerialManager:
    def __init__(self, port, baud, timeout=DEFAULT_TIMEOUT, rx_queue=None):
        self.port = port
        self.baud = baud
        self.timeout = timeout
        self.ser = None
        self.rx_thread = None
        self._stop = threading.Event()
        self.rx_queue = rx_queue or queue.Queue()
        self.ready_event = threading.Event()

    def open(self):
        self.ser = serial.Serial(self.port, self.baud, timeout=self.timeout)
        self._stop.clear()
        self.rx_thread = threading.Thread(target=self._reader, daemon=True)
        self.rx_thread.start()

    def close(self):
        self._stop.set()
        if self.ser and self.ser.is_open:
            try:
                self.ser.close()
            except Exception:
                pass

    def write_line(self, line):
        if not self.ser or not self.ser.is_open:
            raise serial.SerialException("Port not open")
        data = (line.rstrip('\n') + '\n').encode('utf-8')
        self.ser.write(data)
        self.ser.flush()

    def _reader(self):
        while not self._stop.is_set():
            try:
                if self.ser.in_waiting > 0:
                    data = self.ser.read(self.ser.in_waiting)
                    text = data.decode('utf-8', errors='replace')
                    # Put data into queue for GUI
                    self.rx_queue.put(text)
                    if 'READY' in text:
                        self.ready_event.set()
                else:
                    time.sleep(0.05)
            except Exception as e:
                self.rx_queue.put(f"[ERROR] Reader: {e}\n")
                break


class MonitorGUI:
    def __init__(self, root):
        self.root = root
        root.title('STM32 Serial Monitor - GUI')

        # Serial manager
        self.rx_queue = queue.Queue()
        self.serial = None

        # G-code data
        self.gcode_lines = []
        self.send_index = 0
        self.sending_thread = None

        # UI layout
        top_frame = tk.Frame(root)
        top_frame.pack(fill='x', padx=6, pady=6)

        tk.Label(top_frame, text='Port:').pack(side='left')
        self.port_var = tk.StringVar(value=DEFAULT_PORT)
        tk.Entry(top_frame, textvariable=self.port_var, width=20).pack(side='left')

        tk.Label(top_frame, text='Baud:').pack(side='left', padx=(8,0))
        self.baud_var = tk.IntVar(value=DEFAULT_BAUD)
        tk.Entry(top_frame, textvariable=self.baud_var, width=8).pack(side='left')

        self.connect_btn = tk.Button(top_frame, text='Connect', command=self.toggle_connect)
        self.connect_btn.pack(side='left', padx=8)

        file_frame = tk.Frame(root)
        file_frame.pack(fill='both', expand=False, padx=6)

        tk.Button(file_frame, text='Open G-code File', command=self.open_file).pack(side='left')
        self.send_all_btn = tk.Button(file_frame, text='Send All', command=self.start_send_all, state='disabled')
        self.send_all_btn.pack(side='left', padx=6)
        self.send_line_btn = tk.Button(file_frame, text='Send Line', command=self.send_selected_line, state='disabled')
        self.send_line_btn.pack(side='left')

        main_pane = tk.PanedWindow(root, sashrelief='raised', orient='horizontal')
        main_pane.pack(fill='both', expand=True, padx=6, pady=6)

        # Left: gcode list
        left_frame = tk.Frame(main_pane)
        tk.Label(left_frame, text='G-code Lines').pack(anchor='w')
        self.listbox = tk.Listbox(left_frame, width=50, height=25)
        self.listbox.pack(fill='both', expand=True)
        main_pane.add(left_frame)

        # Right: log and jog
        right_frame = tk.Frame(main_pane)
        tk.Label(right_frame, text='Log (sent/received)').pack(anchor='w')
        self.log = ScrolledText(right_frame, width=80, height=20, state='disabled')
        self.log.pack(fill='both', expand=True)

        jog_frame = tk.Frame(right_frame)
        jog_frame.pack(fill='x', pady=6)
        tk.Label(jog_frame, text='Jog Dist:').pack(side='left')
        self.jog_dist = tk.DoubleVar(value=1.0)
        tk.Entry(jog_frame, textvariable=self.jog_dist, width=6).pack(side='left')

        def add_jog_btn(text, cmd):
            return tk.Button(jog_frame, text=text, command=lambda: self.jog(cmd))

        add_jog_btn('X+', 'X+').pack(side='left', padx=2)
        add_jog_btn('X-', 'X-').pack(side='left', padx=2)
        add_jog_btn('Y+', 'Y+').pack(side='left', padx=2)
        add_jog_btn('Y-', 'Y-').pack(side='left', padx=2)
        add_jog_btn('Z+', 'Z+').pack(side='left', padx=2)
        add_jog_btn('Z-', 'Z-').pack(side='left', padx=2)

        main_pane.add(right_frame)

        # Polling loop
        self.root.after(100, self.poll_rx_queue)

    def toggle_connect(self):
        if self.serial and self.serial.ser and self.serial.ser.is_open:
            self.serial.close()
            self.serial = None
            self.connect_btn.config(text='Connect')
            self.append_log('[OK] Disconnected\n')
            self.send_all_btn.config(state='disabled')
            self.send_line_btn.config(state='disabled')
        else:
            port = self.port_var.get()
            baud = int(self.baud_var.get())
            try:
                self.serial = SerialManager(port, baud, rx_queue=self.rx_queue)
                self.serial.open()
                self.append_log(f'[OK] Connected to {port} at {baud} baud\n')
                self.connect_btn.config(text='Disconnect')
                if self.gcode_lines:
                    self.send_all_btn.config(state='normal')
                    self.send_line_btn.config(state='normal')
            except Exception as e:
                messagebox.showerror('Connection Error', str(e))

    def open_file(self):
        path = filedialog.askopenfilename(filetypes=[('G-code', '*.gcode;*.nc;*.txt'), ('All files','*.*')])
        if not path:
            return
        try:
            with open(path, 'r') as fh:
                lines = [ln.strip() for ln in fh.readlines()]
                lines = [ln for ln in lines if ln and not ln.startswith('#')]
                self.gcode_lines = lines
                self.listbox.delete(0, tk.END)
                for ln in lines:
                    self.listbox.insert(tk.END, ln)
                self.append_log(f'[MONITOR] Loaded {len(lines)} lines from {os.path.basename(path)}\n')
                if self.serial and self.serial.ser and self.serial.ser.is_open:
                    self.send_all_btn.config(state='normal')
                    self.send_line_btn.config(state='normal')
        except Exception as e:
            messagebox.showerror('File Error', str(e))

    def send_selected_line(self):
        sel = self.listbox.curselection()
        if not sel:
            messagebox.showinfo('Select', 'Select a line to send')
            return
        line = self.listbox.get(sel[0])
        try:
            self.serial.write_line(line)
            self.append_log(f'> {line}\n')
        except Exception as e:
            messagebox.showerror('Send Error', str(e))

    def start_send_all(self):
        if not self.gcode_lines:
            messagebox.showinfo('No lines', 'Load a G-code file first')
            return
        if not self.serial or not (self.serial.ser and self.serial.ser.is_open):
            messagebox.showinfo('Not connected', 'Open serial connection first')
            return
        # Start sending thread
        if self.sending_thread and self.sending_thread.is_alive():
            messagebox.showinfo('Sending', 'Already sending')
            return
        self.send_index = 0
        self.serial.ready_event.clear()
        self.sending_thread = threading.Thread(target=self._send_all_worker, daemon=True)
        self.sending_thread.start()

    def _send_all_worker(self):
        try:
            while self.send_index < len(self.gcode_lines):
                # Wait for READY
                self.append_log(f'[INFO] Waiting for READY to send line {self.send_index+1}/{len(self.gcode_lines)}\n')
                # Wait until ready_event set (with timeout to allow UI updates)
                ready = self.serial.ready_event.wait(timeout=10)
                if not ready:
                    self.append_log('[WARN] Timeout waiting for READY, will retry\n')
                    continue
                line = self.gcode_lines[self.send_index]
                try:
                    self.serial.write_line(line)
                    self.append_log(f'> {line}\n')
                except Exception as e:
                    self.append_log(f'[ERROR] Writing: {e}\n')
                    break
                self.send_index += 1
                # clear ready and wait for next READY from device
                self.serial.ready_event.clear()
            self.append_log('[OK] Send-all complete\n')
        except Exception as e:
            self.append_log(f'[ERROR] Send worker: {e}\n')

    def jog(self, axis_cmd):
        if not self.serial or not (self.serial.ser and self.serial.ser.is_open):
            messagebox.showinfo('Not connected', 'Open serial connection first')
            return
        d = float(self.jog_dist.get())
        if axis_cmd.endswith('-'):
            d = -d
        axis = axis_cmd[0]
        # Use relative move sequence: G91 (relative), G0, G90 (back to absolute)
        seq = [f'G91', f'G0 {axis}{d}', f'G90']
        try:
            for ln in seq:
                self.serial.write_line(ln)
                self.append_log(f'> {ln}\n')
                time.sleep(0.05)
        except Exception as e:
            self.append_log(f'[ERROR] Jog write: {e}\n')

    def append_log(self, text):
        self.log.configure(state='normal')
        self.log.insert(tk.END, text)
        self.log.see(tk.END)
        self.log.configure(state='disabled')

    def poll_rx_queue(self):
        try:
            while True:
                text = self.rx_queue.get_nowait()
                self.append_log(text)
        except queue.Empty:
            pass
        self.root.after(100, self.poll_rx_queue)


def main():
    root = tk.Tk()
    app = MonitorGUI(root)
    root.protocol('WM_DELETE_WINDOW', lambda: (app.serial.close() if app.serial else None, root.destroy()))
    root.mainloop()


if __name__ == '__main__':
    main()
