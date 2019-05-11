import sys
import serial
import time

if len(sys.argv) < 3:
  print("Too frew args, please give comm port and destination file");
  exit();

print("Opening comm port " + sys.argv[1]);
ser1 = serial.Serial(sys.argv[1], 115200, timeout=5);
f = open(sys.argv[2],"w+")

print("Files open waiting for arduino ");
time.sleep(2);
print("Starting read");

while True:
  text = ser1.readline();
  print(text);
  f.write(text)

