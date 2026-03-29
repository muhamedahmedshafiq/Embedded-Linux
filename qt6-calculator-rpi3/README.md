# Qt6 Calculator on Raspberry Pi 3B+ with Buildroot

![Raspberry Pi](https://img.shields.io/badge/Raspberry%20Pi-3B+-red?logo=raspberrypi)
![Qt](https://img.shields.io/badge/Qt-6.9.1-green?logo=qt)
![Buildroot](https://img.shields.io/badge/Buildroot-2024.02-blue)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 📖 Description

A custom embedded Linux distribution built with **Buildroot** for the **Raspberry Pi 3B+**, featuring a **Qt6 QML Calculator** application that auto-starts on boot. The project demonstrates how to create a minimal embedded Linux system with a graphical Qt6 application running directly on the framebuffer without X11 or Wayland.

### Key Features
- Minimal Linux image (~512MB) built with Buildroot
- Qt6 QML-based calculator with modern UI
- Auto-starts on boot using init script
- Runs on Linux framebuffer (`linuxfb`) — no desktop environment needed
- USB mouse and touchscreen support
- Serial console access via UART
- Cross-compiled for AArch64 (ARM 64-bit)

---

## 📁 Project Structure

```
buildroot/
├── board/
│   └── raspberrypi3-64/
│       ├── config_3_64bit.txt          # RPi boot configuration
│       ├── fix-permissions.sh          # Post-build permission fix script
│       ├── post-build.sh               # Buildroot post-build script
│       ├── post-image.sh               # Buildroot post-image script
│       ├── genimage.cfg.in             # SD card image layout template
│       └── rootfs_overlay/
│           └── etc/
│               └── init.d/
│                   └── S99qtapp        # Auto-start init script
│
├── package/
│   ├── Config.in                       # Main package config (modified)
│   └── iti-gpio/
│       ├── Config.in                   # Package Kconfig
│       ├── iti-gpio.mk                 # Buildroot package makefile
│       ├── CMakeLists.txt              # CMake build configuration
│       ├── main.cpp                    # Qt6 application entry point
│       └── Main.qml                   # QML calculator UI
│
└── configs/
    └── raspberrypi3_64_defconfig       # Base defconfig for RPi 3B+ 64-bit
```

---

## 🖩 Calculator Application

A simple, modern-looking calculator built with Qt6 QML featuring:
- Basic arithmetic operations: `+`, `-`, `×`, `÷`
- Clear (`C`) and Equals (`=`) buttons
- Error handling for invalid expressions
- Dark theme with orange operator buttons
- Responsive grid layout

### Screenshots

| Calculator UI |
|:---:|
| Dark themed calculator with number pad and operator buttons |

---

## 🛠️ Build Instructions

### Prerequisites

- **Host OS**: Ubuntu 20.04+ / Debian 11+
- **Disk Space**: ~15GB free
- **RAM**: 4GB+ recommended
- **Build Time**: 1-3 hours (depending on hardware)

#### Install Required Packages

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    git \
    wget \
    cpio \
    unzip \
    rsync \
    bc \
    libncurses5-dev \
    file \
    sed \
    make \
    binutils \
    gcc \
    g++ \
    bash \
    patch \
    gzip \
    bzip2 \
    perl \
    tar \
    cpio \
    python3 \
    mtools \
    genimage \
    dosfstools \
    e2fsprogs
```

### Step 1: Clone Buildroot

```bash
git clone https://github.com/buildroot/buildroot.git -b 2024.02
cd buildroot
```

### Step 2: Create Package Files

#### 2.1 Create Package Directory

```bash
mkdir -p package/iti-gpio
```

#### 2.2 `package/iti-gpio/Config.in`

```bash
cat > package/iti-gpio/Config.in << 'EOF'
config BR2_PACKAGE_ITI_GPIO
    bool "iti-gpio"
    depends on BR2_PACKAGE_QT6BASE
    depends on BR2_PACKAGE_QT6BASE_GUI
    depends on BR2_PACKAGE_QT6DECLARATIVE
    select BR2_PACKAGE_QT6SHADERTOOLS
    help
      Simple calculator Qt6 QML app for Raspberry Pi 3B+
EOF
```

#### 2.3 `package/iti-gpio/iti-gpio.mk`

```bash
cat > package/iti-gpio/iti-gpio.mk << 'EOF'
ITI_GPIO_VERSION = 1.0
ITI_GPIO_SITE = $(TOPDIR)/package/iti-gpio
ITI_GPIO_SITE_METHOD = local
ITI_GPIO_INSTALL_TARGET = YES
ITI_GPIO_DEPENDENCIES = qt6base qt6declarative qt6shadertools

ITI_GPIO_CONF_OPTS = \
    -DCMAKE_PREFIX_PATH=$(STAGING_DIR)/usr/lib/cmake

$(eval $(cmake-package))
EOF
```

#### 2.4 `package/iti-gpio/CMakeLists.txt`

```bash
cat > package/iti-gpio/CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.16)
project(iti-gpio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

file(WRITE ${CMAKE_BINARY_DIR}/qml.qrc
"<RCC>\n<qresource prefix=\"/\">\n<file>Main.qml</file>\n</qresource>\n</RCC>\n")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Main.qml
    ${CMAKE_BINARY_DIR}/Main.qml
    COPYONLY
)

add_executable(iti-gpio
    main.cpp
    ${CMAKE_BINARY_DIR}/qml.qrc
)

target_link_libraries(iti-gpio PRIVATE Qt6::Quick)

install(TARGETS iti-gpio DESTINATION /usr/bin)
EOF
```

#### 2.5 `package/iti-gpio/main.cpp`

```bash
cat > package/iti-gpio/main.cpp << 'EOF'
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
EOF
```

#### 2.6 `package/iti-gpio/Main.qml`

```bash
cat > package/iti-gpio/Main.qml << 'QMLEOF'
import QtQuick
import QtQuick.Window

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("calculator")
    color: "#1a1a1a"

    property string expression: ""
    property bool isCalculated: false

    function handleInput(buttonValue) {
        if (buttonValue === "C") {
            expression = "";
        } else if (buttonValue === "=") {
            try {
                var formattedExpr = expression.replace(/×/g, "*").replace(/÷/g, "/");
                expression = eval(formattedExpr).toString();
                isCalculated = true;
            } catch (e) {
                expression = "Error";
            }
        } else {
            if (isCalculated) {
                expression = buttonValue;
                isCalculated = false;
            } else {
                expression += buttonValue;
            }
        }
    }

    Rectangle {
        id: displayID
        width: parent.width * .97
        height: parent.height * .25
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 10
        radius: 20
        color: "black"
        border.color: "#333333"
        border.width: 2

        Text {
            id: displayText
            text: expression === "" ? "0" : expression
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 20
            color: "white"
            font.pixelSize: 60
            font.weight: Font.Light
        }
    }

    Rectangle {
        id: buttonArea
        width: parent.width * .9
        height: parent.height * .6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#1a1a1a"

        Grid {
            anchors.fill: parent
            columns: 4
            spacing: 10

            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "7"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("7") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "8"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("8") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "9"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("9") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#ff9500"
                Text { text: "÷"; anchors.centerIn: parent; color: "white"; font.pixelSize: 40; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("÷") }
            }

            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "4"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("4") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "5"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("5") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "6"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("6") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#ff9500"
                Text { text: "×"; anchors.centerIn: parent; color: "white"; font.pixelSize: 40; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("×") }
            }

            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "1"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("1") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "2"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("2") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "3"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("3") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#ff9500"
                Text { text: "-"; anchors.centerIn: parent; color: "white"; font.pixelSize: 45; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("-") }
            }

            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#ff5555"
                Text { text: "C"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("C") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#333333"
                Text { text: "0"; anchors.centerIn: parent; color: "white"; font.pixelSize: 30; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("0") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#2ecc71"
                Text { text: "="; anchors.centerIn: parent; color: "white"; font.pixelSize: 45; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("=") }
            }
            Rectangle {
                width: parent.width / 4 - 10; height: parent.height / 4 - 10
                radius: 20; color: "#ff9500"
                Text { text: "+"; anchors.centerIn: parent; color: "white"; font.pixelSize: 40; font.bold: true }
                MouseArea { anchors.fill: parent; onClicked: handleInput("+") }
            }
        }
    }
}
QMLEOF
```

### Step 3: Register Package in Buildroot

Edit `package/Config.in` and add before the **last** `endmenu`:

```bash
nano package/Config.in
```

Add:

```
menu "Custom packages"
    source "package/iti-gpio/Config.in"
