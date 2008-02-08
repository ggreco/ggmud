#include "http_fetcher.h"
#include "config.h"
#include <gtk/gtk.h>
#include "ggmud.h"

#ifdef WIN32
#define ARCH "win32"
#elif defined(__APPLE__)
#define ARCH "osx"
#elif defined(linux)
#define ARCH "linux"
#else
#define ARCH "other"
#endif

#ifdef __PPC__
#define CPUTYPE "ppc"
#elif defined(__x86_64)
#define CPUTYPE "x86-64"
#elif defined(__i386)
#define CPUTYPE "x86"
#else
#define CPUTYPE "unknown"
#endif

int need_update = FALSE;
char last_version[32];

void ask_for_update()
{
    GtkMessageDialog *d = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                           GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL,
                           "<b>There is a new version of GGMud available!</b>\n\n"
                           "The latest version of GGMud is <b>%s</b> and you are using <b>%s</b>,\n"
                           "can I open a browser window to download it?\n\n"
                           "(you can disable the remote version check and\n"
                           "this notification from the Preferences panel)",
                           last_version, VERSION);

    if (gtk_dialog_run(GTK_DIALOG(d)) == GTK_RESPONSE_OK) 
        openurl("http://sourceforge.net/project/showfiles.php?group_id=91114");
   
    gtk_widget_destroy(GTK_WIDGET(d));
}

void check_for_updates()
{
    char *fileBuf;
    char buffer[256];
    sprintf(buffer, "www.ggsoft.org/GGMudVersion.php?version=%s&arch=%s&cpu=%s",
            VERSION, ARCH, CPUTYPE);

    if( http_fetch(buffer, &fileBuf) != -1) {
        if (!strstr(fileBuf, "NEWEST")) {
            sscanf(fileBuf, "(%[^)])", last_version);
            need_update = TRUE;
        }
    }

    free(fileBuf);
}

