#!/bin/bash

sudo openocd -f interface/ftdi/olimex-arm-usb-ocd-h.cfg -f target/at91sam3ax_8x.cfg
