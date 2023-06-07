/*=========================================================================

  ws2txt

  main.c

  Copyright (c)2022-3 Kevin Boone, GPLv3.0

=========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unixio.h>
#include <conio.h>
#include <string.h>

#include "getopt.h"
#include "defs.h"
#include "compat.h"
#include "romwbw.h"

/* CP/M pads files with ctrl-z */
#define CTRL_Z 26 
#define CTRL_S 19 
#define CTRL_Q 17 

/* Input buffer 
 * This buffer could be an automatic variable in modern C. But it's
 * too big for the stack in CP/M. We could malloc() it in main() or
 * something, but that would mean including all the memory allocator
 * code in the binary. */
char buff[CPMBLK];

/* ROMWBW character unit to write to (0, 1,...) Default to console */
int port = 0x80; 

/*=========================================================================

  show_help 

=========================================================================*/
void show_help (void)
  {
  fputs ("Usage: rwbwsend {/hv} {/c N} {file1} [file2...]\n", stdout);
  fputs ("   /c N          Write to character unit N (default 128)\n", stdout);
  fputs ("   /h            Show this help message\n", stdout);
  fputs ("   /v            Show version information\n", stdout);
  }

/*=========================================================================

  show_version

=========================================================================*/
void show_version (void)
  {
  fputs ("rwbwsend version 0.1\n", stdout);
  fputs ("Copyright (c)2023 Kevin Boone\n", stdout);
  fputs ("Distributed under the terms of the GNU Public Licence, v3.0\n", stdout);
  }


/*=========================================================================

  do_char

  Process the next char from the input file, and write it, with 
    transformation if necessary

=========================================================================*/
void do_char (int c)
  {
  int n;
  fwrite (&c, 1, 1, stdout); 
  rwbw_cout (port, c);
  n = rwbw_cstat (port);
  if (n > 0)
    {
    int i = rwbw_cin (port);
    if (i == CTRL_S)
      {
      do 
        {
        i = rwbw_cin (port);
        } while (i != CTRL_Q);
      }
    }
  }


/*=========================================================================

  do_file

=========================================================================*/
void do_file (char* infile)
  {
  int f_in;
  f_in = open (infile, O_RDONLY);
  if (f_in >= 0)
    {
    BOOL eof = FALSE;
    while (!eof)
      {
      int i, n = read (f_in, buff, CPMBLK);  
      if (n > 0)
        {
        for (i = 0; i < n; i++)
          {
          char c = buff[i];
          if (c == CTRL_Z)
            eof = TRUE;
          else
            do_char (c);
          }
        }
      else
       eof = TRUE;
      }
    close (f_in);
    }
  else
    {
    fputs ("Can't open '", stdout);
    fputs (infile, stdout);
    fputs ("' for reading\n", stdout);
    }
  }

/*=========================================================================

  check_romwbw 

=========================================================================*/
void check_romwbw (void)
  {
  if (rwbw_chk() <= 0)
    {
    fputs ("No ROMWBW detected.\n", stdout);
    exit (0);
    }
  }

/*=========================================================================

  main

=========================================================================*/
int main (int argc, char **argv)
  {
  int opt;
  BOOL opt_version = FALSE;

  check_romwbw();

  /* argv[0] is not set by the HI-TECH C compiler */
  argv[0] = "rwbwsend";

  while ((opt = getopt (argc, argv, "?hHvVc:C:")) != -1)
    {
    switch (opt)
      {
      case '?':
      case ':':
      case 'h':
      case 'H':
        show_help ();
        return 0; 
      case 'v': case 'V':
        opt_version = TRUE;
        break;
      case 'c': case 'C':
	port = atoi (optarg);
	break;
      default:
        return 1;
      }
    }

  if (opt_version)
    {
    show_version();
    }
  else if (argc == optind)
    {
    show_help();
    }
  else 
    {
    int i;
    for (i = optind; i < argc; i++)
    do_file (argv[i]);
    }

  return 0;
  }

