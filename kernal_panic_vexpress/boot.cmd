fatload mmc 0:1 0x60008000 zImage
fatload mmc 0:1 0x61000000 vexpress-v2p-ca9.dtb
setenv bootargs "console=ttyAMA0 root=/dev/mmcblk0p2 rw rootfstype=ext4"
bootz 0x60008000 - 0x61000000
