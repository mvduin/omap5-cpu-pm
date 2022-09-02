programs := omap5-cpu-pm

all :: ${programs}

clean ::
	${RM} ${programs}

${programs}: %: map-phys.o

# where to look for sources
vpath %.cc src


# all packages
declared_pkgs :=

# default packages
pkgs =


target-arch := arm-linux-gnueabihf
include common.mk
