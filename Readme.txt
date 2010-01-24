
PostHaste: Native plugins from RenderMan shaders via LLVM
---------------------------------------------------------

Developed by Mark Leone (markleone@gmail.com)
Distributed under the MIT License:  see License.txt
Hosted at github:  http://github.com/MarkLeone/PostHaste

Introduction
------------
PostHaste is a compiler that reads a compiled RenderMan shader (in SLO form),
identifies simple computational kernels, and compiles them to native code in
the form of a shader plugin (DSO).  LLVM is used for optimization and native
code generation.  The shader is rewritten, replacing the kernel code with DSO
calls.  Compiling shader kernels eliminates interpretive overhead, greatly
reduces memory traffic, and increases instruction-level parallelism,

A primary feature of this work is robustness: any shading language features
that cannot be handled by the PostHaste compiler remain in the residual shader
and are handled by the shader interpreter.  The goal is to accelerate complex
production shaders while minimizing impact on production pipelines.

Limitations
-----------
The benefits are currently quite modest, and additional work is required to
make this system production ready.  In preliminary experiments with "textbook"
shaders, compiled kernels typically run 30-50% faster than interpreted code.
The overhead of shader plugin calls is substantial, however, and kernels with
fewer than 30 instructions rarely benefit from compilation.  Amdahl's law also
applies: the overall performance benefit is limited by the fraction of time
spent in simple computational kernels.  Procedural shaders will gain the most,
but shaders that are dominated by texturing will show little improvement.

The analysis that is currently used to extract kernels is quite simplistic.
Only contiguous sequences of varying instructions that can be entirely
compiled are used to form kernels.  Bigger kernels could be extracted by
reordering code, but doing so requires data and control dependence analysis,
which is future work.

PostHaste currently has no cost-benefit analysis that prevents compilation of
kernels that are too small to recover the overhead of a DSO call. The minimum
kernel size can be specified as a PostHaste command-line option.

Prerequisites
-------------
PostHaste builds under OSX and Linux.  There are no XCode or Visual Studio
project files, but they would be straightforward to create.

First, install LLVM.  A debug build is recommended.  For more information, see
http://llvm.org/docs/GettingStarted.html

1. Download http://llvm.org/releases/2.6/llvm-2.6.tar.gz
2. gunzip -c llvm-2.6.tar.gz | tar -vxf -
3. cd llvm-2.6
4. ./configure --enable-optimized=no
5. make
6. sudo make install

Next, install llvm-gcc.  It's easiest to use prebuilt binaries:

- OSX: http://llvm.org/releases/2.6/llvm-gcc-4.2-2.6-i386-darwin9.tar.gz 
- Linux x86: http://llvm.org/releases/2.6/llvm-gcc-4.2-2.6-i686-linux.tar.gz
- Linux x64: http://llvm.org/releases/2.6/llvm-gcc-4.2-2.6-x86_64-linux.tar.gz

Optionally, install the Google Test framework (for PostHaste unit tests):

1. Download http://googletest.googlecode.com/files/gtest-1.3.0.tar.gz
2. gunzip -c gtest-1.3.0.tar.gz | tar -vxf -
3. cd gtest-1.3.0
4. ./configure
5. make
6. sudo make install

The PostHaste Makefiles assume that LLVM and gtest are installed in standard
system directories.  That can be changed by add appropriate include and
library paths in build/Makefile.common.

Building PostHaste
------------------

1. cd PostHaste
2. make

The command-line executable is written to a platform-specific subdirectory in
PostHaste/inst, for example:

   PostHaste/inst/osx-x86/debug/bin/posthaste

Running PostHaste
-----------------

The "posthaste" command-line executable takes an SLO and produces LLVM bitcode
for a shader plugin, along with modified SLO that calls the plugin.  The
output files are written to a subdirectory called "posthaste" to avoid
overwriting input files.  The input SLO must be compiled with "shader -back",
since PostHaste cannot parse SLO version 5 or higher.

1. shader -back -O2 test.sl
2. posthaste -O2 test.slo

[linux] 3. llc -march=x86-64 -relocation-model=pic posthaste/test.bc
[linux] 4. gcc -m64 -fPIC -shared -o posthaste/test.so posthaste/test.s

[osx] 3. llc -march=x86-64 posthaste/test.bc
[osx] 4. gcc -m64 -bundle -undefined dynamic_lookup -o posthaste/test.so posthaste/test.s

To use the resulting shader, add the "posthaste" subdirectory to the shader
search path, for example by adding the following to your RIB file:

	Option "searchpath" "shader" ["posthaste:&"]
