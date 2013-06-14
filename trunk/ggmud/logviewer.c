/*  GGMud 
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

/* a small Text editor to look at the log files! */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "ggmud.h"

static GtkWidget *text;

#define BUF_SIZE 256


typedef struct {

    void (*func)();
    GtkWidget *filesel;

} typFileSelectionData;

static GtkAccelGroup       *accel_group;
static GtkTooltips         *tooltips = NULL;

static void menu_Find ();
static void menu_Open ();
static void menu_Quit ();

static void 
LoadFile (char *sFilename)
{
    char buffer[BUF_SIZE];
    int nchars;
    FILE *infile;

    GtkTextBuffer *b = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    GtkTextIter begin, end;

    gtk_text_buffer_get_bounds(b, &begin, &end);
    gtk_text_buffer_delete(b, &begin, &end);
    
    infile = fopen (sFilename, "r");
      
    if (infile) {
        GtkTextBuffer *b = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
        GtkTextIter it;

        while ((nchars = fread (buffer, 1, BUF_SIZE, infile)) > 0) {
            
            gtk_text_buffer_get_end_iter(b, &it);
        
            gtk_text_buffer_insert(b, &it, buffer, nchars);

            if (nchars < BUF_SIZE)
                break;
        }
      
        fclose (infile);
    }
  
}


GtkWidget *CreateStockMenuItem(GtkWidget *menu, 
                           char *stock, 
                           GtkSignalFunc func,
                           gpointer data)
{
    GtkWidget *menuitem;

    if (accel_group == NULL) 
        accel_group = gtk_accel_group_new ();
    
    menuitem = gtk_image_menu_item_new_from_stock (stock, accel_group);
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
                    GTK_SIGNAL_FUNC(func), data);

    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    return (menuitem);
}

GtkWidget *CreateMenuItem (GtkWidget *menu, 
                           char *szName, 
                           char *szAccel,
                           char *szTip, 
                           GtkSignalFunc func,
                           gpointer data)
{
    GtkWidget *menuitem;

    if (szName && strlen (szName)) {
        menuitem = gtk_menu_item_new_with_label (szName);
        gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
                    GTK_SIGNAL_FUNC(func), data);
    } else {
        menuitem = gtk_menu_item_new ();
    }

    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    if (accel_group == NULL) 
        accel_group = gtk_accel_group_new ();

    if (szAccel && szAccel[0] == '^') {
        gtk_widget_add_accelerator (menuitem, 
                                    "activate", 
                                    accel_group,
                                    szAccel[1], 
                                    GDK_CONTROL_MASK,
                                    GTK_ACCEL_VISIBLE);
    }

    if (szTip && strlen (szTip)) {

        if (tooltips == NULL) {

            tooltips = gtk_tooltips_new ();
        }
        gtk_tooltips_set_tip (tooltips, menuitem, szTip, NULL);
    }

    return (menuitem);
}

GtkWidget *CreateMenuCheck (GtkWidget *menu, 
                            char *szName, 
                            GtkSignalFunc func, 
                            gpointer data)
{
    GtkWidget *menuitem;

    menuitem = gtk_check_menu_item_new_with_label (szName);

    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    gtk_signal_connect (GTK_OBJECT (menuitem), "toggled",
                        GTK_SIGNAL_FUNC(func), data);

    return (menuitem);
}

void CreateText (GtkWidget *window, GtkWidget *container)
{
    GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show(sw);

    gtk_container_add (GTK_CONTAINER (container), sw);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_ALWAYS);


    text = gtk_text_view_new ();
    gtk_container_add(GTK_CONTAINER(sw), (GtkWidget *)text);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (text), TRUE);

    gtk_widget_show (text);
}

GtkWidget *CreateSubMenu (GtkWidget *menubar, char *szName)
{
    GtkWidget *menuitem;
    GtkWidget *menu;
 
    menuitem = gtk_menu_item_new_with_label (szName);

    gtk_widget_show (menuitem);
    gtk_menu_append (GTK_MENU (menubar), menuitem);

    menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);

    return (menu);
}

