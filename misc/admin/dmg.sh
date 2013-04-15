#!/bin/sh

cd ~

# Create an initial disk image (32 megs)
hdiutil create -size 64m -fs HFS+ -volname "My Volume" oraccx.dmg

# Mount the disk image
hdiutil attach oraccx.dmg

scp -r /usr/local/oracc/bin oracc/dmg

# Obtain device information
DEVS=$(hdiutil attach oraccx.dmg | cut -f 1)
DEV=$(echo $DEVS | cut -f 1 -d ' ')

 
# Unmount the disk image
hdiutil detach $DEV 

# Convert the disk image to read-only
hdiutil convert oraccx.dmg -format UDZO -o oracc.dmg
