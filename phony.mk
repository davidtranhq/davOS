.PHONY: iso run clean

ISOROOT = isoroot

qemu: iso
	qemu-system-x86_64 -cdrom $(ISO)

# Build a bootable CD-ROM for the OS
iso: $(BIN)
# Build limine-deploy.
	cd limine
	make -C limine
	cd ..
# Create a directory which will be our ISO root.
	mkdir -p $(ISOROOT)
# Copy the relevant files over.
	cp -v $(BIN) limine.cfg limine/limine.sys \
		limine/limine-cd.bin limine/limine-cd-efi.bin $(ISOROOT)/
# Create the bootable ISO.
	xorriso -as mkisofs -b limine-cd.bin \
			-no-emul-boot -boot-load-size 4 -boot-info-table \
			--efi-boot limine-cd-efi.bin \
			-efi-boot-part --efi-boot-image --protective-msdos-label \
			$(ISOROOT) -o $(ISO)
# Install Limine stage 1 and 2 for legacy BIOS boot.
	./limine/limine-deploy $(ISO)

clean:
	rm -rf $(BIN) $(ISO) $(ISOROOT) $(OBJS) $(HEADER_DEPS) $(CLEAN)