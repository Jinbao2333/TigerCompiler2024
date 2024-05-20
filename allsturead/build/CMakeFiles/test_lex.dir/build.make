# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/stu/tiger-compiler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/stu/tiger-compiler/build

# Include any dependencies generated for this target.
include CMakeFiles/test_lex.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_lex.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_lex.dir/flags.make

../src/tiger/lex/lex.cc: ../src/tiger/lex/tiger.lex
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating ../src/tiger/lex/lex.cc, ../src/tiger/lex/scannerbase.h"
	cd /home/stu/tiger-compiler/src/tiger/lex && flexc++ tiger.lex

../src/tiger/lex/scannerbase.h: ../src/tiger/lex/lex.cc
	@$(CMAKE_COMMAND) -E touch_nocreate ../src/tiger/lex/scannerbase.h

../src/tiger/parse/parse.cc: ../src/tiger/parse/tiger.y
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating ../src/tiger/parse/parse.cc, ../src/tiger/parse/parserbase.h"
	cd /home/stu/tiger-compiler/src/tiger/parse && bisonc++ tiger.y

../src/tiger/parse/parserbase.h: ../src/tiger/parse/parse.cc
	@$(CMAKE_COMMAND) -E touch_nocreate ../src/tiger/parse/parserbase.h

CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o: ../src/tiger/main/test_lex.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o -c /home/stu/tiger-compiler/src/tiger/main/test_lex.cc

CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/main/test_lex.cc > CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.i

CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/main/test_lex.cc -o CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.s

CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o


CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o: ../src/tiger/symbol/symbol.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o -c /home/stu/tiger-compiler/src/tiger/symbol/symbol.cc

CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/symbol/symbol.cc > CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.i

CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/symbol/symbol.cc -o CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.s

CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o


CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o: ../src/tiger/absyn/absyn.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o -c /home/stu/tiger-compiler/src/tiger/absyn/absyn.cc

CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/absyn/absyn.cc > CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.i

CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/absyn/absyn.cc -o CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.s

CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o


CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o: ../src/tiger/errormsg/errormsg.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o -c /home/stu/tiger-compiler/src/tiger/errormsg/errormsg.cc

CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/errormsg/errormsg.cc > CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.i

CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/errormsg/errormsg.cc -o CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.s

CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o


CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o: ../src/tiger/env/env.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o -c /home/stu/tiger-compiler/src/tiger/env/env.cc

CMakeFiles/test_lex.dir/src/tiger/env/env.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/env/env.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/env/env.cc > CMakeFiles/test_lex.dir/src/tiger/env/env.cc.i

CMakeFiles/test_lex.dir/src/tiger/env/env.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/env/env.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/env/env.cc -o CMakeFiles/test_lex.dir/src/tiger/env/env.cc.s

CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o


CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o: ../src/tiger/semant/semant.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o -c /home/stu/tiger-compiler/src/tiger/semant/semant.cc

CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/semant/semant.cc > CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.i

CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/semant/semant.cc -o CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.s

CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o


CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o: ../src/tiger/semant/types.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o -c /home/stu/tiger-compiler/src/tiger/semant/types.cc

CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/semant/types.cc > CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.i

CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/semant/types.cc -o CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.s

CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o


CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o: ../src/tiger/lex/lex.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o -c /home/stu/tiger-compiler/src/tiger/lex/lex.cc

CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/lex/lex.cc > CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.i

CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/lex/lex.cc -o CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.s

CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o


CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o: CMakeFiles/test_lex.dir/flags.make
CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o: ../src/tiger/parse/parse.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o -c /home/stu/tiger-compiler/src/tiger/parse/parse.cc

CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stu/tiger-compiler/src/tiger/parse/parse.cc > CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.i

CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stu/tiger-compiler/src/tiger/parse/parse.cc -o CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.s

CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o.requires:

.PHONY : CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o.requires

CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o.provides: CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o.requires
	$(MAKE) -f CMakeFiles/test_lex.dir/build.make CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o.provides.build
.PHONY : CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o.provides

CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o.provides.build: CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o


# Object files for target test_lex
test_lex_OBJECTS = \
"CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o" \
"CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o" \
"CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o" \
"CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o" \
"CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o" \
"CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o" \
"CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o" \
"CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o" \
"CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o"

# External object files for target test_lex
test_lex_EXTERNAL_OBJECTS =

test_lex: CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o
test_lex: CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o
test_lex: CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o
test_lex: CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o
test_lex: CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o
test_lex: CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o
test_lex: CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o
test_lex: CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o
test_lex: CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o
test_lex: CMakeFiles/test_lex.dir/build.make
test_lex: CMakeFiles/test_lex.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/stu/tiger-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking CXX executable test_lex"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_lex.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_lex.dir/build: test_lex

.PHONY : CMakeFiles/test_lex.dir/build

CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/main/test_lex.cc.o.requires
CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/symbol/symbol.cc.o.requires
CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/absyn/absyn.cc.o.requires
CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/errormsg/errormsg.cc.o.requires
CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/env/env.cc.o.requires
CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/semant/semant.cc.o.requires
CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/semant/types.cc.o.requires
CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/lex/lex.cc.o.requires
CMakeFiles/test_lex.dir/requires: CMakeFiles/test_lex.dir/src/tiger/parse/parse.cc.o.requires

.PHONY : CMakeFiles/test_lex.dir/requires

CMakeFiles/test_lex.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_lex.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_lex.dir/clean

CMakeFiles/test_lex.dir/depend: ../src/tiger/lex/lex.cc
CMakeFiles/test_lex.dir/depend: ../src/tiger/lex/scannerbase.h
CMakeFiles/test_lex.dir/depend: ../src/tiger/parse/parse.cc
CMakeFiles/test_lex.dir/depend: ../src/tiger/parse/parserbase.h
	cd /home/stu/tiger-compiler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stu/tiger-compiler /home/stu/tiger-compiler /home/stu/tiger-compiler/build /home/stu/tiger-compiler/build /home/stu/tiger-compiler/build/CMakeFiles/test_lex.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_lex.dir/depend

