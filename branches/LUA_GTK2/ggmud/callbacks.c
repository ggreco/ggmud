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
on_checkbutton_use_proxy_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    gtk_widget_set_sensitive( lookup_widget((GtkWidget *)togglebutton,
                "button_socks_settings"),
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



