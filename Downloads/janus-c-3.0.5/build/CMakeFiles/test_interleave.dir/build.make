# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ttm4128/Downloads/janus-c-3.0.5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ttm4128/Downloads/janus-c-3.0.5/build

# Include any dependencies generated for this target.
include CMakeFiles/test_interleave.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_interleave.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_interleave.dir/flags.make

CMakeFiles/test_interleave.dir/tests/c/interleaver.c.o: CMakeFiles/test_interleave.dir/flags.make
CMakeFiles/test_interleave.dir/tests/c/interleaver.c.o: ../tests/c/interleaver.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ttm4128/Downloads/janus-c-3.0.5/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test_interleave.dir/tests/c/interleaver.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_interleave.dir/tests/c/interleaver.c.o   -c /home/ttm4128/Downloads/janus-c-3.0.5/tests/c/interleaver.c

CMakeFiles/test_interleave.dir/tests/c/interleaver.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_interleave.dir/tests/c/interleaver.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ttm4128/Downloads/janus-c-3.0.5/tests/c/interleaver.c > CMakeFiles/test_interleave.dir/tests/c/interleaver.c.i

CMakeFiles/test_interleave.dir/tests/c/interleaver.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_interleave.dir/tests/c/interleaver.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ttm4128/Downloads/janus-c-3.0.5/tests/c/interleaver.c -o CMakeFiles/test_interleave.dir/tests/c/interleaver.c.s

# Object files for target test_interleave
test_interleave_OBJECTS = \
"CMakeFiles/test_interleave.dir/tests/c/interleaver.c.o"

# External object files for target test_interleave
test_interleave_EXTERNAL_OBJECTS =

test_interleave: CMakeFiles/test_interleave.dir/tests/c/interleaver.c.o
test_interleave: CMakeFiles/test_interleave.dir/build.make
test_interleave: libjanus.a
test_interleave: /usr/local/lib/libfftw3.a
test_interleave: CMakeFiles/test_interleave.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ttm4128/Downloads/janus-c-3.0.5/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test_interleave"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_interleave.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_interleave.dir/build: test_interleave

.PHONY : CMakeFiles/test_interleave.dir/build

CMakeFiles/test_interleave.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_interleave.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_interleave.dir/clean

CMakeFiles/test_interleave.dir/depend:
	cd /home/ttm4128/Downloads/janus-c-3.0.5/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ttm4128/Downloads/janus-c-3.0.5 /home/ttm4128/Downloads/janus-c-3.0.5 /home/ttm4128/Downloads/janus-c-3.0.5/build /home/ttm4128/Downloads/janus-c-3.0.5/build /home/ttm4128/Downloads/janus-c-3.0.5/build/CMakeFiles/test_interleave.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_interleave.dir/depend

