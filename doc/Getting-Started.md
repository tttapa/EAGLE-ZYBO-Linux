# Getting Started

This is a quick install guide to get the project up and running, and some
instructions on how to build, debug and run code and tests.

## Installation

### Docker

The toolchain runs in a Docker container. You need Docker and Docker-Compose to
run it.

#### Linux

<https://www.digitalocean.com/community/tutorials/how-to-install-and-use-docker-on-ubuntu-18-04>  
<https://www.digitalocean.com/community/tutorials/how-to-install-docker-compose-on-ubuntu-18-04>

```sh
sudo apt update
sudo apt install apt-transport-https ca-certificates curl software-properties-common
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu bionic stable"
sudo apt update
sudo apt install docker-ce
sudo usermod -aG docker $USER
sudo curl -L https://github.com/docker/compose/releases/download/1.21.2/docker-compose-`uname -s`-`uname -m` -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
```

Then logout and back in again.

#### Windows Home

<https://download.docker.com/win/stable/DockerToolbox.exe>

#### Windows Pro

<https://docs.docker.com/docker-for-windows/install/>

#### OSX

<https://docs.docker.com/docker-for-mac/install/>

### Visual Studio Code

<https://code.visualstudio.com/>  
<https://github.com/VSCodium/vscodium> (Open-source version without Microsoft
telemetry)

Also install the Docker and C/C++ extensions by Microsoft (`CTRL+SHIFT+X`).

## Setup

When you have installed Docker, Docker-Compose and VSCode, clone this repository
and open the folder in VSCode (`CTRL+K+O`). Make sure to clone using SSH, not
HTTPS.

Then execute the following tasks by pressing `CTRL+P` and then typing
`task <task name>` (you don't have to type the entire name, just a few letters
is enough, then use the arrows, and hit enter):

- Build Docker Image (crosstool-NG) - ~40 minutes
- Build Docker Image - ~35 minutes
- Start Docker Container

These commands install all dependencies and build the toolchain (compiler etc.)
and OpenCV.  
You only have to do this once.  
The first two commands took 40 and 35 minutes on my i7-7700HQ, and 70 and 60
minutes on my i7-3630QM. If you have a slow computer, feel free to just
ask me for the image on a USB stick.

## Building for x86 (Computer)

- Clear 'build' Directory x86-64 - optional
- Run CMake x86-64 (Docker)
- Build All x86-64 (Docker)

For recompiling afterwards, you only need to execute Build All (Docker).

## Running the tests

- Run Tests (Docker)

This task will automatically compile any files that have changed.

## Building for ARMv7 (ZYBO)

- Clear 'build' Directory ARMv7 ZYBO - optional
- Run CMake ARMv7 ZYBO (Docker)
- Build All ARMv7 ZYBO (Docker)

When building for ZYBO, you cannot run the tests on your computer.

## Debugging

The easiest way to debug is running it in Linux. If this is not possible
(if you have library conflicts, or if you are running an inferior operating
system), you can debug in the Docker container.

Press `CTRL+SHIFT+D` and select either Launch C++ Docker Debug or Launch C++
Local Debug. If you select the Docker version, it will automatically launch the
GDB Server in the container.

To select the application to debug, you have to replace the executable path in
the Start GDB Server task in `tasks.json` and the configurations in
`launch.json`.

When debugging in the Docker container, you don't have access to the standard
input.  
If you want access to the standard output, you can run the Start GDB Server task
before launching the debug session.

On Windows, VSCode doesn't support GDB, so you'll have to use a different front-
end, like [gdbgui](https://gdbgui.com/).

## Running

You can run the application in the Docker container by running the Run
Application (Docker) task.  
You can change the application to run in `tasks.json`.

## Generating the Documentation

- Generate Documentation (Docker)

The documentation is generated in `doc/html`.

## Writing Code

Check the @ref Structure.md page for information about the folder structure of
the project.

For example code, have a look at `src/greet` and `src/crypto`.