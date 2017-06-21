# Quelldatei
SRCDIR = src
BUILD = build
# Objektdatei
OBJDIR = obj
CFILES := $(foreach dir, $(SRCDIR)/, $(notdir $(wildcard $(SRCDIR)/*.c)))
OBJS := $(addprefix $(OBJDIR)/, $(CFILES:.c=.o))
# Includedatei
INCLUDE = -I ../h -I include
# Toolchain
TOOLCHAIN = arm-eb63-elf-
# Compiler
COMPILER = gcc
# Linker/Binder
LINKER = ld
# Debugger
DEBUGGER = insight
# Optimierungsstufe
OPTI = 0

OUTPUT = ausschank

# Bauen
all: $(OBJS)


# Erzeugen der benoetitgen Objektdateien
	$(TOOLCHAIN)$(COMPILER) -c -g -O$(OPTI) ../boot/swi.S -o $(OBJDIR)/swi.o $(INCLUDE)
	$(TOOLCHAIN)$(COMPILER) -c -g -O$(OPTI) ../boot/boot_ice.S -o $(OBJDIR)/boot_ice.o $(INCLUDE)
	
# Binden fuer die RAM-Version 
	$(TOOLCHAIN)$(LINKER) -Ttext 0x02000000 -O$(OPTI) $(OBJDIR)/boot_ice.o $(OBJDIR)/swi.o $(OBJS) -o $(OUTPUT).elf /opt/arm-eb63-elf/lib/gcc/arm-eb63-elf/4.4.6/libgcc.a


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(TOOLCHAIN)$(COMPILER) -c -g -O$(OPTI) $< $(INCLUDE) -o $@

# Debugger starten
debug:
	$(TOOLCHAIN)$(DEBUGGER) $(OUTPUT).elf

doc:
	doxygen doxygen.conf

# Aufraeumen
clean:
	rm -r $(OBJDIR)
	rm *.elf

