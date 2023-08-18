drivers: ide.o ioapic.o kbd.o lapic.o mp.o picirq.o uart.o
	$(LD) $(LDFLAGS) -N -o drivers.o
	$(OBJCOPY) -S -O binary drivers.o drivers