MARENGO

3D Printer Firmware for STM32F429 discovery board based on ChibiOS and uGFX

How to build:
-download or clone this repo
-download ChibiOS v18.2.2 and unpack WITHOUT OVERWRITING to chibios1822 folder (or rename ChibiOS_18.2.2 folder)
-download uGFX v2.8 and unpack WITHOUT OVERWRITING to ugfx_2.8
-type "make" in console (you migt need GNU Arm Embedded toolchain, f.e. in Ubuntu type "sudo apt-get install -y gcc-arm-none-eabi")
-program your board with stlink (f.e. using https://github.com/texane/stlink)
