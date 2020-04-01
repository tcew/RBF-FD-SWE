include ../../include.mk

INCLUDE = -I./include $(INCLUDE_COMMON)

OCCA_OBJS = device_setup.o RK_occa.o buffers.o

OCCA_LIB = swe_occa.a

all: $(OCCA_LIB)

$(OCCA_LIB): $(OCCA_OBJS)
	$(LD_R) $^ -o $@
	@$(MAKE) clean

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o

