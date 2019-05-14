# Configure the network
ifconfig eth0 192.168.3.3
ifconfig eth0 netmask 255.255.255.0
route add default gateway 192.168.3.1
echo "nameserver 192.168.3.1" > /etc/resolv.conf

# Configure the network
# ifconfig eth0 10.42.0.2
# ifconfig eth0 netmask 255.255.255.0
# route add default gateway 10.42.0.1
# echo "nameserver 10.42.0.1" > /etc/resolv.conf

# Configure the network
# ifconfig eth0 192.168.137.200
# ifconfig eth0 netmask 255.255.255.0
# route add default gateway 192.168.137.1
# echo "nameserver 192.168.137.1" > /etc/resolv.conf

# Mount the filesystem containing shared libraries for glibc and opencv
mount -t ext4 -o loop /media/sysroot.ext4 /mnt
ln -s /mnt/lib/ld-linux-armhf.so.3 /lib/
ldconfig

# Configure gdb
ln -s /media/.gdbinit /

# Run the tests
cd /media
source ldpath.sh
# for test in *.test; do
#     ./"$test" --gtest_color=yes
# done
cd -

# Launch the Python framework
cd /media/Python
# python launch.py &
cd -

# Set the SSH host key
ln -s /media/dropbear_rsa_host_key /etc/dropbear/
ln -s /media/dropbear_dss_host_key /etc/dropbear/
ln -s /media/dropbear_ecdsa_host_key /etc/dropbear/

# Add authorized SSH keys
mkdir -p /root/.ssh
for key in /media/*.pub; do
  cat $key >> /root/.ssh/authorized_keys
done

# Automatically set the LD path
echo "source /media/ldpath.sh" >> /etc/profile

# Enable colors for the terminal
export TERM='xterm-256color'
