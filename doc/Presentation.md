# Presentation

## Things to brag about

### Baremetal

We have completely re-written the original C code for the drone controller. 
We're using a **modern GCC toolchain**, instead of the outdated version that came 
with the Xilinx SDK. Using modern **C++** allowed us to write much **cleaner, easier
to read code, with fewer bugs**. The most important bug fix was the **sensor system**.
Large mathematical errors were not seen by the authors, and because of this the
orientation measurement finished. **Many small bugs from the original code are now
fixed** and **extra structure and functionality** has been added. The
we fixed and some mathematical errors were somehow not seen by the original authors.were some **bugs and mathematical errors** in 
the sensor system that we fixed.

We used a complete cross-compilation toolchain with a cross-platform build 
system, so we can develop and run unit tests on our computer.

We have done **performance checks** on the new and old programs, and we can confirm
that after switching to C++, our program still runs at exactly the same speed, 
and the increase in readability and maintainability is huge.

We're using a different **sensor fusion algorithm** for the IMU, that performs much 
better than the given code.

We are now able to run the IMU input attitude controller at almost **1 kHz**
instead of the original 238 Hz, so we get less drift, and a more stable 
controller.

The **controller code is automatically generated** using a MATLAB script that allows
us to **easily tune any parameter**. However, uploading the new code to the drone 
was a tedious task (creating a boot image in Vivado on an ESAT PC, turn off the
drone, take out the SD card, run to the computer with it, copy the boot image to
the SD card, safely remove, run back to the drone, plug it in, wait for it to
boot, etc.)

Our solution to this problem is two-fold: 1) use a faster way to upload new code
2) tune without having to re-upload code.

For **faster uploads**, we use **SSH**. This means that we don't need physical access to
the drone, and we don't have to unplug the battery. We just use `SCP` to upload
the boot image, and then reboot the ZYBO over `SSH`. Because we don't unplug the
battery, we don't have to wait over five minutes for the WiFi router to boot.

We also added **multiple slots of controllers** to our code generator. So we can 
try multiple controllers by changing the configuration using the tune knob on 
the remote controller. This means that we don't have to re-upload for every
tuning setting we want to try. 

### Vision

The first approach used the Hough transform in OpenCV Python. However, this was
too slow, so the algorithm skipped some squares when moving or tilting too fast.
A second approach just counted the number of red pixels on each row and each
column. This was a bit faster, but it only worked if the drone didn't rotate
about the z-axis. This algorithm still occasionally skipped a square, making it
not suitable for a loitering controller. We tried speeding things up even more
by down-sampling the images, but no avail.

Another problem for vision was the given Python framework. It had some serious
issues, such as deadlock between the vision and logger thread, it didn't release
the video input when something went wrong, and it degraded the performance even
further.

After countless nights of trying to get the drone to loiter, we decided to start
from scratch. We threw out the Python framework in favor of a much more 
light-weight C++ application, and we created a very ingenious grid finder
algorithm in C++ as well.

For development of the vision algorithm on the PC, we used pybind11. This 
allowed us to use our C++ algorithm in Jupyter notebooks in Python, having 
access to all the benefits of a scripting language, and modules like matplotlib
and OpenCV for easy debugging, while still keeping super fast C++ performance.

Once we had the algorithm figured out, and we could show it working on the given
videos, we started setting up a cross-compilation toolchain for the ZYBO.  
We decided to use a Docker build container, so it could be used by all team
members, regardless of the operating system they were using.
The toolchain consists of a custom GCC build for the ZYBO, with cross-compiled 
versions of the OpenCV and ZBar libraries. Native x86 versions of all tools are
available as well, so we can do the initial development on the computer, without
requiring access to the ZYBO.  
All unit tests (that don't require platform-specific features like the FPGA) are
run natively in the Docker container as well.  
The entire toolchain is tightly integrated with Visual Studio Code: there are 
shortcuts for quickly building the entire project for ARM or for x86, for 
running the unit test on the PC or on the ZYBO, for getting a shell on the ZYBO,
for starting a debug session on the ZYBO, etc.  
CMake ensures that all libraries and binaries are up to date, and they are 
automatically synced with the drone over SSH.  
For unit testing, the Google Test framework is used. Automatic documentation 
generation is handled by Doxygen. A Python Jupyter environment and Python 
bindings for the C++ libraries we wrote are available as well, for easy 
visualization of the vision algorithm. All of this is contained within the 
Docker container.

Once we got our grid finder algorithm working on the ZYBO, it became clear that
we had to speed up the masking as well: the new algorithm to find the lines
ran at 800 fps, but the masking (detecting the red pixels in the image) only
ran at 50 fps. We got rid of the OpenCV functions for masking, and wrote our own
version in C++. We're using the ARM NEON SIMD vector unit of the ZYBO, using
intrinsics, meaning that we can mask 16 pixels in parallel. At the full
resolution of 640×480, the masking now runs at almost 1000 fps.

All of this means that we're able to update the position measurement of the 
drone at 60 fps, which is the refresh rate of the HDMI input. This enables us
to design better controllers, and ensures that we don't skip any squares, even 
when moving very quickly.

The grid finder algorithm is very robust. It can handle cases where certain 
parts are overexposed by direct sunlight, gaps in the lines, curved lines due
to lens distortion, and it is mathematically correct even when turning the drone
360° around the z-axis.

[Video sunlight](drone-images-sunlight+mask.out.mp4)  
[Video 360°](DroneCam-Spinning.out.mp4)

The final Linux application consists of three main parts: the vision algorithm
for the localization, the QR decoding using ZBar and the algorithm developed by
our crypto team, that runs on the FPGA, and the data logger for ANC.

Decoding the QR code happens asynchronously, so it doesn't interrupt the vision
algorithm (it does slow it down slightly). The logger doesn't really require any
processing, it just reads whatever it finds in the shared memory from ANC, and 
sends it over the network as UDP Multicast. It runs in a separate thread, and
runs periodically.

