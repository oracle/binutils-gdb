try:
   import thread
except:
   import _thread
import time
import gdb

# Define a function for the thread
def print_thread_hello():
   count = 0
   while count < 10:
      time.sleep(1)
      count += 1
      print ("Hello ( %d )" % count)

# Create a threads a continue
try:
   thread.start_new_thread (print_thread_hello, ())
   gdb.execute ("continue", release_gil=True)
except:
   try:
      _thread.start_new_thread (print_thread_hello, ())
      gdb.execute ("continue", release_gil=True)
   except:
      print ("Error: unable to start thread")

while 1:
   pass
