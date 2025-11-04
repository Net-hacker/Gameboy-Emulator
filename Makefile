CC_GCC = gcc
CC_CLANG = clang
INCLUDE_FILES = src/Include
CFLAGS_DEBUG = -lraylib -I $(INCLUDE_FILES)
CFLAGS_RELEASE = -lraylib -O2 -I $(INCLUDE_FILES)
INPUT_FILES = src/main.c src/rom/rom.c src/cpu/cpu.c src/debug/debug.c
OUTPUT_FILE_DEBUG = -o build/Debug/GB
OUTPUT_FILE_RELEASE = -o build/Release/GB

make_debug_gcc:
	$(CC_GCC) $(CFLAGS_DEBUG) $(INPUT_FILES) $(OUTPUT_FILE_DEBUG)

make_debug_clang:
	$(CC_CLANG) $(CFLAGS_DEBUG) $(INPUT_FILES) $(OUTPUT_FILE_DEBUG)

make_release_gcc:
	$(CC_GCC) $(CFLAGS_RELEASE) $(INPUT_FILES) $(OUTPUT_FILE_RELEASE)

make_release_clang:
	$(CC_CLANG) $(CFLAGS_RELEASE) $(INPUT_FILES) $(OUTPUT_FILE_RELEASE)

make_without_gui:
	$(CC_GCC) -I $(INCLUDE_FILES) $(INPUT_FILES) $(OUTPUT_FILE_DEBUG)
