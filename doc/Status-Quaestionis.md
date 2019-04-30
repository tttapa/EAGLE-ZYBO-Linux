# Status Quaestionis

## ZYBO Linux

For compiling C/C++ applications for the ZYBO core running Linux, a GNU
Toolchain is used. This toolchain is available as a Docker image for maximum
portability, because installing GCC etc. on Windows is not trivial.

The Docker image also contains a native x86-64 toolchain for testing and
debugging. This allows you to run the applications not only on the ZYBO, but on
any normal computer as well.

All build tasks are available as tasks in Visual Studio Code, this includes
building the Docker images, toolchains, libraries, ZYBO applications, testing,
debugging.

### Crypto

✓ I provided the basic structure for the Crypto code. It just writes a
single word to R0, and then reads it back from R1.

✓ I included the ZBar library for decoding the QR code. I also tried the OpenCV
QR decoder, but ZBar seems to work better. I added a test as an example to run
it asynchronously, that is, vision keeps on refreshing while the QR decoder is
running. It remains to be seen if this is a good idea, because we only have a 
single thread anyway.

✓ I also added a base64-decoder library for extracting the binary data from the 
QR code.

🗙 The next step is to write functions that are able to decode strings of data, 
as well as more elaborate test cases.

The public API of the crypto code goes in `src/crypto/include/crypto.hpp`, and
the implementations in `src/crypto/src/crypto.cpp`.  
Test cases go in `src/crypto/test/test-crypto.cpp`.

🗙 The reading and writing of the FPGA registers uses the provided code in
`src/comm/src/wrapper.c`, from [Xilinx Application note 1078](https://www.xilinx.com/support/documentation/application_notes/xapp1078-amp-linux-bare-metal.pdf).  
This is not ideal, because it has a lot of overhead when reading large amounts
of data. The code is simple enough, and adding a function for doing longer
transactions to the FPGA is a good idea, instead of calling the `rmem()` and
`wmem()` functions for each word separately.

✓ Tuur and Cyriel are working on the code that reads and decodes the QR code, 
and sends the data to crypto.

### Logging

The logging on the Linux side is pretty basic: it just reads the size of the log
entry from shared memory (`wrapper.h`), and then reads this many words and sends
them as a multicast UDP packet.

✓ The multicast sender has been implemented and tested.

🗙 The part that reads from shared memory is trivial, but not finished yet,
we'll have to finish the logger in Baremetal first.  
It's important that we use different locks for logging and vision.

🗙 If we can't get multicast routing to work for the base station, we might have
to send the same message as unicast as well.

### Vision

✓ The main part of the vision algorithm is GridFinder: as an input, it uses the
mask, where red lines are white, and everything else is black. As an output, it
returns the vertices of the square of the grid in the center of the frame, and
the lines (and angles) of that square.  
This part is finished, and has been tested using videos.

✓ I've completely re-written the red pixel detection for generating the masks.  
It now uses ARM NEON SIMD vector instructions, so it can mask 16 pixels in 
parallel. There's also a function that's a bit more accurate, but a bit slower.
If dark, washed-out red is detected as a line, and if this is a problem, we 
could use that code instead.

✓ Reading the image from the HDMI input seems to be working. There is a purple
line at the edge of the frame though, that I can't seem to get rid of.

✓ Getting the absolute position of the drone seems to be working. The entire
vision program runs at 60 fps on the ZYBO, but every two frames are the same,
because the camera only updates at 30 fps.

🗙 Finally, this location has to be sent to the BareMetal core. This is trivial.

## ZYBO Baremetal

### BaremetalImproved framework

✓ src-vivado is rewritten  
🗙 corrected functionality in src-vivado (e.g. IMU calibration, IMU reading,
AHRS orientation, clamp RC inputs) needs to be verified  
❓ src is nearly finished  
🗙 the entire framework needs to be debugged  

### Controllers

✓ Controllers simulated  
✓ Code generator working  
❓ Attitude is tuned, but needs fine tuning for new battery  
❓ Altitude is tuned, but needs fine tuning for new battery  
❓ Navigation controller needs tuning  
🗙 Observer for navigation that only uses the accelerometer measurements 
(for taking off/landing when there's no vision data)

### Autopilot

❓ Autonomous navigation has been programmed, it will be tested as soon as we 
finish tuning the navigation controller  
❓ Automatic take-off and landing has been implemented, but not tested yet

### Logger

✓ Code generator working  
❓ Update code generator with new BaremetalImproved  

## Videos for technical milestones T4

### 1. Flying

✓ Stable flight demonstrated for 10s  
✓ Pilot in control of flight moving left, right, forward, backward, up and down

### 2. ESCs

✓ Demonstrate at least one working and integrated ESC. The pilot is able to 
control motor throttle via the remote controller.  
🗙 Flying with own ESCs

### 3. Altitude controller

✓ Flying for 10s while only manually controlling X-Y  
✓ Dito, but with very stable achieved (<30cm variation) at most 20 seconds after 
enabling height control and maintained for at least 20 seconds

### 4. Loitering

🗙 Staying autonomously (more or less) within a 3x3 square grid without manual
x-y control for 10s  
🗙 Long term (>10s) stable location  within a single square, proven by
automatic landing within a square (on the coil)

### 5. Wireless video link

❓ Wireless video link established and link performance analysis done  
🗙 Wireless video link parameters tuned autonomously

### 6. Logging

❓ Real-time logging info during flight  
🗙 Very informative and visually attractive logging, graphically plotting info
at real time during flight

### 7. Navigation (walking)

🗙 Decode current X-Y coordinates in the grid at any square (also those without 
QR code), and decode location targets from QR codes while walking over the grid 
with drone, being correct over at least 10 squares and 2 QR codes. Results 
should show up on GUI  
🗙 Do the same with a hardware accelerated crypto implementation

### 8. Navigation (flying)

🗙 Real-time localization during actual flight  
🗙 Flying/navigating fully autonomously following the QR trail of the mission

### 9. WPT

✓ Capable of fully lighting up the LED wall after manually putting the drone on 
the coil, with on/off activation with the remote control.  
✓ Lighting up the LED wall even when the two coils are aligned only 50%, e.g.
after automated landing.