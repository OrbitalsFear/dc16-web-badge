#
# All application Makefiles should define the following macros:
#
#   NETOS_DIR       :  relative path to the "netos" toplevel
#   BUILD_RAM_IMAGE :  set to "0" if not desired, and to "1"
#                      if desired (and supported by platform)
#   BUILD_ROM_IMAGE :  set to "0" if not desired, and to "1"
#                      if desired (and supported by platform)
#   APP_C_FILES     :  list of application specific "C" files
#                      (must all have a ".c" suffix)
#   APP_C_FLAGS     :  application specific CFLAGS
#   APP_CC_FILES    :  list of application specific "C++" files
#                      (must all have either a ".cc", ".C", or ".cxx" suffix)
#   APP_CC_FLAGS    :  application specific CFLAGS for C++ compilation
#   APP_ASM_FILES   :  list of application specific assembly files
#                      (must all have either a ".s", ".S", or ".arm" suffix)
#   APP_ASM_FLAGS   :  application specific ASFLAGS for assembly
#   APP_INCDIRS     :  list of application specific include directories
#                      in "-I.." format.
#
#   APP_DESCRIPTION :  Simple one line description of application
#                      (for applicable platforms, will be incorporated
#                      into the binary VPD)
#
#   USE_SIMPLE_WEB_SERVER : set to "1" if desired (used for compatibility
#                           with older examples... links against a different
#                           set of libraries)
#
#   INCLUDE_CXX_STARTUP : set to "1" to include C++ initialization libraries
#
#   APP_OBJ_FILES   :  An optional veriable which can specify "extra"
#                      objects to be linked into an image which are
#                      not derived from c, c++, or assembly.  The
#                      application Makefile bears the burden of providing
#                      rules to build these files if necessary.
#
# The application Makefile should also include VPATH specifications
# for all application source.
#

NETOS_DIR= ../../../..

BUILD_RAM_IMAGE= 1
BUILD_ROM_IMAGE= 1

APP_DESCRIPTION="NASerial Example Application"

vpath %.c ..

APP_C_FILES=	main.c tmp_ftp_server.c 

APP_CC_FILES=
APP_ASM_FILES=

# APP_C_FLAGS=-DOLD_SERIAL_API
APP_C_FLAGS=

APP_CC_FLAGS=
APP_ASM_FLAGS=

APP_INCDIRS= 	-I.. \
             	-I../../bsp/platforms/$(PLATFORM)


# List of libraries needed for the application.
APP_LIBS =  $(NETOS_BSPLIBPATH)/libbsp.a \
            $(NETOS_LIBPATH)/libftpsvr.a \
            $(NETOS_LIBPATH)/libfsock.a \
            $(NETOS_LIBPATH)/libflash.a \
            $(NETOS_LIBPATH)/libposix.a \
            $(NETOS_LIBPATH)/libtx.a \
            $(NETOS_LIBPATH)/libdnsclnt.a \
            $(NETOS_LIBPATH)/libfastip.a \
            $(NETOS_LIBPATH)/libemailc.a \
            $(NETOS_LIBPATH)/librphttpd.a \
            $(NETOS_LIBPATH)/librphttpddbg.a \
            $(NETOS_LIBPATH)/libfilesys.a \
            $(NETOS_LIBPATH)/libfilesysdbg.a \
            $(NETOS_LIBPATH)/libmanapi.a \
            $(NETOS_LIBPATH)/libaddp.a \
            $(NETOS_LIBPATH)/libsntp.a \
            $(NETOS_LIBPATH)/libtcpip.a \
            $(NETOS_LIBPATH)/libcrypto.a \
            $(NETOS_LIBPATH)/libssl.a 

#ifneq ($(findstring $(PLATFORM), connectsp),)
#$(error This application is not supported for the specified platform, check readme for more information)endifAPP_DESCRIPTION="NASerial Example Application")
#endif

ifneq ($(findstring $(PLATFORM), connectwime connectwiem),)
APP_LIBS += $(NETOS_LIBPATH)/libwpa.a $(NETOS_LIBPATH)/libcrypto.a $(NETOS_LIBPATH)/libssl.a 
endif


#
# All "required" elements must be supplied prior to including the
# "core" Makefile and targets.
#

include $(NETOS_DIR)/src/linkerScripts/Makefile.app.inc


ifeq "$(PLATFORM)" "ns9360_a"
C_DEFS += -DODIC_9360
endif

#
# Application specific macros and targets can go below here.
#


