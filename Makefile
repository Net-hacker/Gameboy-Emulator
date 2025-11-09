CC_GCC = gcc
CC_CLANG = clang
INCLUDE_FILES = src/Include
CFLAGS_DEBUG = -I $(INCLUDE_FILES)
CFLAGS_RELEASE = -O2 -I $(INCLUDE_FILES)
INPUT_FILES = src/main.c src/rom/rom.c src/cpu/cpu.c src/debug/debug.c
OUTPUT_FILE_DEBUG = -o build/Debug/GB
OUTPUT_FILE_RELEASE = -o build/Release/GB

gcc_debug:
	$(CC_GCC) $(CFLAGS_DEBUG) $(INPUT_FILES) $(OUTPUT_FILE_DEBUG)

clang_debug:
	$(CC_CLANG) $(CFLAGS_DEBUG) $(INPUT_FILES) $(OUTPUT_FILE_DEBUG)

gcc_release:
	$(CC_GCC) $(CFLAGS_RELEASE) $(INPUT_FILES) $(OUTPUT_FILE_RELEASE)

clang_release:
	$(CC_CLANG) $(CFLAGS_RELEASE) $(INPUT_FILES) $(OUTPUT_FILE_RELEASE)
