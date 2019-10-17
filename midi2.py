#!/usr/bin/env python3
#
# test_midiin_poll.py
#
"""Show how to receive MIDI input by polling an input port."""

from __future__ import print_function
#from subprocess import Popen, PIPE
import traceback

import logging
import os
import sys
import time
import io
import signal
from rtmidi.midiutil import open_midiinput
signal.signal(signal.SIGPIPE, signal.SIG_DFL)

log = logging.getLogger('midiin_poll')
logging.basicConfig(level=logging.DEBUG)

# Prompts user for MIDI input port, unless a valid port number or name
# is given as the first argument on the command line.
# API backend defaults to ALSA on Linux.
port = sys.argv[1] if len(sys.argv) > 1 else None

try:
    midiin, port_name = open_midiinput(port)
except (EOFError, KeyboardInterrupt):
    sys.exit()

print("Entering main loop of python")

try:
    timer = time.time()
    while True:
            msg = midiin.get_message()
            if msg:
                message, deltatime = msg
                timer += deltatime
                print(str(message))
                sys.stdout.flush()
            time.sleep(0.01)
except KeyboardInterrupt:
    sys.stderr.write('')
finally:
    sys.stderr.write("Exit.\n")
    midiin.close_port()
    del midiin
