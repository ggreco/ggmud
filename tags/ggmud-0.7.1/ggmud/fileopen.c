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

#ifdef WIN32
#include <shlobj.h>
#define mkdir(x, y) _mkdir(x)
#endif

int check_ggmud_dir (gchar *dirname, int silent) {
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
        if (!mkdir(dirname, 0777)) {
        /* this isn't dangerous, umask modifies it */
            if (!silent) {
                popup_window (INFO, "GGMud settings directory <b>%s</b> created.", 
                        dirname);
                do_manual();
            }
        } else {
            popup_window (ERR, "%s NOT created: %s", dirname, strerror (errno));
            return_val = errno;
        }
    }
    return (return_val);
}

#include <stdarg.h>

void 
migrate_config(const char *dest_path, ...)
{
    static int detected = 0;
    char dest[255];
    char buffer[512];
    int len;
    va_list va;
    char *file;
    FILE *fp, *out;
    va_start(va, dest_path);

    while ((file = va_arg(va, char *))) {

        if ((fp = fopen(file, "rb"))) {
            if (!detected) {
                popup_window (INFO, 
                        "GGMud 0.7+ has changed the directory where settings are stored in Windows, "
                        "the change has been made to handle different configurations for different users on the same machine.\n\n"
                        "Since some existing settings have been found in your program directory,\n"
                        "GGMud will NOW move them in <b>%s</b>.\n"
                        "If you use a LUA startup script you'll have to move it by hand in the new directory.\n\n"
                        "A copy of the original file are still present in the directory "
                        "<b>disabled</b> that you can find in the GGMud installation directory."
                        , dest_path);

                mkdir("disabled", 0777);
                detected = 1;
            }
            g_snprintf(dest, sizeof(dest), "%s/%s", dest_path, file);

            if ((out = fopen(dest, "wb"))) {
                while ( (len = fread(buffer, 1, sizeof(buffer), fp)) > 0)
                    fwrite(buffer, 1,  len, out);

                fclose(out);
            }
            fclose(fp);

            g_snprintf(dest, sizeof(dest), "disabled/%s", file);
            rename(file, dest);
        }
    }
    va_end(va);
}

FILE *fileopen (gchar *fname, gchar *mode) {
/* Does all necessary checks and tries to open the specified file */
    FILE *fp;
    gchar path[256] = "";
    gchar *home;
#ifndef WIN32
    gchar *dir = "/.ggmud";

    home = getenv ("HOME");
    g_snprintf (path, sizeof(path), "%s%s", home!= NULL ? home : "", dir);
    if (check_ggmud_dir(path, 0)) return NULL;
#else
    static int check_migration = 0;
    gchar *dir = "/ggmud";
    TCHAR szPath[MAX_PATH];

    if(SUCCEEDED(SHGetFolderPath(NULL, 
                             CSIDL_APPDATA, 
                             NULL, 
                             0, 
                             szPath))) {
        home = szPath;

        // version 0.7+ changes the location of the configuration files
        // this routine perform a migration
        if (!check_migration) {
            g_snprintf(path, sizeof(path), "%s/%s", szPath, dir);
            if (check_ggmud_dir(path, 1)) return NULL;

            migrate_config(path,
                    "triggers", "aliases", "gag", "font", "macro",
                    "connections", "complete", "highlight",  "ggmud.prf",
                    "Preference", "variable", "winpositions",
                    NULL);
            check_migration = 1;
        }
    }
    else {
        home = NULL;
        strcpy(path, ".");
    }
#endif
    g_snprintf (path, sizeof(path), "%s%s/%s", home!= NULL ? home : "", dir, fname);
    fp = fopen (path, mode);
    return fp;
}
