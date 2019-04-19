# Install

This is a quick install guide to get the project up and running.

## Docker

The toolchain runs in a Docker container. You need Docker and Docker-Compose to
run it.

### Linux
<https://www.digitalocean.com/community/tutorials/how-to-install-and-use-docker-on-ubuntu-18-04>  
<https://www.digitalocean.com/community/tutorials/how-to-install-docker-compose-on-ubuntu-18-04>
```sh
sudo apt update
sudo apt install apt-transport-https ca-certificates curl software-properties-common
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo apt update
sudo apt install docker-ce
sudo usermod -aG docker $USER
sudo curl -L https://github.com/docker/compose/releases/download/1.21.2/docker-compose-`uname -s`-`uname -m` -o /usr/local/bin/docker-compose
```

### Windows Home
<https://download.docker.com/win/stable/DockerToolbox.exe>

### Windows Pro
<https://docs.docker.com/docker-for-windows/install/>

### OSX
<https://docs.docker.com/docker-for-mac/install/>

## Visual Studio Code

<https://code.visualstudio.com/>
<https://github.com/VSCodium/vscodium> (Open-source version without Microsoft 
Telemetry)

## Setup

When you have installed Docker, Docker-Compose and VSCode, clone this repository
and open the folder in VSCode (`CTRL+K+O`).

Then execute the following tasks by pressing `CTRL+P` and then typing 
`task <task name>` (you don't have to type the entire name, just a few letters
is enough, then use the arrows, and hit enter):
- Build Docker Image (crosstool-NG) - ~40 minutes
- Build Docker Image - ~30 minutes
- Start Docker Container

These commands install all dependencies and build the toolchain (compiler etc.)
and OpenCV.  
You only have to do this once. If you have a slow computer, feel free to just
ask me for the image on a USB stick.

## Building for x86 (Computer)
- Clear 'build' Directory
- Run CMake ARMv7 ZYBO (Docker)
- Build All (Docker) or Run Tests (Docker)

For recompiling afterwards, you only need to execute Build All (Docker) or Run 
Tests (Docker)

## Building for ARMv7 (ZYBO)
- Clear 'build' Directory
- Run CMake ARMv7 ZYBO (Docker)
- Build All (Docker)

When building for ZYBO, you cannot run the tests on your computer.