GtkWidget *CreateBarSubMenu (GtkWidget *menu, char *szName)
{
    GtkWidget *menuitem;
    GtkWidget *submenu;
 
    menuitem = gtk_menu_item_new_with_label (szName);

    gtk_menu_bar_append (GTK_MENU_BAR (menu), menuitem);
    gtk_widget_show (menuitem);

    submenu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);

    return (submenu);
}



static void MyCreateMenu (GtkWidget *window, GtkWidget *vbox_main)
{
    GtkWidget *menubar;
    GtkWidget *menu;

    accel_group = gtk_accel_group_new ();
//    gtk_accel_group_attach (accel_group, GTK_OBJECT (window));

    menubar = gtk_menu_bar_new ();
    gtk_box_pack_start (GTK_BOX (vbox_main), menubar, FALSE, TRUE, 0);
    gtk_widget_show (menubar);

    menu = CreateBarSubMenu (menubar, "File");

    CreateStockMenuItem (menu, GTK_STOCK_OPEN, 
                     GTK_SIGNAL_FUNC (menu_Open), "open");

    CreateMenuItem (menu, NULL, NULL, 
                     NULL, NULL, NULL);

    CreateStockMenuItem (menu, GTK_STOCK_QUIT, 
                     GTK_SIGNAL_FUNC (menu_Quit), window);

    menu = CreateBarSubMenu (menubar, "Search");

    CreateStockMenuItem (menu, GTK_STOCK_FIND, 
                     GTK_SIGNAL_FUNC (menu_Find), "find");

}

void GetFilename (char *sTitle, void (*do_callback) (char *))
{
    static gchar *sFilename = NULL;

    GtkWidget *filew = gtk_file_chooser_dialog_new (sTitle, GTK_WINDOW(mud->window), 
                                  GTK_FILE_CHOOSER_ACTION_OPEN,
                                  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                  NULL);

    if (sFilename) 
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (filew), 
                                         sFilename);
    
    gtk_widget_show (filew);

    if (gtk_dialog_run(GTK_DIALOG(filew)) == GTK_RESPONSE_ACCEPT) {
        if (sFilename) g_free (sFilename);

        sFilename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filew));
        
        do_callback(sFilename);
    }

    gtk_widget_destroy(filew);
}

gint ClosingAppWindow (GtkWidget *widget, gpointer data);

void log_viewer()
{
    GtkWidget *main_vbox;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title (GTK_WINDOW (window), "GGMud Log Viewer");
    gtk_container_border_width (GTK_CONTAINER (window), 0);

    gtk_signal_connect (GTK_OBJECT(window), "delete_event", 
               GTK_SIGNAL_FUNC (ClosingAppWindow), 
               NULL);

    gtk_widget_set_usize (GTK_WIDGET(window), 400, 300);
    
    main_vbox = gtk_vbox_new (FALSE, 1);

    gtk_container_border_width (GTK_CONTAINER(main_vbox), 1);

    gtk_container_add (GTK_CONTAINER(window), main_vbox);

    gtk_widget_show (main_vbox);

    gtk_widget_realize (window);
    
    MyCreateMenu (window, main_vbox);
    CreateText (window, main_vbox);
/* need to rewrite the whole window for this to work!!
    gdk_window_set_background(GTK_TEXT(text)->text_area,
			      &(prefs.BackgroundColor));
*/
    gtk_widget_show (window);
}

gint ClosingAppWindow (GtkWidget *widget, gpointer data)
{
    return (FALSE);
}

void menu_Quit (GtkWidget *widget, GtkWidget *window)
{
   gtk_widget_destroy (window);
}


void menu_Open (GtkWidget *widget, gpointer data)
{
    GetFilename ("Open", LoadFile);
}


void ClearFile (GtkWidget *widget, gpointer data)
{
    gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
}

void TextCopy (GtkWidget *widget, gpointer data) 
{
    gtk_editable_copy_clipboard (GTK_EDITABLE (text));
}

void TextPaste (GtkWidget *widget, gpointer data)
{
    gtk_editable_paste_clipboard (GTK_EDITABLE (text));
}

char *GetText ()
{
    // TODO
    char *buffer = NULL;
#if 0

    buffer = gtk_editable_get_chars (
                 GTK_EDITABLE (text), 
                 (gint) 0,
                 (gint) gtk_text_get_length (GTK_TEXT (text)));
#endif
    return (buffer);
}

