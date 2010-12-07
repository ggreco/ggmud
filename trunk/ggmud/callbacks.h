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

void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_capture_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_macro_save_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_clist_macros_select_row             (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_clist_macros_unselect_row           (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_entry_shortcut_key_press_event      (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
on_me_button_add_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_me_button_mod_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_me_button_del_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
do_con                                 (GtkButton       *button,
                                        gpointer         user_data);

void
close_window_widget                    (GtkButton       *button,
                                        gpointer         user_data);

gboolean
quit                                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
do_wiz                                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
cbox                                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
disconnect                             (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
window_font                            (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
color_prefs                            (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
window_prefs                           (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
window_alias                           (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
window_macro                           (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
triggers_window                        (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
do_manual                              (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
send_to_connection                     (GtkEntry        *entry,
                                        gpointer         user_data);

gboolean
hist_evt                               (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data);

void
toggle_triggers                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
toggle_parsing                         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
toggle_review                          (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
reconnect                              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
load_tt_prefs                          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
load_zmud_prefs                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
save_win_pos                           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
save_all_prefs                         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
save_review                            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
macro_window                           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
highlights_window                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
gags_window                            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
create_complete_window                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
variables_window                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
toggle_logger                          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
log_viewer                             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
triggerclass_window                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
mccp_status                            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
do_about                               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
do_log                                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
close_window                           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
trigger_selection_made                 (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
find_in_list                           (GtkButton       *button,
                                        gpointer         user_data);

void
trigger_button_add                     (GtkButton       *button,
                                        gpointer         user_data);

void
trigger_button_delete                  (GtkButton       *button,
                                        gpointer         user_data);

void
save_triggers                          (GtkButton       *button,
                                        gpointer         user_data);

void
trigger_class_toggle                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
save_triggerclass_state                (GtkButton       *button,
                                        gpointer         user_data);

void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
close_window                           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
close_window                           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
alias_selection_made                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
find_in_list                           (GtkButton       *button,
                                        gpointer         user_data);

void
alias_button_delete                    (GtkButton       *button,
                                        gpointer         user_data);

void
save_aliases                           (GtkButton       *button,
                                        gpointer         user_data);

void
delete_a_gui_window                    (GtkButton       *button,
                                        gpointer         user_data);

void
alias_button_add                       (GtkButton       *button,
                                        gpointer         user_data);

void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
close_window                           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
gag_selection_made                     (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
gag_button_add                         (GtkButton       *button,
                                        gpointer         user_data);

void
gag_button_delete                      (GtkButton       *button,
                                        gpointer         user_data);

void
save_gags                              (GtkButton       *button,
                                        gpointer         user_data);

void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
close_window                           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
complete_selection_made                (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
complete_button_add                    (GtkButton       *button,
                                        gpointer         user_data);

void
complete_button_delete                 (GtkButton       *button,
                                        gpointer         user_data);

void
save_complete                          (GtkButton       *button,
                                        gpointer         user_data);


gboolean
close_window                           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
close_a_gui_window                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_documentation_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_homepage_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_buttons_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_clist_button_select_row             (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_buttadd_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_buttdel_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
save_buttons                           (GtkButton       *button,
                                        gpointer         user_data);

void
on_clist_button_select_row             (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
close_window                           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_font_selection_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_fonts_ok_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_fonts_save_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_checkbutton_button_toggled          (GtkWidget *, 
                                        GtkToggleButton *togglebutton);

void
on_button_msp_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_msp_ko_clicked                      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_msp_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data);



void
sub_selection_made                     (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
sub_button_add                         (GtkButton       *button,
                                        gpointer         user_data);

void
sub_button_delete                      (GtkButton       *button,
                                        gpointer         user_data);

void
save_subs                              (GtkButton       *button,
                                        gpointer         user_data);

void
sub_window                             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
window_subs                            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_clear_review_buffer_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
