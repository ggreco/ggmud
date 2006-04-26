/*  GGMud
 *  Copyright (C) 1999 Heathen (the.heathen@swipnet.se)
 *                1999 Drizzt  (doc.day@swipnet.se)
 *                2003 Gabry (gabrielegreco@gmail.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "ggmud.h"

#ifndef WIN32

int check_sclient_dir (gchar *dirname) {
/* check if the specified directory exists, try to create it if it doesn't */
    struct stat file_stat;
    int return_val = 0;
            
    if (!stat(dirname, &file_stat)) {
    /* can we stat ~/.sclient? */
    	if (!S_ISDIR(file_stat.st_mode)) {
    	/* if it's not a directory */
            popup_window (ERR, "%s already exists and is not a directory!", dirname);
        }
    } else {
    /* it must not exist */
        popup_window (INFO, "%s does not exist, Creating it as a directory.", dirname);
        if (!mkdir(dirname, 0777)) {
        /* this isn't dangerous, umask modifies it */
            popup_window (INFO, "%s created.", dirname);
            do_manual();
        } else {
            popup_window (ERR, "%s NOT created: %s", dirname, strerror (errno));
            return_val = errno;
        }
    }
    return (return_val);
}
#endif

FILE *fileopen (gchar *fname, gchar *mode) {
/* Does all necessary checks and tries to open the specified file */
#ifndef WIN32
    gchar *dir = "/.ggmud";
    gchar *home;
    gchar path[256] = "";
    FILE *fp;

    home = getenv ("HOME");
    g_snprintf (path, 255, "%s%s", home!= NULL ? home : "", dir);
    if (check_sclient_dir(path)) return NULL;
    g_snprintf (path, 255, "%s%s/%s", home!= NULL ? home : "", dir, fname);
    fp = fopen (path, mode);
    return fp;
#else
    return fopen(fname, mode);
#endif
}
