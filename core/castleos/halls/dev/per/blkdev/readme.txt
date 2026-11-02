## Controller modes 

Each folder in this directory is for supporting a 
type of controller.
Probing into PCI interface we will find what type of 
controller we are using.

List of controller modes. 

SCSI     0x00
ATA      0x01
RAID     0x04
// Sub-class 05h = ATA Controller with ADMA
DMA      0x05   // (USB ?)
AHCI     0x06
// 0x08: NVMe (Non-Volatile Memory Express)
NVME     0x08
// 0x09: SAS (Serial Attached SCSI)
SAS      0x09
UNKNOWN  0xFF
