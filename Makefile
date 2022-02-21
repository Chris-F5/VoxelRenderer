OUTPUTNAME = VoxelRenderer
CC = gcc
CFLAGS = -g -D DEBUG -I src
LDFLAGS = -l glfw -l vulkan -l cglm -l m
SRCS = $(shell find ./src -type f -name "*.c")
HEADERS = $(shell find ./src -type f -name "*.h")
OBJS = $(patsubst ./src/%.c, obj/%.o, $(SRCS))
DEPENDS = $(patsubst ./src/%.c, obj/%.d,$(SRCS))

.PHONY: all run clean debug valgrind

REQUIREDSHADERS = target/vox_tri.vert.spv \
				  target/vox_tri.frag.spv \
				  target/debug_line.vert.spv \
				  target/debug_line.frag.spv \
				  target/chunk_lighting.comp.spv \
				  target/normal_gen.comp.spv

SHADERLIBS = src/shaders/chunk.glsl

all: target/$(OUTPUTNAME) $(REQUIREDSHADERS) target/object1.ply target/object1.svo

-include $(DEPENDS)

target/$(OUTPUTNAME): $(OBJS) $(HEADERS)
	mkdir -p target
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

obj/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(LDFLAGS)

target/%.spv: src/shaders/% $(SHADERLIBS)
	glslc $< -o $@

target/%.voxobj: %.voxobj
	mkdir -p target
	cp $< $@

target/%.ply: %.ply
	mkdir -p target
	cp $< $@

target/%.svo: %.svo
	mkdir -p target
	cp $< $@

run: all
	cd target; ./$(OUTPUTNAME)

debug: all
	cd target; gdb $(OUTPUTNAME)

valgrind: all
	cd target; valgrind ./$(OUTPUTNAME)

clean:
	rm -fr target obj
