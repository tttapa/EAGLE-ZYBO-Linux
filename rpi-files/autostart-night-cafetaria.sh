#!/bin/sh
GATEWAY=192.168.2.2
IP=192.168.2.1
NETMASK=255.255.255.0
TARGET=192.168.4.100

echo > nohup.out
echo "Enabling usb ethernet interface" >&2
modprobe dwc2
modprobe libcomposite

mount -t configfs none /sys/kernel/config
 
cd /sys/kernel/config/usb_gadget/
mkdir -p g
cd g
 
echo 0x1d6b > idVendor  # Linux Foundation
echo 0x0104 > idProduct # Multifunction Composite Gadget
echo 0x0100 > bcdDevice
echo 0x0200 > bcdUSB
 
mkdir -p strings/0x409
echo "8498337459872345" > strings/0x409/serialnumber
echo "Bertold"        > strings/0x409/manufacturer
echo "Eagle camera"   > strings/0x409/product

mkdir -p configs/c.1/strings/0x409
echo "Config 1: NCM network" > configs/c.1/strings/0x409/configuration
echo 250 > configs/c.1/MaxPower
 
mkdir -p functions/ncm.usb0  # network

HOST="48:6f:73:74:50:43" # "HostPC"
SELF="42:61:64:55:53:42" # "BadUSB"

echo $HOST > functions/ncm.usb0/host_addr
echo $SELF > functions/ncm.usb0/dev_addr
 
ln -s functions/ncm.usb0 configs/c.1/
 
ls /sys/class/udc > UDC

sleep 2
ifconfig usb0 $IP netmask $NETMASK
route add default gw $GATEWAY 
echo "nameserver $GATEWAY" > /etc/resolv.conf

echo "Starting video capture and stream" >&2
raspivid --fullscreen --irefresh both --awb off -awbg '1.2,0.9' -w 640 -h 480 -b 1500000 --exposure sun -co 0 -br 50 -sa 0 -ISO 300 -ev 5 -fps 60 -vf -hf --metering backlit -t 0 -o - | gst-launch-1.0 -v fdsrc !  h264parse ! rtph264pay config-interval=1 pt=96 ! udpsink host=$TARGET port=5000 &

sh /mnt/net_monitor.sh $GATEWAY &

echo "Autostart.sh finished!" >&2
