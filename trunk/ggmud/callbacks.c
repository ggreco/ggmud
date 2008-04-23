#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"


void
on_radiobutton_v4_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_hide(lookup_widget((GtkWidget *)button,
                "frame_pwd_auth"));
}


void
on_radiobutton_v5_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_show(lookup_widget((GtkWidget *)button,
                "frame_pwd_auth"));
}


void
on_checkbutton_pwd_auth_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    gtk_widget_set_sensitive(lookup_widget((GtkWidget *)togglebutton,
                "table_pwd_auth"),
            gtk_toggle_button_get_active(togglebutton));

}



void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(gtk_widget_get_toplevel((GtkWidget *)button));
}


void
on_checkbutton_tickcounter_toggled     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    gtk_widget_set_sensitive( lookup_widget((GtkWidget *)togglebutton,
                "spinbutton_ticklength"),
            gtk_toggle_button_get_active(togglebutton));
}

void
on_checkbutton_button_toggled          (GtkWidget *target,
                                        GtkToggleButton *togglebutton)
{
    gtk_widget_set_sensitive(target, 
            gtk_toggle_button_get_active(togglebutton));
}


void
on_checkbutton_autologin_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    gtk_widget_set_sensitive( lookup_widget((GtkWidget *)togglebutton,
                "table_autologin"),
            gtk_toggle_button_get_active(togglebutton));    
}

#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif

// open an URL, different for each operating system
void
openurl(const char *url)
{
#ifdef WIN32
    ShellExecute(GetActiveWindow(),
         "open", url, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "open %s", url);
    system(buffer);
#else
    char *apps[] = {"x-www-browser",
                    "firefox",
                    "opera",
                    "mozilla",
                    "konqueror", NULL};

    char buffer[256];
    int i = 0;

    while (apps[i]) {
        snprintf(buffer, sizeof(buffer), "which %s >/dev/null", apps[i]);
        if (system(buffer) == 0) {
            snprintf(buffer, sizeof(buffer), "%s %s", apps[i], url);
            system(buffer);
            return;
        }
        i++;
    }
#endif

}

void
on_documentation_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    openurl("http://www.ggsoft.org/ggmud/doc");
}


void
on_homepage_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    openurl("http://www.ggsoft.org/ggmud");
}

