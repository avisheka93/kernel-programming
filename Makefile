# Makefile – makefile of our first driver
  
# if KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq (${KERNELRELEASE},)
	obj-m += test-usb-driver.o
# Otherwise we were called directly from the command line.
# Invoke the kernel build system.
else
	KVER := $(shell uname -r)
	KERNEL_SOURCE := /usr/src/linux-headers-${KVER}
	PWD := $(shell pwd)
all:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
              
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif
