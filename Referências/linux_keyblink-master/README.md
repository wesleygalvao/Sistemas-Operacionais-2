# A simple keyboard-blinker Linux kernel module

Blink blink! Control each of your keyboard LED as you want using a Linux kernel
module.

## Description

Implemented from an old paper about writting a keyboard-blinker module for
Linux 2.6 kernels, and converted to be compatible with the new 3.x Linux
kernel version.

## Building

Be sure to have Linux kernel headers installed on your system (package
`linux-headers` on Archlinux) in order to be able to compile the module.

Then you can run the Makefile:

    $ make

And you should see a new kernel module named `keyboard_blink.ko` in your
working directory.

## Running

In order to run the module, you will have to load it as root into your kernel
using the `insmod` command:

    # insmod keyboard_blink.ko

Once you think your keyboard has blinked enough, you can stop it by removing
the module from the kernel (which will restore the keyboard LEDs state):

    # rmmod keyboard_blink

Feel free to hack around (you can change blinking frequency for example by
editing the source code of the module).

## Bugs

There are actually two important bugs in this software:

 - **Changing TTY while the module is active will trigger a kernel panic** ; I
   think the main reason is that we are trying to call the TTY driver of the
   active terminal we retrieved on the module setup but this TTY is not active
   anymore.
 - **Unloading the module will not reset the LEDs state**, however on any
   special key pressed (CapsLock, …) the LEDs will show their own states again.

## Copyright

This project is brought to you under the GNU General Public License version 3
or later. For further informations please read the provided COPYING file.

## Reference

 - [The Linux Kernel Module Programming Guide (for 2.6
   kernel)](http://www.tldp.org/LDP/lkmpg/2.6/html/index.html).
 - The kernel headers, especially `linux/drivers/char/keyboard.c` in order to
   understand how the LEDs are handled using `KDSETLED` ioctl command.
 - StackOverflow threads for fixing the code to 3.x branch.

