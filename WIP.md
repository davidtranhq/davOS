# WIP

Paging works: the last step is reclaiming Limine bootloader memory, which requires us to
finally replace the Limine stack with our own (since the Limine stack is stored in Limine
bootloader memory)