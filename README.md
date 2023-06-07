# rwbwsend 

Version 0.1

## What is this?

`rwbwsend` is a simple CP/M utility for systems with ROMWBW firmware, that
sends text files to a numbered serial port, respecting software flow
control. I wrote it for the specific task of sending text files to
a slow, serial printer, when the serial port does not
support hardware flow control (CST/RTS, etc). This lack of support 
for hardware flow control might be because the serial peripheral 
device does not support it (e.g., the Z180 CPU's second serial port)
or because the cabling or ports don't have the connections
in place. 

It's usual these days, when a CP/M machine is part of a larger
installation, for the CP/M machine to be the slowest part of the
system. This is not true when a vintage printer is involved. Even the slowest
CP/M machine will easily overwhelm a dot-matrix printer. Some kind of
flow control is therefore essential, so that the printer can regulate
the flow of incoming data. 

## Why does this only work on ROMWBW machines?

Standard CP/M provides no platform-independent way to read data from
a printer port, only to write it. There is therefore no platform-independent
way to use XON/XOFF flow control with CP/M, other than with the 
primary console -- for which both read and write functions are provided
in the operating system.  

If `rwbwsend` is used on a CP/M machine without ROMWBW firmware, it will
print an error message and stop.

## Usage

    rwbwsend [/v] [/h] [/c N] {text files...} 

The switches are case-insensitive (which is unavoidable on CP/M), and
you can use `-` rather than `/` if preferred. 

`/h` shows the help message; `/v` shows the version. 

`/c N` sets the serial port number. ROMWBW refers to these ports as
`com0`, `com1`, etc., but only the number should be used with
`rwbwsend`. The `mode` utility, supplied with ROMWBW, lists
the installed serial ports.

`rwbwsend` doesn't check that the number corresponds to a real device.
If it doesn't, the BIOS sends the data to the console, which is what
also will happen if `/c` is not given.

## Building

`rwbwsend` can be built on CP/M or on Linux, using a CP/M emulator. 
Whether building on Linux or CP/M, the utility is written for 
version 3.09 of HI-TECH C for Z80, which is maintained by
Tony Nicholson, and available in his GitHub repository:

https://github.com/agn453/HI-TECH-Z80-C

Most likely the original HI-TECH distribution would work as well, but
you would need to change `linq` to `link` in the `Makefile`.

### Building on CP/M

You'll need to install the HI-TECH C binaries on a drive, and the
program source in (ideally) a different drive. Tony Nicholson's
version of the compiler is designed (I think) to be installed on 
drive C:, rather than the A: of the original. To use a different
drive than C:, you can create a file `ENVIRON` in the drive
that contains the source code. This file should indicate where the
compiler is installed. So, with the compiler files on drive H:, and
program files on B:, `B:ENVIRON` should contain: 

    HITECH=H:

Then you should be able to build `rwbwsend.com` by renaming 
`romwbw.zas` to `romwbw.as` and then: 

    B> H:c309 -cpm -o rwbwsend.com romwbw.as getopt.c compat.c

The rename is necessary because I use the suffix `zas` for my assembler
source files, to distinguish them from the assembly-language files generated
by the compiler. However, the `c309` command doesn't recognize `.zas`
files as assembler sources.

Note that this one-step build only works on CP/M implementations that
have a working 'SUBMIT' mechanism (CP/M emulators typically do not).

### Building on Linux for CP/M

The Linux build for CP/M is controlled by the `Makefile`. This
chains together all the various build steps using the HI-TECH
tools. A bunch of shell scripts run these tools in ways that can make
sense in a `Makefile`. This use of scripts, while unavoidable, makes
the build process more complicated than it really ought to be.
An additional problem is that CP/M can only accept a 127-character
command line. So the build process renames all the `.obj` files to
single-number names, so that the `linq` command line is not too long
when there are many command-line arguments. 

The build process uses Joseph Allen's CP/M emulator for Linux. It is
available here:

https://codeload.github.com/jhallen/cpm/zip/refs/heads/master 

You'll need to build and install this before building `rwbwsend`.

You can copy  the HI-TECH C binaries directly into the program's source
directory. Alternatively, if you have `lbrate` on your system, you
can do `make prepare` to download and unpack the latest version
of the HI-TECH tools from GitHub.  

`make unprepare` will remove the HI-TECH tools without cleaning
anything else.

The file `ENVIRON` in the source directory is necessary, because
the CP/M emulator has only an 'A' drive, so the HI-TECH C compiler
must look on `A:`, regardless of its defaults.

All being well, running `make` should do everything that is required.
Even under emulation, its about a hundred times faster to build on
a modern Linux system than on CP/M -- but a key part of this whole
exercise is that the source code is the same, and the utility 
_will_ build on CP/M.

## Limitations

If the remote system sends XOFF, the utility will wait potentially
forever for it to send XON.

There is no progress indication.

The transfer cannot be interrupted from the console.

Because CP/M has no length indication in its files, text files are padded
to a whole number of records with ctrl-z characters. As a result, 
`rwbwsend` treats ctrl-z as an end-of-file marker. Consequently, it cannot
be used to send any file that might contain a ctrl-z before the end.
Text files, on the whole, do not; other files types might.

When sending data to a printer, the `rwbwsend` utility will run for
as long as it takes to print. It makes sense that it should do so,
but that isn't behaviour we're used to seeing with modern printers.

