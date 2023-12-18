ifeq ($(shell uname -s),Darwin)
	OS := macOS
else
	OS := other
endif

.PHONY: qemu debug test debug-test iso run clean

ISOROOT = isoroot

qemu: iso
	qemu-system-x86_64 -cdrom $(ISO) -d int -no-shutdown -no-reboot

# Run a qemu instance in the background and attach a GDB instance to it
debug: iso
	qemu-system-x86_64 -cdrom $(ISO) -d int -no-shutdown -no-reboot -S -gdb tcp::1234 &
ifeq ($(OS), macOS)
	lldb \
		-o "target create $(BIN)" \
		-o "gdb-remote localhost:1234" \
		-o "breakpoint set --name kernel_main" \
		-o "c"
else
	gdb -ex 'target remote localhost:1234' -ex 'symbol-file $(BIN)' -ex 'b kernel_main'
endif

# run with DEBUG flags but don't attach a debugger
test: CPPFLAGS += -DDEBUG_BUILD
test: qemu

debug: CPPFLAGS += -DDEBUG_BUILD
debug: debug

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