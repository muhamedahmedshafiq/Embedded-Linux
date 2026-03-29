# 🐧 Build & Boot Linux Kernel on QEMU vexpress-A9 (ARM32)
### Goal: Reach `Kernel panic - not syncing: No working init found` 🏆

---

## 📋 Table of Contents
1. [Install Dependencies](#1-install-dependencies)
2. [Set Environment Variables](#2-set-environment-variables)
3. [Clone Linux Kernel](#3-clone-linux-kernel)
4. [Add Your Signature](#4-add-your-signature)
5. [Configure Kernel](#5-configure-kernel)
6. [Build Kernel](#6-build-kernel)
7. [Create Virtual SD Card](#7-create-virtual-sd-card)
8. [Verify SD Card Contents](#8-verify-sd-card-contents)
9. [Copy Files to SD Card](#9-copy-files-to-sd-card)
10. [Build U-Boot](#10-build-u-boot)
11. [Configure U-Boot Environment Storage](#11-configure-u-boot-environment-storage)
12. [What is boot.cmd?](#12-what-is-bootcmd)
13. [Create Boot Script](#13-create-boot-script)
14. [Boot QEMU](#14-boot-qemu)
15. [Boot Kernel from U-Boot](#15-boot-kernel-from-u-boot)
16. [U-Boot Environment — setenv and saveenv](#16-u-boot-environment--setenv-and-saveenv)
17. [Common Errors and Fixes](#17-common-errors-and-fixes)

---

## 1. Install Dependencies

```bash
sudo apt update
sudo apt install -y gcc-arm-linux-gnueabi make bc bison flex \
    libssl-dev libelf-dev qemu-system-arm git wget u-boot-tools
```

| Package | Why We Need It |
|---|---|
| `gcc-arm-linux-gnueabi` | ARM cross-compiler (soft-float) to build kernel on your x86 laptop |
| `make` | Build system to compile the kernel |
| `bc` | Math tool required by kernel build scripts |
| `bison` / `flex` | Parser tools required by kernel configuration |
| `libssl-dev` | SSL library needed to sign kernel modules |
| `libelf-dev` | ELF library needed by kernel build |
| `qemu-system-arm` | ARM emulator to run our kernel virtually |
| `u-boot-tools` | Provides `mkimage` to create boot scripts |

> ⚠️ Use `gcc-arm-linux-gnueabi` (soft-float) for vexpress-A9 on QEMU.
> `gcc-arm-linux-gnueabihf` (hard-float) is for real hardware like Raspberry Pi.

---

## 2. Set Environment Variables

```bash
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
```

**What this means:**

| Variable | Value | Meaning |
|---|---|---|
| `ARCH` | `arm` | Tell the kernel build system we are targeting ARM 32-bit |
| `CROSS_COMPILE` | `arm-linux-gnueabi-` | Prefix for all cross-compiler tools (gcc, ld, objcopy...) |

**Why we need them:**

We are building on an x86_64 laptop but the kernel will run on ARM.
Without these, `make` would try to use your laptop's native gcc and
build an x86 kernel instead of an ARM kernel.

> ⚠️ These exports only last for the current terminal session.
> If you close the terminal, export them again.
> You must set these BEFORE running any `make` command — for both the kernel AND U-Boot.

**Verify the compiler works:**
```bash
${CROSS_COMPILE}gcc --version
# Expected: arm-linux-gnueabi-gcc (Ubuntu 11.4.0) 11.4.0
```

---

## 3. Clone Linux Kernel

```bash
git clone --depth=1 --branch v6.6.15 \
    https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git \
    linux-kernel

cd linux-kernel
```

| Flag | Meaning |
|---|---|
| `--depth=1` | Download only the latest commit, not full history (saves time and disk) |
| `--branch v6.6.15` | Use stable version 6.6.15 instead of latest unstable code |

---

## 4. Add Your Signature

```bash
nano Makefile
```

Find these lines near the top of the file:
```makefile
VERSION = 6
PATCHLEVEL = 6
SUBLEVEL = 15
EXTRAVERSION =
```

Change `EXTRAVERSION` to include your name:
```makefile
EXTRAVERSION = -YourName-v1
```

**Why?**

This makes `uname -r` output `6.6.15-YourName-v1`.
It is one of the lab requirements — your kernel must show your identity.
You will see this version string in the boot log:
```
Linux version 6.6.15-YourName-v1 ...
```

---

## 5. Configure Kernel

```bash
make vexpress_defconfig
```

**What this does:**

- Loads a pre-made configuration file from `arch/arm/configs/vexpress_defconfig`
- This config has all the right drivers enabled for the ARM Versatile Express board
- QEMU emulates this exact board with `-M vexpress-a9`
- Creates a `.config` file in the kernel root directory

**Why not `make defconfig`?**

`defconfig` gives a generic ARM config. `vexpress_defconfig` is specifically
tuned for the vexpress-A9 board that QEMU emulates — it has the right
display, UART, MMC, and ethernet drivers pre-enabled.

**Optional — Open menuconfig to explore:**
```bash
make menuconfig
# Navigate with arrow keys, Space to toggle, / to search, Q to quit
```

---

## 6. Build Kernel

```bash
make -j$(nproc) zImage dtbs
```

| Part | Meaning |
|---|---|
| `-j$(nproc)` | Use all CPU cores in parallel — `nproc` returns your core count (e.g. 8) |
| `zImage` | Build only the compressed kernel image for ARM32 |
| `dtbs` | Build only the Device Tree Blobs for all supported boards |

**Why `zImage` and not just `make`?**

Plain `make` builds everything including all kernel modules (`.ko` files)
which takes 2-3x longer. We only need `zImage` and `dtbs` to boot.

**Expected output at end of build:**
```
Kernel: arch/arm/boot/zImage is ready
```

**Verify build output:**
```bash
ls -lh arch/arm/boot/zImage
ls -lh arch/arm/boot/dts/arm/vexpress-v2p-ca9.dtb
```

**What these files are:**

| File | Description |
|---|---|
| `zImage` | Compressed kernel image — what gets loaded into RAM and executed |
| `vexpress-v2p-ca9.dtb` | Device Tree Blob — describes the hardware layout to the kernel |

---

## 7. Create Virtual SD Card

```bash
# Create a 1GB empty image file
dd if=/dev/zero of=sd.img bs=1M count=1024
```

| Part | Meaning |
|---|---|
| `if=/dev/zero` | Input from `/dev/zero` — an infinite source of zero bytes |
| `of=sd.img` | Output file — our virtual SD card |
| `bs=1M` | Block size = 1 Megabyte |
| `count=1024` | Write 1024 blocks = 1024 MB = 1 GB |

**Partition the SD card:**
```bash
fdisk sd.img
```

Inside `fdisk` type these in order:
```
o          # Create new DOS partition table
n          # New partition
p          # Primary partition
1          # Partition number 1
2048       # First sector (leave space for partition table)
+100M      # Size: 100MB for boot partition
t          # Change partition type
b          # W95 FAT32 (for boot files)
a          # Mark as bootable
n          # New partition
p          # Primary
2          # Partition number 2
           # Press Enter (use default first sector)
           # Press Enter (use rest of disk for rootfs)
w          # Write changes and exit
```

**Verify partitioning was successful:**
```bash
fdisk -l sd.img
```

Expected output:
```
Device     Boot  Start     End Sectors  Size Id Type
sd.img1    *      2048  206847  204800  100M  b W95 FAT32
sd.img2         206848 2097151 1890304  923M 83 Linux
```

✅ Two partitions, p1 = FAT32 (type `b`), p2 = Linux (type `83`), p1 has boot flag (`*`).

**Attach and format:**
```bash
# Attach sd.img as a loop device (makes it act like a real disk)
sudo losetup --find --partscan sd.img

# Check which loop device was assigned to YOUR sd.img
sudo losetup -l | grep sd.img

# Format partition 1 as FAT32 (boot partition — U-Boot can read FAT)
sudo mkfs.vfat -F 32 -n BOOT /dev/loop18p1   # replace loop18 with your number

# Format partition 2 as ext4 (rootfs partition)
sudo mkfs.ext4 -L rootfs /dev/loop18p2        # replace loop18 with your number
```

> ⚠️ If mkfs.ext4 asks "Proceed anyway? (y,N)" — type `y`.
> This just means the partition was formatted before. It is safe to reformat.

**Why two partitions?**

| Partition | Format | Contains | Who reads it |
|---|---|---|---|
| `mmcblk0p1` | FAT32 | zImage, DTB, boot.scr, uboot.env | U-Boot (only understands FAT) |
| `mmcblk0p2` | ext4 | rootfs (future lab) | Linux kernel |

---

## 8. Verify SD Card Contents

### How to find your loop device number

Every time you attach `sd.img`, the system assigns a loop number automatically.
It will NOT always be the same number. Always check first:

```bash
sudo losetup --find --partscan sd.img
sudo losetup -l | grep sd.img
```

Example output:
```
/dev/loop18   0   0   0  0  /home/muhamed/ITI/ITI_FADY/LINUX/vexpress/sd.img   0   512
```

Your loop device is `/dev/loop18` in this example. Use this number for all mount commands.

### Check partition 1 (FAT32 — boot files)

```bash
sudo mkdir -p /mnt/boot
sudo mount /dev/loop18p1 /mnt/boot    # use your actual loop number
ls -lh /mnt/boot/
sudo umount /mnt/boot
```

Expected after all files are copied:
```
total 5.8M
-rwxr-xr-x 1 root root  257  boot.scr
-rwxr-xr-x 1 root root  14K  vexpress-v2p-ca9.dtb
-rwxr-xr-x 1 root root 5.7M  zImage
-rwxr-xr-x 1 root root  xxx  uboot.env    ← appears after first saveenv
```

### Check partition 2 (ext4 — rootfs)

```bash
sudo mkdir -p /mnt/rootfs
sudo mount /dev/loop18p2 /mnt/rootfs   # use your actual loop number
ls -lh /mnt/rootfs/
sudo umount /mnt/rootfs
```

Right now it will only show:
```
total 16K
drwx------ 2 root root 16K  lost+found
```

This is normal — ext4 always creates `lost+found`. Rootfs is empty until Lab 07.

### Verify partition formatting

```bash
sudo file -s /dev/loop18p1
# Expected: DOS/MBR boot sector ... FAT (32 bit)

sudo file -s /dev/loop18p2
# Expected: Linux rev 1.0 ext4 filesystem data
```

### Quick check without mounting (no sudo needed)

```bash
sudo apt install mtools
mdir -i sd.img@@1M    # lists FAT32 contents directly from sd.img
```

`@@1M` means offset = 2048 sectors × 512 bytes = 1MB (where partition 1 starts).

### Always detach when done

```bash
sudo losetup -d /dev/loop18    # use your actual loop number
```

> ⚠️ If you forget to detach and run `losetup --find --partscan sd.img` again,
> a NEW loop device will be created. You can have multiple loops pointing to the
> same file. Always check `sudo losetup -l | grep sd.img` to see all of them
> and detach all with `sudo losetup -d /dev/loopX` for each one.

---

## 9. Copy Files to SD Card

```bash
# Attach sd.img
sudo losetup --find --partscan sd.img
sudo losetup -l | grep sd.img    # find your loop number

# Mount the boot partition
sudo mkdir -p /mnt/boot
sudo mount /dev/loop18p1 /mnt/boot    # use your actual loop number

# Copy kernel image
sudo cp arch/arm/boot/zImage /mnt/boot/

# Copy Device Tree Blob
sudo cp arch/arm/boot/dts/arm/vexpress-v2p-ca9.dtb /mnt/boot/

# Verify files are there
ls -lh /mnt/boot/

# Unmount and detach when done
sudo umount /mnt/boot
sudo losetup -d /dev/loop18    # use your actual loop number
```

**What `/mnt/boot` is:**

It is a temporary mount point on your laptop.
When you mount the loop device there, you can access the SD card
partition as if it were a normal folder.
Anything you copy there goes directly into `sd.img`.

```
sd.img  →  /dev/loop18  →  /mnt/boot/  (your view into the SD card)
```

---

## 10. Build U-Boot

> ⚠️ CRITICAL: You MUST export `ARCH` and `CROSS_COMPILE` before building U-Boot.
> Forgetting this causes the build to fail with `-mtune=generic-armv7-a` errors
> because it uses your host x86 compiler instead of the ARM cross-compiler.

```bash
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

git clone --depth=1 https://github.com/u-boot/u-boot.git
cd u-boot

# Configure for vexpress-A9
make vexpress_ca9x4_defconfig

# Build
make -j$(nproc)
```

**What U-Boot is:**

U-Boot is a bootloader — it runs before the kernel.
Its job is to initialize hardware and then load and start the kernel.

```
Power on → U-Boot starts → reads boot.scr → loads kernel → jumps to kernel
```

**Why `vexpress_ca9x4_defconfig`?**

This config matches the exact board QEMU emulates: ARM Versatile Express with Cortex-A9 x4.

**Output:**
```
u-boot      ← ELF binary (this is what we pass to QEMU with -kernel)
u-boot.bin  ← Raw binary
```

**Verify:**
```bash
ls -lh u-boot
# Should show ~5MB ELF binary
```

---

## 11. Configure U-Boot Environment Storage

By default, `vexpress_ca9x4_defconfig` saves the U-Boot environment to
**emulated NOR flash** — which is lost every time QEMU exits.

To make `saveenv` persist across QEMU sessions, configure U-Boot to store
the environment in a file (`uboot.env`) on the FAT32 partition of `sd.img`.

### Flash vs FAT — which to use for QEMU?

| | Flash (default) | FAT (sd.img) |
|---|---|---|
| Real hardware | ✅ Perfect | ✅ Works |
| QEMU | ❌ Lost on exit | ✅ Persists to disk |
| Needs SD card | ❌ No | ✅ Yes |
| Good for this lab | ❌ No | ✅ Yes |

Flash on QEMU = writing to RAM. When QEMU exits, RAM is gone. Nothing is saved to disk.
FAT on QEMU = writing to `sd.img`. That file lives on your laptop disk. It survives reboots.

### How to configure

```bash
cd ~/ITI/ITI_FADY/u_boot/u-boot
make menuconfig
```

Navigate to `Environment` and make these changes:

```
[ ] Environment in flash memory        ← disable this
[*] Environment is in a FAT filesystem ← enable this
```

Press Enter on the FAT option to set sub-options:
```
(mmc)       Name of the block device for the environment
(0:1)       Device and partition for where to store the environment in FAT
(uboot.env) Name of the FAT file to use for the environment
```

Save and exit, then rebuild:
```bash
make -j$(nproc)
```

### Verify the config is correct

```bash
grep -i "ENV_FAT" .config
```

Expected:
```
CONFIG_ENV_IS_IN_FAT=y
CONFIG_ENV_FAT_INTERFACE="mmc"
CONFIG_ENV_FAT_DEVICE_AND_PART="0:1"
CONFIG_ENV_FAT_FILE="uboot.env"
```

---

## 12. What is boot.cmd?

`boot.cmd` is a plain text file containing U-Boot commands — like a shell script
but for U-Boot instead of bash.

U-Boot cannot read plain text files directly. It needs a binary format with a
special header. So we write `boot.cmd` as human-readable text, then convert it
to `boot.scr` using `mkimage`.

```
boot.cmd  →  mkimage adds header  →  boot.scr
 plain text                           binary U-Boot understands
```

Only `boot.scr` goes on the SD card. `boot.cmd` is just your source file.

### What each line does

```bash
fatload mmc 0:1 0x60008000 zImage
```
| Part | Meaning |
|---|---|
| `fatload` | Read a file from a FAT32 partition |
| `mmc 0:1` | SD card 0, partition 1 (our FAT32 boot partition) |
| `0x60008000` | Load it into RAM at this address |
| `zImage` | The file to load |

Think of it as: **"Copy zImage from SD card → RAM at address 0x60008000"**

---

```bash
fatload mmc 0:1 0x61000000 vexpress-v2p-ca9.dtb
```

Same as above but loads the DTB into RAM at a different address so it does not
overlap with zImage.

---

```bash
setenv bootargs "console=ttyAMA0 root=/dev/mmcblk0p2 rw rootfstype=ext4"
```

Sets the kernel command line — arguments passed to the Linux kernel at boot:

| Argument | Meaning |
|---|---|
| `console=ttyAMA0` | Print kernel messages to UART serial port (visible in your terminal) |
| `root=/dev/mmcblk0p2` | The rootfs is on SD card partition 2 |
| `rw` | Mount rootfs as read-write |
| `rootfstype=ext4` | It is formatted as ext4 |

---

```bash
bootz 0x60008000 - 0x61000000
```

| Part | Meaning |
|---|---|
| `bootz` | Boot command for ARM32 zImage |
| `0x60008000` | Address in RAM where zImage was loaded |
| `-` | No initrd (we do not have one yet) |
| `0x61000000` | Address in RAM where DTB was loaded |

This is the line that hands control from U-Boot to the Linux kernel.

### Why `bootz` and not `bootm` or `booti`?

| Command | Used for |
|---|---|
| `bootz` | ARM32 `zImage` — our case |
| `booti` | ARM64 `Image` — Raspberry Pi 3B+ |
| `bootm` | Legacy `uImage` format |

---

## 13. Create Boot Script

**Create the script text file:**
```bash
cat > boot.cmd << 'EOF'
fatload mmc 0:1 0x60008000 zImage
fatload mmc 0:1 0x61000000 vexpress-v2p-ca9.dtb
setenv bootargs "console=ttyAMA0 root=/dev/mmcblk0p2 rw rootfstype=ext4"
bootz 0x60008000 - 0x61000000
EOF
```

**Convert to U-Boot binary format:**
```bash
mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "Boot Script" -d boot.cmd boot.scr
```

| mkimage flag | Meaning |
|---|---|
| `-A arm` | Target architecture = ARM |
| `-O linux` | Operating system = Linux |
| `-T script` | Image type = Script (U-Boot script) |
| `-C none` | No compression |
| `-d boot.cmd` | Input file (our text script) |
| `boot.scr` | Output file (binary format U-Boot understands) |

Expected output:
```
Image Name:   Boot Script
Image Type:   ARM Linux Script (uncompressed)
Data Size:    172 Bytes
```

**Copy boot.scr to SD card:**
```bash
sudo losetup --find --partscan sd.img
sudo losetup -l | grep sd.img          # find your loop number
sudo mount /dev/loop18p1 /mnt/boot     # use your actual loop number
sudo cp boot.scr /mnt/boot/

# Verify all 3 files are present
ls -lh /mnt/boot/
# Must show: zImage, vexpress-v2p-ca9.dtb, boot.scr

sudo umount /mnt/boot
sudo losetup -d /dev/loop18            # use your actual loop number
```

---

## 14. Boot QEMU

```bash
qemu-system-arm \
  -M vexpress-a9 \
  -kernel ~/ITI/ITI_FADY/u_boot/u-boot/u-boot \
  -nographic \
  -m 512M \
  -drive file=sd.img,if=sd,format=raw
```

| Flag | Meaning |
|---|---|
| `-M vexpress-a9` | Emulate the ARM Versatile Express A9 board |
| `-kernel u-boot` | Load U-Boot as the first program to run (not the Linux kernel directly) |
| `-nographic` | No GUI window — all serial output goes to your terminal |
| `-m 512M` | Give the virtual machine 512MB of RAM |
| `-drive file=sd.img,if=sd,format=raw` | Attach our virtual SD card |

**`-drive` flag breakdown:**

| Part | Meaning |
|---|---|
| `file=sd.img` | Path to our SD card image file on the laptop |
| `if=sd` | Interface = SD card (kernel sees it as `/dev/mmcblk0`) |
| `format=raw` | Raw binary format (what `dd` created) |

> ⚠️ To exit QEMU at any time: press `Ctrl+A` then `X`

> ⚠️ The `pulseaudio` warnings you may see are harmless — QEMU trying to use
> audio which we don't need. Ignore them.

---

## 15. Boot Kernel from U-Boot

If `boot.scr` is on the SD card, U-Boot finds and runs it automatically.

**If you need to boot manually** (press any key to stop autoboot first):

```bash
# Load kernel into RAM
fatload mmc 0:1 0x60008000 zImage

# Load DTB into RAM
fatload mmc 0:1 0x61000000 vexpress-v2p-ca9.dtb

# Set boot arguments
setenv bootargs "console=ttyAMA0 root=/dev/mmcblk0p2 rw rootfstype=ext4"

# Boot the kernel
bootz 0x60008000 - 0x61000000
```

**RAM Load Addresses for vexpress-A9:**

| Item | Address | Why this address |
|---|---|---|
| `zImage` | `0x60008000` | Standard ARM Linux load address |
| DTB | `0x61000000` | After kernel, with enough gap |
| Initrd | `0x62000000` | After DTB (not used in this lab) |

**Save bootcmd to run automatically on every boot:**
```bash
setenv bootcmd "fatload mmc 0:1 0x60008000 zImage; fatload mmc 0:1 0x61000000 vexpress-v2p-ca9.dtb; setenv bootargs 'console=ttyAMA0 root=/dev/mmcblk0p2 rw rootfstype=ext4'; bootz 0x60008000 - 0x61000000"
saveenv
run bootcmd
```

---

## 16. U-Boot Environment — setenv and saveenv

### What is the U-Boot environment?

The U-Boot environment is a set of key=value variables stored in non-volatile memory.
They control how U-Boot behaves at boot — what to load, where to load it, what
arguments to pass to the kernel.

```
printenv          ← show all variables
printenv bootcmd  ← show one variable
setenv myvar hello ← set a variable (RAM only, lost on reboot)
saveenv            ← write all variables to storage (persists across reboots)
```

### Where are variables saved?

| Storage | QEMU behavior | Real hardware behavior |
|---|---|---|
| Flash (default) | ❌ Lost when QEMU exits | ✅ Persists on NOR flash chip |
| FAT (uboot.env) | ✅ Persists in sd.img | ✅ Persists on SD card |

For QEMU you must configure FAT storage (see Section 11) to make `saveenv` work.

### Testing saveenv

After configuring FAT storage and rebuilding U-Boot:

```
=> setenv hello mooo
=> saveenv
Saving Environment to FAT... OK    ← must see OK, not "No device specified"
=> reset
```

After reboot:
```
=> printenv hello
hello=mooo                         ← variable survived the reboot ✅
```

### Verify uboot.env was created on the SD card

```bash
sudo losetup --find --partscan sd.img
sudo mount /dev/loop18p1 /mnt/boot
ls -lh /mnt/boot/
# Should now show uboot.env alongside the other files
sudo umount /mnt/boot
sudo losetup -d /dev/loop18
```

### Do you need saveenv for kernel panic?

**No.** If `boot.scr` is on the SD card, U-Boot boots the kernel automatically
without any `saveenv`. The environment only matters if you want to customize
boot behavior and have it persist between QEMU sessions.

---

## 17. Common Errors and Fixes

### `arm-linux-gnueabi-gcc: No such file or directory`
```bash
sudo apt install gcc-arm-linux-gnueabi
```

### `cc1: error: bad value ('generic-armv7-a') for '-mtune=' switch`
You forgot to export `CROSS_COMPILE` before building U-Boot. Fix:
```bash
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
make distclean
make vexpress_ca9x4_defconfig
make -j$(nproc)
```

### `** No device specified **` when running saveenv
U-Boot environment FAT settings are missing. See Section 11 to configure:
```
CONFIG_ENV_FAT_INTERFACE="mmc"
CONFIG_ENV_FAT_DEVICE_AND_PART="0:1"
CONFIG_ENV_FAT_FILE="uboot.env"
```

### `Proceed anyway? (y,N)` during mkfs.ext4
Type `y` — the partition was formatted before. Safe to reformat.

### Loop device not found after reboot
`losetup` assignments reset when you reboot your laptop. Always re-attach:
```bash
sudo losetup --find --partscan sd.img
sudo losetup -l | grep sd.img    # find the new loop number
```

### QEMU shows no kernel output, just U-Boot prompt
`boot.scr` is missing from the SD card. Copy it:
```bash
sudo losetup --find --partscan sd.img
sudo mount /dev/loop18p1 /mnt/boot
sudo cp boot.scr /mnt/boot/
sudo umount /mnt/boot
sudo losetup -d /dev/loop18
```

---

## 🏆 Expected Final Result

```
Starting kernel ...

Booting Linux on physical CPU 0x0
Linux version 6.6.15-YourName-v1 (arm-linux-gnueabi-gcc) ...
...
mmcblk0: mmc0:4567 QEMU! 1.00 GiB
mmcblk0: p1 p2
...
VFS: Cannot open root device "" or unknown-block(0,0)
...
Kernel panic - not syncing: No working init found
```

This panic means:
- ✅ Kernel compiled successfully from source
- ✅ U-Boot loaded and executed the kernel
- ✅ Kernel initialized all hardware (CPU, RAM, MMC, UART)
- ✅ Kernel found the SD card and its partitions
- ❌ No userspace (init/rootfs) — that is Lab 07's job!

---

## 📁 Final File Structure

```
~/ITI/ITI_FADY/
├── u_boot/
│   └── u-boot/
│       └── u-boot              ← U-Boot binary for QEMU
├── LINUX/
│   ├── linux/
│   │   ├── arch/arm/boot/
│   │   │   ├── zImage          ← Kernel image
│   │   │   └── dts/arm/
│   │   │       └── vexpress-v2p-ca9.dtb  ← Device Tree
│   └── vexpress/
│       ├── sd.img              ← Virtual SD card
│       ├── boot.cmd            ← Boot script (text source)
│       └── boot.scr            ← Boot script (U-Boot binary)
```

**What is inside sd.img:**
```
sd.img
├── p1 (FAT32 — 100MB)
│   ├── zImage
│   ├── vexpress-v2p-ca9.dtb
│   ├── boot.scr
│   └── uboot.env       ← created after first saveenv
└── p2 (ext4 — 923MB)
    └── lost+found      ← empty until Lab 07
```

---

## ⚡ Quick Command Reference

```bash
# Environment (run in every new terminal)
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

# Build kernel
cd ~/ITI/ITI_FADY/LINUX/linux
make vexpress_defconfig
make -j$(nproc) zImage dtbs

# Create SD card
cd ~/ITI/ITI_FADY/LINUX/vexpress
dd if=/dev/zero of=sd.img bs=1M count=1024
# partition with fdisk (see Section 7)
sudo losetup --find --partscan sd.img
sudo losetup -l | grep sd.img           # find loop number
sudo mkfs.vfat -F 32 -n BOOT /dev/loop18p1
sudo mkfs.ext4 -L rootfs /dev/loop18p2

# Copy files to SD card
sudo mount /dev/loop18p1 /mnt/boot
sudo cp ~/ITI/ITI_FADY/LINUX/linux/arch/arm/boot/zImage /mnt/boot/
sudo cp ~/ITI/ITI_FADY/LINUX/linux/arch/arm/boot/dts/arm/vexpress-v2p-ca9.dtb /mnt/boot/
sudo cp boot.scr /mnt/boot/
ls -lh /mnt/boot/                       # verify
sudo umount /mnt/boot
sudo losetup -d /dev/loop18

# Build U-Boot
cd ~/ITI/ITI_FADY/u_boot/u-boot
make vexpress_ca9x4_defconfig
make -j$(nproc)

# Create boot script
cd ~/ITI/ITI_FADY/LINUX/vexpress
mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "Boot Script" -d boot.cmd boot.scr

# Boot QEMU
qemu-system-arm -M vexpress-a9 \
  -kernel ~/ITI/ITI_FADY/u_boot/u-boot/u-boot \
  -nographic -m 512M \
  -drive file=sd.img,if=sd,format=raw

# Exit QEMU: Ctrl+A then X
```