endmenu
```

The end of the file should look like:

```
    source "package/uemacs/Config.in"
    source "package/vim/Config.in"
endmenu

menu "Custom packages"
    source "package/iti-gpio/Config.in"
endmenu

endmenu
```

### Step 4: Create Board Configuration Files

#### 4.1 Boot Configuration (`board/raspberrypi3-64/config_3_64bit.txt`)

```bash
cat > board/raspberrypi3-64/config_3_64bit.txt << 'EOF'
start_file=start.elf
fixup_file=fixup.dat

kernel=Image

# Disable overscan
disable_overscan=1

# GPU Memory
gpu_mem=256

# Enable UART0 for serial console
dtoverlay=miniuart-bt

# Enable 64bits support
arm_64bit=1

# Enable DRM/KMS driver for Qt EGLFS
dtoverlay=vc4-fkms-v3d

# Force HDMI output
hdmi_force_hotplug=1
EOF
```

#### 4.2 Permission Fix Script (`board/raspberrypi3-64/fix-permissions.sh`)

```bash
cat > board/raspberrypi3-64/fix-permissions.sh << 'EOF'
#!/bin/sh
chmod +x ${TARGET_DIR}/etc/init.d/S99qtapp
EOF
chmod +x board/raspberrypi3-64/fix-permissions.sh
```

#### 4.3 Auto-Start Init Script

```bash
mkdir -p board/raspberrypi3-64/rootfs_overlay/etc/init.d/

