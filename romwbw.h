/*===========================================================================

  romwbw.h

  Copyright (c)2022-3 Kevin Boone, GPL v3.0

===========================================================================*/

int rwbw_chk (void);
int rwbw_getrtc (char *buff);
int rwbw_cout (int unit, int c);
int rwbw_cstat (int unit);
/* Bottom byte is the character value; top byte an error code. */
int rwbw_cin (int unit);

