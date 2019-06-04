#
# Component Makefile
#

REL_PY_SRC = micropython/py
ABS_PY_SRC = $(COMPONENT_PATH)/$(REL_PY_SRC)
HEADER_BUILD = $(COMPONENT_PATH)/inc/genhdr
GENSRC_BUILD = $(COMPONENT_PATH)/src/gensrc
FROZEN_MPY_BUILD = $(BUILD_DIR_BASE)/micropython/frozen_mpy
FROZEN_MPY_DIR = ${COMPONENT_PATH}/modules
FROZEN_DIR = 

CFLAGS += -DN_XTENSA -O3

COMPONENT_EXTRA_CLEAN = inc/genhdr/*.h src/gensrc/*.c

COMPONENT_ADD_INCLUDEDIRS := inc
COMPONENT_PRIV_INCLUDEDIRS := inc micropython micropython/lib/utils micropython/lib/mp-readline micropython/lib/netutils
#micropython micropython/esp32 micropython/lib/mp-readline micropython/lib/netutils micropython/lib/timeutils micropython/extmod

COMPONENT_SRCDIRS := src src/gensrc $(REL_PY_SRC) micropython/lib/utils micropython/lib/mp-readline micropython/lib/netutils micropython/extmod
COMPONENT_OBJS := src/help.o \
	src/file.o \
	src/mpthreadport.o \
	src/mp.o \
	src/mphalport.o \
	src/gccollect.o \
	src/modbadge.o \
	src/modesp.o \
	src/modu8g2.o \
	src/moduhashlib.o \
	src/modmachine.o \
	src/modnetwork.o \
	src/modsocket.o \
	src/moduos.o \
	src/modutime.o \
	$(REL_PY_SRC)/reader.o \
	$(REL_PY_SRC)/emitcommon.o \
	$(REL_PY_SRC)/objlist.o \
	$(REL_PY_SRC)/gc.o \
	$(REL_PY_SRC)/smallint.o \
	$(REL_PY_SRC)/map.o \
	$(REL_PY_SRC)/objnamedtuple.o \
	$(REL_PY_SRC)/objstringio.o \
	$(REL_PY_SRC)/asmbase.o \
	$(REL_PY_SRC)/stream.o \
	$(REL_PY_SRC)/builtinimport.o \
	$(REL_PY_SRC)/emitnxtensa.o \
	$(REL_PY_SRC)/runtime_utils.o \
	$(REL_PY_SRC)/malloc.o \
	$(REL_PY_SRC)/obj.o \
	$(REL_PY_SRC)/emitinlinethumb.o \
	$(REL_PY_SRC)/modcollections.o \
	$(REL_PY_SRC)/emitglue.o \
	$(REL_PY_SRC)/asmxtensa.o \
	$(REL_PY_SRC)/objzip.o \
	$(REL_PY_SRC)/binary.o \
	$(REL_PY_SRC)/objbool.o \
	$(REL_PY_SRC)/emitbc.o \
	$(REL_PY_SRC)/moduerrno.o \
	$(REL_PY_SRC)/mpz.o \
	$(REL_PY_SRC)/opmethods.o \
	$(REL_PY_SRC)/warning.o \
	$(REL_PY_SRC)/objreversed.o \
	$(REL_PY_SRC)/objnone.o \
	$(REL_PY_SRC)/objgetitemiter.o \
	$(REL_PY_SRC)/nlrsetjmp.o \
	$(REL_PY_SRC)/objexcept.o \
	$(REL_PY_SRC)/modio.o \
	$(REL_PY_SRC)/qstr.o \
	$(REL_PY_SRC)/parse.o \
	$(REL_PY_SRC)/nativeglue.o \
	$(REL_PY_SRC)/formatfloat.o \
	$(REL_PY_SRC)/mpstate.o \
	$(REL_PY_SRC)/objproperty.o \
	$(REL_PY_SRC)/objint_longlong.o \
	$(REL_PY_SRC)/objgenerator.o \
	$(REL_PY_SRC)/modstruct.o \
	$(REL_PY_SRC)/emitinlinextensa.o \
	$(REL_PY_SRC)/objint.o \
	$(REL_PY_SRC)/objtuple.o \
	$(REL_PY_SRC)/objmodule.o \
	$(REL_PY_SRC)/reader.o \
	$(REL_PY_SRC)/nlr.o \
	$(REL_PY_SRC)/bc.o \
	$(REL_PY_SRC)/objslice.o \
	$(REL_PY_SRC)/unicode.o \
	$(REL_PY_SRC)/vstr.o \
	$(REL_PY_SRC)/builtinevex.o \
	$(REL_PY_SRC)/modmicropython.o \
	$(REL_PY_SRC)/showbc.o \
	$(REL_PY_SRC)/sequence.o \
	$(REL_PY_SRC)/frozenmod.o \
	$(REL_PY_SRC)/objpolyiter.o \
	$(REL_PY_SRC)/objset.o \
	$(REL_PY_SRC)/objdict.o \
	$(REL_PY_SRC)/objtype.o \
	$(REL_PY_SRC)/modsys.o \
	$(REL_PY_SRC)/modmath.o \
	$(REL_PY_SRC)/runtime.o \
	$(REL_PY_SRC)/objsingleton.o \
	$(REL_PY_SRC)/objarray.o \
	$(REL_PY_SRC)/asmthumb.o \
	$(REL_PY_SRC)/lexer.o \
	$(REL_PY_SRC)/argcheck.o \
	$(REL_PY_SRC)/modarray.o \
	$(REL_PY_SRC)/modthread.o \
	$(REL_PY_SRC)/compile.o \
	$(REL_PY_SRC)/modbuiltins.o \
	$(REL_PY_SRC)/mpprint.o \
	$(REL_PY_SRC)/parsenumbase.o \
	$(REL_PY_SRC)/objmap.o \
	$(REL_PY_SRC)/objclosure.o \
	$(REL_PY_SRC)/objcomplex.o \
	$(REL_PY_SRC)/modcmath.o \
	$(REL_PY_SRC)/objfun.o \
	$(REL_PY_SRC)/scheduler.o \
	$(REL_PY_SRC)/stackctrl.o \
	$(REL_PY_SRC)/objobject.o \
	$(REL_PY_SRC)/objstrunicode.o \
	$(REL_PY_SRC)/nlrxtensa.o \
	$(REL_PY_SRC)/scope.o \
	$(REL_PY_SRC)/objint_mpz.o \
	$(REL_PY_SRC)/builtinhelp.o \
	$(REL_PY_SRC)/objstr.o \
	$(REL_PY_SRC)/objfilter.o \
	$(REL_PY_SRC)/objboundmeth.o \
	$(REL_PY_SRC)/objcell.o \
	$(REL_PY_SRC)/parsenum.o \
	$(REL_PY_SRC)/repl.o \
	$(REL_PY_SRC)/vm.o \
	$(REL_PY_SRC)/persistentcode.o \
	$(REL_PY_SRC)/objenumerate.o \
	$(REL_PY_SRC)/objattrtuple.o \
	$(REL_PY_SRC)/modgc.o \
	$(REL_PY_SRC)/objfloat.o \
	$(REL_PY_SRC)/objrange.o \
	micropython/lib/utils/interrupt_char.o \
	micropython/lib/utils/printf.o \
	micropython/lib/utils/pyexec.o \
	micropython/lib/utils/stdout_helpers.o \
	micropython/lib/mp-readline/readline.o \
	micropython/lib/netutils/netutils.o \
	micropython/extmod/modframebuf.o \
	micropython/extmod/modubinascii.o \
	micropython/extmod/moductypes.o \
	micropython/extmod/moduheapq.o \
	micropython/extmod/modujson.o \
	micropython/extmod/modure.o \
	micropython/extmod/modurandom.o \
	micropython/extmod/moduselect.o \
	micropython/extmod/modussl_mbedtls.o \
	micropython/extmod/moduzlib.o \
	micropython/extmod/modwebsocket.o \
	micropython/extmod/modwebrepl.o \
	micropython/extmod/uos_dupterm.o \
	micropython/extmod/utime_mphal.o
	
# QSTR shenanigans below this line

#$(IDF_PATH)/components/esp32/include $(IDF_PATH)/components/soc/include $(IDF_PATH)/components/soc/esp32/include
#SRC_QSTR = $(COMPONENT_PATH)/src/moduhashlib.c
SRC_QSTR = $(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_OBJS:.o=.c)))

QSTR_INCLUDES = $(abspath $(addprefix $(COMPONENT_PATH)/,$(COMPONENT_ADD_INCLUDEDIRS) $(COMPONENT_PRIV_INCLUDEDIRS)))
QSTR_INCLUDE_FLAGS = $(addprefix -I ,$(QSTR_INCLUDES) $(COMPONENT_INCLUDES))

QSTR_DEFS = $(COMPONENT_PATH)/inc/qstrdefsport.h
PY_QSTR_DEFS = $(ABS_PY_SRC)/qstrdefs.h
QSTR_DEFS_COLLECTED = $(HEADER_BUILD)/qstrdefs.collected.h
QSTR_GEN_EXTRA_CFLAGS += -DNO_QSTR $(QSTR_INCLUDE_FLAGS)
SED ?= sed
MKDIR ?= mkdir
PYTHON ?= python
CPP ?= $(CC) -E
MAKE ?= make

TOP = $(COMPONENT_PATH)/micropython
MAKE_FROZEN = $(TOP)/tools/make-frozen.py
MPY_CROSS = $(TOP)/mpy-cross/mpy-cross
MPY_TOOL = $(TOP)/tools/mpy-tool.py


# The following rule uses | to create an order only prerequisite. Order only
# prerequisites only get built if they don't exist. They don't cause timestamp
# checking to be performed.
#
# We don't know which source files actually need the generated.h (since
# it is #included from str.h). The compiler generated dependencies will cause
# the right .o's to get recompiled if the generated.h file changes. Adding
# an order-only dependency to all of the .o's will cause the generated .h
# to get built before we try to compile any of them.
$(COMPONENT_OBJS): | $(HEADER_BUILD)/qstrdefs.generated.h $(HEADER_BUILD)/mpversion.h

$(HEADER_BUILD)/qstr.i.last: $(SRC_QSTR) | $(HEADER_BUILD)/mpversion.h
	$(summary) "GEN $@"
	$(CPP) $(QSTR_GEN_EXTRA_CFLAGS) $(CFLAGS) $(if $?,$?,$^) >$(HEADER_BUILD)/qstr.i.last;

$(HEADER_BUILD)/qstr.split: $(HEADER_BUILD)/qstr.i.last
	$(summary) "GEN $@"
	$(PYTHON) $(ABS_PY_SRC)/makeqstrdefs.py split $(HEADER_BUILD)/qstr.i.last $(HEADER_BUILD)/qstr $(QSTR_DEFS_COLLECTED)
	touch $@

$(QSTR_DEFS_COLLECTED): $(HEADER_BUILD)/qstr.split
	$(summary) "GEN $@"
	$(PYTHON) $(ABS_PY_SRC)/makeqstrdefs.py cat $(HEADER_BUILD)/qstr.i.last $(HEADER_BUILD)/qstr $(QSTR_DEFS_COLLECTED)
	
$(HEADER_BUILD)/qstrdefs.generated.h: $(PY_QSTR_DEFS) $(QSTR_DEFS) $(QSTR_DEFS_COLLECTED) $(ABS_PY_SRC)/makeqstrdata.py $(COMPONENT_PATH)/inc/mpconfigport.h $(ABS_PY_SRC)/mpconfig.h | $(HEADER_BUILD) 
	$(summary) "GEN $@"
	cat $(PY_QSTR_DEFS) $(QSTR_DEFS) $(QSTR_DEFS_COLLECTED) | $(SED) 's/^Q(.*)/"&"/' | $(CPP) $(QSTR_INCLUDE_FLAGS) $(CFLAGS) - | $(SED) 's/^"\(Q(.*)\)"/\1/' > $(HEADER_BUILD)/qstrdefs.preprocessed.h
	$(PYTHON) $(ABS_PY_SRC)/makeqstrdata.py $(HEADER_BUILD)/qstrdefs.preprocessed.h > $@

$(HEADER_BUILD)/mpversion.h: FORCE | $(HEADER_BUILD)
	cd $(ABS_PY_SRC) && $(PYTHON) $(ABS_PY_SRC)/makeversionhdr.py $@

$(HEADER_BUILD):
	$(MKDIR) -p $@
	
$(GENSRC_BUILD):
	$(MKDIR) -p $@

## Frozen stuff

ifneq ($(FROZEN_DIR),)
$(GENSRC_BUILD)/frozen.c: $(wildcard $(FROZEN_DIR)/*) $(HEADER_BUILD) $(FROZEN_EXTRA_DEPS)
	$(summary) "Generating $@"
	$(MAKE_FROZEN) $(FROZEN_DIR) > $@
endif

ifneq ($(FROZEN_MPY_DIR),)
# to build the MicroPython cross compiler
$(TOP)/mpy-cross/mpy-cross: $(TOP)/py/*.[ch] $(TOP)/mpy-cross/*.[ch] $(TOP)/ports/windows/fmode.c
	$(MAKE) -C $(TOP)/mpy-cross

# make a list of all the .py files that need compiling and freezing
FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py' | $(SED) -e 's=^$(FROZEN_MPY_DIR)/==')
FROZEN_MPY_MPY_FILES := $(addprefix $(FROZEN_MPY_BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))

# to build .mpy files from .py files
$(FROZEN_MPY_BUILD)/%.mpy: $(FROZEN_MPY_DIR)/%.py $(TOP)/mpy-cross/mpy-cross $(FROZEN_MPY_BUILD)
	$(summary) "MPY $<"
	$(MKDIR) -p $(dir $@)
	$(MPY_CROSS) -o $@ -s $(<:$(FROZEN_MPY_DIR)/%=%) $(MPY_CROSS_FLAGS) $<

# to build frozen_mpy.c from all .mpy files
$(GENSRC_BUILD)/frozen_mpy.c: $(FROZEN_MPY_MPY_FILES) $(HEADER_BUILD)/qstrdefs.generated.h $(GENSRC_BUILD)
	$(summary) "Creating $@"
	$(PYTHON) $(MPY_TOOL) -f -q $(HEADER_BUILD)/qstrdefs.preprocessed.h $(FROZEN_MPY_MPY_FILES) > $@
	
$(FROZEN_MPY_BUILD):
	$(MKDIR) -p $@
endif

# This must come AFTER the qstr stuff to avoid circular dependencies
# TODO(supersat): Move this to the build directory?
COMPONENT_OBJS += src/gensrc/frozen_mpy.o

# Anything that depends on FORCE will be considered out-of-date
FORCE:
.PHONY: FORCE
