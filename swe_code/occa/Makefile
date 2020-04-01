include ../../include.mk

INCLUDE = -I./include $(INCLUDE_COMMON) 

OCC_OBJS = RK_occa.o buffers.o
#device_setup.o RK_occa.o buffers.o

OCC_LIB = swe_occa.a

all: $(OCC_LIB)

$(OCC_LIB): $(OCC_OBJS)
	$(LD_R) $^ -o $@
	@$(MAKE) clean

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o

