/*
TinTin++
Copyright (C) 2001 Davin Chan, Robert Ellsworth, etc. (See CREDITS file)

This program is protected under the GNU GPL (See COPYING)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
/* New for v2.0: readline support -- daw */

/* i put the logging crap in it's own file, in case we wanna change it later */

#include "config.h"
#include "tintin.h"

#include "include/log.h"

void logit(struct session *s, const char *b)
{
  static char bb[BUFFER_SIZE+1];
  int i, j;

  if(!s->logfile)
    return;

#if OLD_LOG
  fputs(b, s->logfile);
#else
  /* new logging behavior: ignore control-m's */
  for(i = j = 0; b[i]; i++)
    if(b[i] != '\r')
      bb[j++] = b[i];
  bb[j] = '\0';
  fputs(bb, s->logfile);
#endif
}
/* low-level logging - log rare bytes from mud. - ycjhi */
void lowlogit(struct session *s, const char *b, size_t nbytes)
{

  if(!s->lowlogfile)
    return;

  fwrite(b, 1, nbytes, s->lowlogfile);
  fprintf(s->lowlogfile, "(%d,%d,%d)\n", s->old_more_coming, s->more_coming, 
	  nbytes) ;
}