cat > board/raspberrypi3-64/rootfs_overlay/etc/init.d/S99qtapp << 'EOF'
#!/bin/sh

case "\$1" in
    start)
        echo "Starting Calculator..."
        export QT_QPA_PLATFORM=linuxfb
        export LANG=C.UTF-8
        export QT_QPA_FB_HIDECURSOR=0
        export QT_QPA_GENERIC_PLUGINS=evdevmouse:/dev/input/mice,evdevtouch:/dev/input/event1
        sleep 5
        /usr/bin/iti-gpio > /tmp/qtapp.log 2>&1 &
        ;;
    stop)
        killall -9 iti-gpio
        ;;
    *)
        echo "Usage: \$0 {start|stop}"
        ;;
esac

exit 0
EOF
chmod +x board/raspberrypi3-64/rootfs_overlay/etc/init.d/S99qtapp
```

### Step 5: Configure Buildroot (`make menuconfig`)

```bash
make raspberrypi3_64_defconfig
make menuconfig
```

#### 5.1 Target Options

```
Target options --->
    Target Architecture: AArch64 (little endian)
    Target Architecture Variant: cortex-A53
    Floating point strategy: VFPv4
    MMU Page Size: 4KB
```

#### 5.2 Toolchain

```
Toolchain --->
    C library: glibc
    [*] Enable C++ support
    [*] Enable WCHAR support
    [*] Enable threads support
```

#### 5.3 System Configuration

```
System configuration --->
    (board/raspberrypi3-64/rootfs_overlay) Root filesystem overlay directories
    (board/raspberrypi3-64/post-build.sh board/raspberrypi3-64/fix-permissions.sh) Custom scripts to run before creating filesystem images
```

#### 5.4 Target Packages — Graphics

```
Target packages --->
    Graphic libraries and applications (graphic/text) --->
        [*] libdrm
            -*- vc4

        [*] mesa3d --->
            [*] Gallium v3d driver
            -*- Gallium vc4 driver
            -*- gbm
            -*- OpenGL EGL
            [*] OpenGL ES

        [*] Qt6 --->
            -*- qt6base
            [*]   concurrent module
            -*-   gui module
            [*]     linuxfb support
            [*]     eglfs support
            -*-     OpenGL support
                    OpenGL API (OpenGL ES 2.0+)
            [*]     fontconfig support
            [*]     harfbuzz support
            [*]     GIF support
            [*]     JPEG support
            [*]     PNG support
            [*]   widgets module
            -*-   qt6shadertools
            -*-   qt6declarative
            -*-     quick module
```

#### 5.5 Target Packages — Custom

```
Target packages --->
    Custom packages --->
        [*] iti-gpio
```

#### 5.6 Target Packages — Fonts

```
Target packages --->
    Fonts, cursors, icons, sounds and themes --->
        [*] DejaVu fonts
```

### Step 6: Build

```bash
make -j$(nproc)
```

> ⏳ Build takes **1-3 hours** depending on your machine. Qt6 is the largest component.

### Step 7: Flash to SD Card

```bash
# Find your SD card device
lsblk

