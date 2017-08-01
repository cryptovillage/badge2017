#!/usr/bin/python

import os
import sys
import time
from watchdog.observers import Observer
import watchdog.events
import subprocess

class Handler(watchdog.events.FileSystemEventHandler):
  def on_created(self, event):
    print(event.src_path[:11])
    if (event.src_path[:11] == "/dev/ttyACM"):
      subprocess.Popen(("python esptool/esptool.py --chip esp32 --port %s --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x10000 cpv2017.bin 0x8000 partitions.bin" % (event.src_path)).split(" "))

observer = Observer()
observer.schedule(Handler(), "/dev", recursive=False)
observer.start()

try:
  while True:
    time.sleep(1)
except KeyboardInterrupt:
  observer.stop()
observer.join()

