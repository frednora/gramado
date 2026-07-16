
Device Driver Categories

Different abstractions:
 + Block devices → random access, storage.
 + Character devices → sequential streams, input/output.
 + Network devices → packet/frame oriented.

blkdev/
Block devices (storage that works in fixed‑size blocks).
Examples: hard disks, SSDs, USB mass storage.
Exposes read/write operations in terms of blocks (e.g., 512 bytes).
Used by filesystems to store and retrieve data.

chardev/
Character devices (stream‑oriented, no fixed block size).
Examples: serial ports, keyboards, mice, terminals.
Provides sequential read/write access (like a stream of bytes).
Often used for interactive I/O.

netdev/
Network devices (NICs, Wi‑Fi adapters, virtual interfaces).
Handles sending/receiving frames, interrupts, DMA, buffer management.
Provides the low‑level glue between hardware and your net/ stack.

--------------------------------------
Device Drivers (dev/)
 + blkdev/ → block devices (disks, SSDs, USB storage).
 + chardev/ → character devices (keyboard, serial ports, mice, terminals).
 + netdev/ → network devices (NICs, Wi‑Fi adapters, virtual interfaces).

This separation is classic:
 + Block devices → random access storage.
 + Character devices → sequential streams.
 + Network devices → packet/frame oriented.
