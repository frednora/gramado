# blkdev - Block devices

Folders in this directory:

```
 + ahci/      - Support for AHCI devices.
 + ata/       - Support for ATA devices.
 + docs/      - Design notes for blkdev/.
 + storage.c  - Interface for block devices support.
```

## Controller modes 

List of controller modes. Probing into PCI interface we will find what type of controller we are using.

```
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

```