# Unmount any mounted partitions
sudo umount /dev/sdX1
sudo umount /dev/sdX2

# Flash the image (replace /dev/sdX with your SD card device)
sudo dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress
sync
```

> ⚠️ **WARNING**: Double-check the target device (`/dev/sdX`) before running `dd` to avoid overwriting your system drive!

---

## 🔧 Hardware Setup

### Required Hardware
- Raspberry Pi 3 Model B+
- MicroSD card (8GB+)
- HDMI display
- 5V/2.5A power supply
- USB mouse or USB touchscreen (optional)
- USB-to-Serial adapter (optional, for debugging)

### Wiring for Serial Console (Optional)

| USB-to-Serial | RPi 3B+ GPIO |
|:---:|:---:|
| TX | GPIO 15 (RXD) |
| RX | GPIO 14 (TXD) |
| GND | GND |

Connect via serial:

```bash
sudo minicom -D /dev/ttyUSB0 -b 115200
```

---

## 📋 Boot Partition Contents

After building, the boot partition contains:

| File | Description |
|------|-------------|
| `bcm2710-rpi-3-b-plus.dtb` | Device Tree Blob for RPi 3B+ |
| `bootcode.bin` | GPU bootloader (1st stage) |
| `config.txt` | RPi boot configuration |
| `cmdline.txt` | Kernel command line |
| `fixup.dat` | GPU firmware fixup |
| `start.elf` | GPU firmware (2nd stage) |
| `Image` | Linux kernel (AArch64) |
| `overlays/` | Device tree overlays |

---

## 🐛 Troubleshooting

### App Doesn't Start

```bash
# Login via serial console
root

# Check log
cat /tmp/qtapp.log

# Check if binary exists
which iti-gpio

# Check if process is running
ps | grep iti
```

### Try Different Display Backends

```bash
# Kill existing instance
killall -9 iti-gpio

# Try linuxfb
QT_QPA_PLATFORM=linuxfb /usr/bin/iti-gpio

# Try with specific framebuffer
QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0 /usr/bin/iti-gpio
```

### Mouse/Touch Not Working

```bash
# Check input devices
ls /dev/input/
cat /proc/bus/input/devices

# Restart with mouse support
killall -9 iti-gpio
export QT_QPA_PLATFORM=linuxfb
export QT_QPA_GENERIC_PLUGINS=evdevmouse:/dev/input/mice,evdevtouch
/usr/bin/iti-gpio &
```

### Permission Denied on S99qtapp

```bash
# Mount SD card on PC
sudo mount /dev/sdX2 /mnt
sudo chmod +x /mnt/etc/init.d/S99qtapp
sudo umount /mnt
```

### Undervoltage Warning

```
hwmon hwmon1: Undervoltage detected!
```

Use a **5V/2.5A** power supply with a good quality USB cable.

### Available Qt Platform Plugins

```
vnc, minimal, offscreen, linuxfb
```

---

## 📊 System Information

| Component | Details |
|-----------|---------|
| **Board** | Raspberry Pi 3 Model B+ |
| **SoC** | BCM2837B0 (Cortex-A53) |
| **Architecture** | AArch64 (ARM 64-bit) |
| **Kernel** | Linux 6.12.61-v8 |
| **Buildroot** | 2024.02 |
| **Qt Version** | 6.9.1 |
| **Display** | LinuxFB (Framebuffer) |
| **Resolution** | 640x480 |
| **Root filesystem** | ext4 |
| **Boot partition** | FAT32 (vfat) |

---

## 📚 References

- [Buildroot Documentation](https://buildroot.org/downloads/manual/manual.html)
- [Qt6 for Embedded Linux](https://doc.qt.io/qt-6/embedded-linux.html)
- [Raspberry Pi Documentation](https://www.raspberrypi.com/documentation/)
- [Raspberry Pi config.txt](https://www.raspberrypi.com/documentation/computers/config_txt.html)
- [Qt Platform Abstraction (QPA)](https://doc.qt.io/qt-6/qpa.html)

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 👤 Author

**Mohamed** — ITI Embedded Linux Track

---

## 🙏 Acknowledgments

- [Buildroot](https://buildroot.org/) — Embedded Linux build system
- [Qt Project](https://www.qt.io/) — Cross-platform application framework
- [Raspberry Pi Foundation](https://www.raspberrypi.org/) — Hardware platform
