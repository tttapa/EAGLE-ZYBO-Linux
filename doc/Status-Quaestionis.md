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

🗙 The next step is to write functions that are able to decode strings of data, as
well as more elaborate test cases.

The public API of the crypto code goes in `src/crypto/include/crypto.hpp`, and
the implementations in `src/crypto/src/crypto.cpp`.  
Test cases go in `src/crypto/test/test-crypto.cpp`.

🗙 The reading and writing of the FPGA registers uses the provided code in
`src/comm/src/wrapper.c`, from [Xilinx Application note 1078](https://www.xilinx.com/support/documentation/application_notes/xapp1078-amp-linux-bare-metal.pdf).  
This is not ideal, because it has a lot of overhead when reading large amounts
of data. The code is simple enough, and adding a function for doing longer
transactions to the FPGA is a good idea, instead of calling the `rmem()` and
`wmem()` functions for each word separately.

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

🗙 Next, we need a way to transform the points returned by GridFinder to get the
global location of the drone, relative to the grid. I believe Cyriel finished
the code, but didn't test it yet.
I also added the code to get the orientation of a square. It still needs code to
handle the case where not all four sides of the square are available.

🗙 Finally, this location has to be sent to the BareMetal core.

🗙 When BareMetal indicates that the target position has been reached, the image
should be sent to the QR decoder and then the crypto code needs to be executed.

## ZYBO Baremetal

### Controllers

🗙 Status?

### Autopilot

🗙 Status?

### Logger

🗙 Status?
