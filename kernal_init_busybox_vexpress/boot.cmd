fatload mmc 0:1 0x60008000 zImage
fatload mmc 0:1 0x61000000 vexpress-v2p-ca9.dtb
fatload mmc 0:1 0x62000000 rootramfs.cpio

# Use the filesize variable from the fatload above
setenv ramdisk_size ${filesize}

# Change rdinit to /init (the link we just made)
setenv bootargs "console=ttyAMA0 rdinit=/init"

bootz 0x60008000 0x62000000:${ramdisk_size} 0x61000000
