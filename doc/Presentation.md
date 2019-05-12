# Presentation

## Things to brag about

### Baremetal

We have completely re-written the original C code for the drone controller. 
We're using a **modern GCC toolchain**, instead of the outdated version that came 
with the Xilinx SDK. Using modern **C++** allowed us to write much **cleaner, easier
to read code with fewer bugs**. The most important bug fix was the **sensor system**.
Large mathematical errors were not seen by the authors, which caused the measurement
orientation to drift. Now, the IMU calibration and AHRS update are mathematically
correct, and the system is equipped with a beefy Madgwick sensor-fusion filter, so
**there is absolutely no drift**. Furthermore, many small bugs from the original code
have been fixed, and **extra structure and functionality** has been added. The structure
of the code has been improved in the several ways.

 - **DOCUMENTATION!!! For the love of god...**.
 - **NO MORE GLOBALS BEING WRITTEN & READ ALL OVER THE CODE!** The flow of data can
 clearly be followed in *MainInterrupt.cpp*.
 - **Separation of src-vivado/ and src/**. The first directory contains code to start
 the Xilinx platform, read from inputs (IMU/AHRS, RC, SONAR) and write to outputs
 (BUZZER, MOTORS, WPT).
 - **Class representation** of the three controllers (ATTITUDE, ALTITUDE, POSITION);
 - **Clear distinction between controllers flight modes** (MANUAL, ALTITUDE-HOLD,
 AUTONOMOUS).
 - Struct representation of input measurements, control system types and output signals;
  - **Autonomous-FSM** using the states IDLE_GROUND, PRE_TAKEOFF, TAKEOFF, LOITERING,
 NAVIGATING, CONVERGING, LANDING, WPT and ERROR. If autonomous mode is started during
 flight, then the drone will begin in the LOITERING state. Otherwise, it will begin in
 the IDLE_GROUND state.
 - **QR-FSM** to facilitate the communication between ANC, IMP and CRYPTO when
 decrypting QR codes. This includes the states IDLE, QR_READ_REQUEST, QR_READ_BUSY,
 NEW_TARGET, QR_LAND, QR_UNKNOWN, and ERROR.

Some aspects of the assignment have been extended. Firstly, during altitude-hold mode,
the pilot is able to adjust the reference height with the RC throttle. A more significant
change is the **rework of the autonomous mode**. Aside from switching to autonomous mode
after stable altitude-hold flight, **the pilot now has the option to start this mode from
the ground**. If the drone is grounded and in autonomous mode, then the pilot can start
autonomous flight by raising the RC throttle. The drone will perform a pre-takeoff routine
(ESC script for example), and then **it will take off autonomously**. As soon as
it reaches the reference height, it will begin loitering. After that, it will follow the
QR trail and automatically land. The pilot can then flip the WPT switch to power the LED
wall.

In addition, many extra functionalites have been added to the framework.

 - **Adjustable IMU configuration**. The frequency of the IMU, and therefore the entire
 framework, can be set to 119, 238, 476 or 952 Hz. The resolution of the IMU can also
 be adjusted: the maximum value of the gyroscope can be set to 245, 500 or 2000 deg/s,
 and that of the accelerometer can be set to 2, 4, 8, or 16 g.
 - The drone comes equipped with **five controller configurations**. Using the tuner
 knob, the pilot can change the controller configuration without having to reboot the
 Zybo. This allows for rapid testing of various parameters and **calibration of the ESCs**,
 which is set to configuration 5.
 - If the hardware team requires special PWM signals or timings to start properly, the
 framework is provided to add a **custom ESC startup script**.
 - **Gradual thrust change** prevents rapid thrust change when switching from altitude-
 hold mode to manual mode, or when the ESC startup script has finished and the drone
 assumes the thrust provided by the RC.
 - The **buzzer** now plays sounds for initiation, arming, disarming, controller
 configuration, Autonomous/QR error states (TODO) and WPT (TODO).


// TODO: created or used? \
We used a complete cross-compilation toolchain with a cross-platform build 
system, so we can develop and run unit tests on our computer.

We have done **performance checks** on the new and old programs, and we can confirm
that after switching to C++, our program still runs at exactly the same speed, 
and the increase in readability and maintainability is huge. The rework has even
allowed us to run the IMU at 952 Hz instead of 238 Hz. This resulted in a better
orientation estimate and a more stable control system.

The **controller code is automatically generated** using a MATLAB script, and the
boot image can be immediately copied to the drone using `SCP`. Then the Zybo is
rebooted over `SSH`. Before doing this, we had to unplug the battery, remove the SD
card, run to a computer, copy the new boot image, run back to the drone, insert the
SD card, plug in the battery, wait for the Zybo to boot and finally wait for the
WiFi router to boot (5 minutes). **This has improved our process flow so much**.


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

