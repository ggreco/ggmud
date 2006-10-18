#include <gtk/gtk.h>


gboolean
on_socks_ko_clicked                    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_socks_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_radiobutton_v4_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_radiobutton_v5_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_checkbutton_pwd_auth_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
