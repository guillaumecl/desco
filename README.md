desco
=====

tslib / framebuffer interface for Linux and the Raspberry pi


This is a small program that is used as the boot interface of a Raspberry Pi that has a PiTFT plugged in.

Ultimately, this will allow to control mpd with the small tactile screen (320x240). 

The dependencies should be kept as small as possible, and the boot time is important, because this will be used as a small computer inside my car if all goes well.

Currently, using systemd (on a quite basic gentoo install). boot time is about 4 seconds after plugging the power on. Even after adding more features, we should be careful to try and make the boot time never much more than this.
