#
# Makefile for Lyos ramdisk
#

INITRD = $(ARCHDIR)/initrd.tar

.PHONY: everything realclean

everything : realclean $(INITRD)

realclean :
	@rm -f $(INITRD)

$(INITRD):
	$(Q)$(MAKE) -C ../utils $(MAKEFLAGS)
	$(Q)$(MAKE) -C ../utils $(MAKEFLAGS) install
	@cp -f $(DESTDIR)/sbin/init sbin/
	@cp -f $(DESTDIR)/sbin/ext2fs sbin/
	@cp -f $(DESTDIR)/sbin/ata sbin/
	@cp -f $(DESTDIR)/sbin/procfs sbin/
	@cp -f $(DESTDIR)/sbin/input sbin/
	@cp -f $(DESTDIR)/sbin/keyboard sbin/
	@cp -f $(DESTDIR)/sbin/fb sbin/
	@cp -f $(DESTDIR)/usr/bin/getty usr/bin/
	@cp -f $(DESTDIR)/usr/bin/login usr/bin/
	@cp -f $(DESTDIR)/bin/sh bin/
	@cp -f $(DESTDIR)/bin/service bin/
	@cp -f $(DESTDIR)/bin/mount bin/
	@cp -f $(DESTDIR)/lib/ld-lyos.so lib/
	@cp -f $(DESTDIR)/usr/lib/libc.so usr/lib
	@cp -f $(DESTDIR)/usr/lib/libg.so usr/lib
	@cp -rf ../sysroot/etc/* etc/
	@touch .root
	@(cd ../scripts ; bash create-ramdisk-dev.sh)
	@tar -cvf $(INITRD) .root bin/* lib/* sbin/* dev/* etc/* usr/bin/* usr/lib/* > /dev/null
	@rm .root
	@cp -f $(INITRD) $(DESTDIR)/boot/
