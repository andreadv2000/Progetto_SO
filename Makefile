
#SERVER COMPILE
# put here the file containing the main routine
# to be uploaded on the avr
# you can add multiple files, they will be all generated
BINS = server.elf

# put here the additional .o files you want to generate
# one .c file for each .o should be present
OBJS = ./avr_common/uart.o

# put here the additional header files needed for compilation
HEADERS = ./avr_common/uart.h

# ---------------------
# Include dependencies
# ---------------------
include ./avr_common/server.mk
include client.mk


