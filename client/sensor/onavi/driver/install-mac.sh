# note - this is risky so make sure you have backed up your computer first!
#!/bin/sh
sudo rm -rf /System/Library/Extensions/ExarUSBCDCACM.kext
sudo unzip mac-driver-exar.zip -d /System/Library/Extensions
sudo chmod -R 755 /System/Library/Extensions/ExarUSBCDCACM.kext
sudo chown -R root:wheel /System/Library/Extensions/ExarUSBCDCACM.kext
sudo rm -R /System/Library/Extensions.kextcache
sudo rm -R /System/Library/Extensions.mkext
sudo reboot
