import _thread
import cpv2017
import gc

_thread.start_new_thread(cpv2017.UserInterface().start, ())
cpv2017.LEDs.start_animation_thread()
gc.threshold(2048)