# Set the prefix for `riscvXX-unknown-elf-*`
# On installations using `multilib`, this will be `riscv64` even for compiling to 32-bit targets
TOOLCHAIN_PATH=/opt/riscv64-unknown-elf-gcc-8.3.0-2019.08.0-x86_64-linux-ubuntu14/bin/
TOOLCHAIN_PREFIX=riscv64
#TOOLCHAIN_PREFIX=riscv32
INCLUDES=-I$(shell pwd) -Igenerated/ -Ifirmware

VOLTAGE=$(shell python3 -c "from gpio_config_def import voltage; print(voltage)")

# ---- Test patterns for project raven ----

.SUFFIXES:

PATTERN = konrad_test

all: ${PATTERN}.hex ${PATTERN}.elf ${PATTERN}.bin

# Flashrom requires a binary that's equal to the size of the flash
flashrom: ${PATTERN}.bin
	rm -f root.bin && dd if=${PATTERN}.bin of=root.bin && dd if=/dev/zero of=root.bin bs=1 count=1 bs=1M seek=3
	flashrom -p serprog:dev=/dev/ttyACM0 --layout flashregion.layout --image root.bin -w root.bin -N


hex:  ${PATTERN:=.hex}

%.elf: %.c sections.lds crt0_vex.S
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-unknown-elf-gcc $(INCLUDES) -O0 -mabi=ilp32 -march=rv32i -D__vexriscv__ -Wl,-Bstatic,-T,sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ crt0_vex.S firmware/isr.c $<
	${TOOLCHAIN_PATH}$(TOOLCHAIN_PREFIX)-unknown-elf-objdump -D $@ > $*.lst

%.hex: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-unknown-elf-objcopy -O verilog $< $@
	sed -i.orig -e 's/@1000/@0000/g' $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-unknown-elf-objcopy -O binary $< $@

client: client.c
	gcc client.c -o client

flash: konrad_test.hex
	make -f ../../nucleo/Makefile flash FILE=konrad_test.hex VOLTAGE=$(VOLTAGE)

sanity_check: gpio_config_def.py
	make -f ../../nucleo/Makefile sanity_check FILE=gpio_config_def.py

# run to bringup Caravel power while mounted on Nucleo
poweron:
	make -f ../../nucleo/Makefile poweron VOLTAGE=$(VOLTAGE)

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.lst *.orig gpio_config_data.*

.PHONY: clean hex all flash

