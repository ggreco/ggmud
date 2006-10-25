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

void
on_button_browse_lua_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_proxy_settings_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_preferences_ok_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_preferences_save_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_checkbutton_tickcounter_toggled     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_checkbutton_use_proxy_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_checkbutton_autologin_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_button_wiz_add_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_wiz_apply_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_wiz_delete_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_wiz_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_wiz_save__clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data);
