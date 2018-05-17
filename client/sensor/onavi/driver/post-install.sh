#!/bin/sh
if [ -f /System/Library/Extensions.kextcache ]
then
  rm -fR /System/Library/Extensions.kextcache
fi
if [ -f /System/Library/Extensions.mkext ]
then
  rm -fR /System/Library/Extensions.mkext
fi
touch /System/Library/Extensions
