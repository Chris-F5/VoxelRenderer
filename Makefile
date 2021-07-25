OUTPUTNAME = VoxelRenderer
CC = gcc
CFLAGS = -g -D DEBUG
LDFLAGS = -l glfw -l vulkan
SRCS = $(shell find ./src -type f -name "*.c")
HEADERS = $(shell find ./src -type f -name "*.h")
OBJS = $(patsubst ./src/%.c, obj/%.o, $(SRCS))
DEPENDS = $(patsubst ./src/%.c, obj/%.d,$(SRCS))

-include $(DEPENDS)

.PHONY: run clean all

all: target/$(OUTPUTNAME)

target/$(OUTPUTNAME): $(OBJS) $(HEADERS)
	mkdir -p target
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

obj/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(LDFLAGS)

target/%.spv: src/%.comp
	glslc $< -o $@

run: all
	cd target; ./$(OUTPUTNAME)
clean:
	rm -fr target obj
