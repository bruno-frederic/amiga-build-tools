CC=vc
CONFIG=+kick13m
ODIR=build-vbcc
ADF=$(subst /,$(PATHSEP),uae/$(notdir $(CURDIR)).adf)   # <=> basename of current folder
UAE_CACHE_FILE=bin/configuration.cache bin/winuaebootlog.txt bin/default.uss bin/winuae_*.dmp
MAKEFILE_UPTODATE=$(ODIR)/Makefile.uptodate

# Prepare variables for target 'clean'
ifeq ($(OS),Windows_NT)
	RM:=del /F /Q
	TOUCH:=COPY /Y NUL
	PATHSEP:=\\
	CONFIG:=${CONFIG}_win
else
	RM:=rm -f
	TOUCH:=touch
	PATHSEP:=/
endif

all: $(MAKEFILE_UPTODATE) $(ADF)

uae/dh0/VersionExec : VersionExec.c kick-1_0-functions.h
	$(CC) $(CONFIG) -size $< -o $@

uae/dh0/VersionExec-debug : VersionExec.c kick-1_0-functions.h
	$(CC) $(CONFIG) -g -D_DEBUG $< -o $@

uae/dh0/Pause              : Pause.c
	$(CC) $(CONFIG) -size $< -o $@

uae/dh0/Pause-debug        : Pause.c
	$(CC) $(CONFIG) -g -D_DEBUG $< -o $@

uae/dh0/UAEexit      : UAEexit.c kick-1_0-functions.h uae_pragmas.h
	$(CC) $(CONFIG) -O3 $< -o $@

uae/dh0/UAEexit-debug: UAEexit.c kick-1_0-functions.h uae_pragmas.h
	$(CC) $(CONFIG) -g -D_DEBUG $< -o $@

$(ADF) : uae/dh0/VersionExec uae/dh0/VersionExec-debug \
         uae/dh0/Pause       uae/dh0/Pause-debug \
		 uae/dh0/UAEexit     uae/dh0/UAEexit-debug \
		 uae/dh0/s/Startup-Sequence
	-$(RM) $(ADF)
	exe2adf --directory uae/dh0 --label $(notdir $(CURDIR)) --adf $(ADF)

clean:
	-$(RM) $(subst /,$(PATHSEP),uae/dh0/*)
	-$(RM) $(ADF)
	-$(RM) $(subst /,$(PATHSEP),$(UAE_CACHE_FILE))
	-$(RM) $(subst /,$(PATHSEP),$(MAKEFILE_UPTODATE))

# Force clean when Makefile is updated :
$(MAKEFILE_UPTODATE): Makefile
	make clean
	$(TOUCH) $(subst /,$(PATHSEP),$@)