GtkWidget *GetTextWidget ()
{
    return (text);
}

int LookForString (char *szHaystack, char *szNeedle, int nStart);

static GtkWidget *dialog_window = NULL;
static GtkWidget *entry;
static char *szNeedle;


void CloseFindDialog (GtkWidget *widget, gpointer data)
{

    gtk_widget_destroy (widget);

    dialog_window = NULL;
}

void FindFunction (GtkWidget *widget, gpointer data)
{
    // TODO
#if 0
    int nIndex;
    GtkWidget *text = GetTextWidget ();
    char *szHaystack;   

    szHaystack = GetText ();

    if (szNeedle) {
        free (szNeedle);
    }
    szNeedle = gtk_editable_get_chars (
                 	GTK_EDITABLE (entry), 0, -1);

    nIndex = GTK_EDITABLE (text)->selection_end_pos;
    nIndex = LookForString (szHaystack, szNeedle, nIndex);

    if (nIndex >= 0) {
        gtk_text_set_point (GTK_TEXT (text), nIndex);
        gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL, " ", 1);
        gtk_text_backward_delete (GTK_TEXT (text), 1);
        gtk_editable_select_region (GTK_EDITABLE (text), 
                                nIndex, nIndex + strlen (szNeedle));

        dialog_window = NULL;
    } else {

        ShowMessage ("Find...", "Not found.  Reached the end of the file.");
    }

    free (szHaystack);
#endif
}

int LookForString (char *szHaystack, char *szNeedle, int nStart)
{
    int nHaystackLength;
    int nNeedleLength;
    int nPos;

    nHaystackLength = strlen (szHaystack);
    nNeedleLength = strlen (szNeedle);

    for (nPos = nStart; nPos < nHaystackLength; nPos++) {
        if (strncmp (&szHaystack[nPos], szNeedle, nNeedleLength) == 0) {
             return (nPos);
        }
    }

    return (-1);
}

void CancelFunction (GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy (GTK_WIDGET (data));

    dialog_window = NULL;
}

void FindStringDialog (char *szMessage, void (*YesFunc)(), void (*NoFunc)())
{
    GtkWidget *label;
    GtkWidget *button;
    GtkWidget *hbox;

    if (dialog_window) return;

    dialog_window = gtk_dialog_new ();
    gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
	                GTK_SIGNAL_FUNC (CloseFindDialog),
	                dialog_window);

    gtk_window_set_title (GTK_WINDOW (dialog_window), "Find");
    gtk_container_border_width (GTK_CONTAINER (dialog_window), 5);

    hbox = gtk_hbox_new (TRUE, TRUE);

    label = gtk_label_new ("Find What:");
    gtk_widget_show (label);
 

    entry = gtk_entry_new ();
    gtk_widget_show (entry);

    if (szNeedle) {

        gtk_entry_set_text (GTK_ENTRY (entry), szNeedle);
    }

    gtk_box_pack_start (GTK_BOX (hbox), 
			  label, TRUE, TRUE, 0);

    gtk_box_pack_start (GTK_BOX (hbox), 
			  entry, TRUE, TRUE, 0);
    gtk_widget_show (hbox);

    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->vbox), 
                        hbox, TRUE, TRUE, 0);
    button = gtk_button_new_from_stock (GTK_STOCK_FIND);
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
	                GTK_SIGNAL_FUNC (YesFunc),
	                dialog_window);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
			  button, TRUE, TRUE, 0);
    gtk_widget_show (button);

    button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
	                GTK_SIGNAL_FUNC (NoFunc),
	                dialog_window);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
			  button, TRUE, TRUE, 0);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);

    gtk_widget_show (dialog_window);
}

void menu_Find (GtkWidget *widget, gpointer data)
{
    FindStringDialog ("Find", FindFunction, CancelFunction);
}

void CloseShowMessage (GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog_widget = (GtkWidget *) data;

    gtk_widget_destroy (dialog_widget);
}


void ClearShowMessage (GtkWidget *widget, gpointer data)
{
    gtk_grab_remove (widget);
}

