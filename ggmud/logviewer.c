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

static   char        *sFilename = NULL; 
static GtkWidget           *win_main;
static GtkAccelGroup       *accel_group;
static GtkTooltips         *tooltips = NULL;

static void menu_New ();
static void menu_Find ();
static void menu_Open ();
static void menu_Save ();
static void menu_SaveAs ();
static void menu_Quit ();
static void ShowMessage (char *szTitle, char *szMessage);

static void 
LoadFile (char *sFilename)
{
    char buffer[BUF_SIZE];
    int nchars;
    FILE *infile;
    struct stat fileStatus;
    long fileLen = 0;

    gtk_text_freeze (GTK_TEXT (text));

    gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);

    stat (sFilename, &fileStatus);
    fileLen = fileStatus.st_size;

    infile = fopen (sFilename, "r");
      
    if (infile) {
      
        while ((nchars = fread (buffer, 1, BUF_SIZE, infile)) > 0) {

            gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL, buffer, nchars);
          
            if (nchars < BUF_SIZE)
                break;
        }
      
        fclose (infile);
    }
  
    gtk_text_thaw (GTK_TEXT (text));
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

    if (accel_group == NULL) {
        accel_group = gtk_accel_group_new ();
        gtk_accel_group_attach (accel_group, GTK_OBJECT (win_main));
    }

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
    GtkWidget *table;
    GtkWidget *hscrollbar;
    GtkWidget *vscrollbar;

    table = gtk_table_new (2, 2, FALSE);

    gtk_container_add (GTK_CONTAINER (container), table);

    gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
    gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);

    gtk_widget_show (table);

    text = gtk_text_new (NULL, NULL);
    gtk_text_set_editable (GTK_TEXT (text), TRUE);

    gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
            GTK_EXPAND | GTK_SHRINK | GTK_FILL,
            GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);

    gtk_widget_show (text);

    hscrollbar = gtk_hscrollbar_new (GTK_TEXT (text)->hadj);
    gtk_table_attach (GTK_TABLE (table), hscrollbar, 0, 1, 1, 2,
            GTK_EXPAND | GTK_FILL | GTK_SHRINK, GTK_FILL, 0, 0);
    gtk_widget_show (hscrollbar);

    vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
    gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
            GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
    gtk_widget_show (vscrollbar);

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


GtkWidget *CreateMenuRadio (GtkWidget *menu, 
                            char *szName, 
                            GSList **group,
                            GtkSignalFunc func, 
                            gpointer data)
{
    GtkWidget *menuitem;

    menuitem = gtk_radio_menu_item_new_with_label (*group, szName);
    *group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (menuitem));

    gtk_menu_append (GTK_MENU (menu), menuitem);
    gtk_widget_show (menuitem);

    gtk_signal_connect (GTK_OBJECT (menuitem), "toggled",
                        GTK_SIGNAL_FUNC(func), data);

    return (menuitem);
}


static void CreateMenu (GtkWidget *window, GtkWidget *vbox_main)
{
    GtkWidget *menubar;
    GtkWidget *menu;
    GtkWidget *menuitem;

    win_main = window;

    accel_group = gtk_accel_group_new ();
    gtk_accel_group_attach (accel_group, GTK_OBJECT (window));

    menubar = gtk_menu_bar_new ();
    gtk_box_pack_start (GTK_BOX (vbox_main), menubar, FALSE, TRUE, 0);
    gtk_widget_show (menubar);

    menu = CreateBarSubMenu (menubar, "File");

    menuitem = CreateMenuItem (menu, "Open", "^O", 
                     "Open an existing item", 
                     GTK_SIGNAL_FUNC (menu_Open), "open");

    menuitem = CreateMenuItem (menu, NULL, NULL, 
                     NULL, NULL, NULL);

    menuitem = CreateMenuItem (menu, "Quit", "", 
                     "What's more descriptive than quit?", 
                     GTK_SIGNAL_FUNC (menu_Quit), "quit");

    menu = CreateBarSubMenu (menubar, "Search");

    menuitem = CreateMenuItem (menu, "Find", "^F", 
                     "Find item", 
                     GTK_SIGNAL_FUNC (menu_Find), "paste");

}

static char *
GetExistingFile ()
{
    return (sFilename);
}
    
static void 
FileOk (GtkWidget *w, gpointer data)
{
    char *sTempFile;
    typFileSelectionData *localdata;
    GtkWidget *filew;
 
    localdata = (typFileSelectionData *) data;
    filew = localdata->filesel;

    sTempFile = gtk_file_selection_get_filename (GTK_FILE_SELECTION (filew));

    if (sFilename) free (sFilename);
    sFilename = strdup (sTempFile); 
    (*(localdata->func)) (sFilename);
    gtk_widget_destroy (filew);
}

static void destroy (GtkWidget *widget, gpointer *data)
{
    gtk_grab_remove (widget);

    free (data);
}

void GetFilename (char *sTitle, void (*callback) (char *))
{
    GtkWidget *filew = NULL;
    typFileSelectionData *data;

    filew = gtk_file_selection_new (sTitle);

    data = malloc (sizeof (typFileSelectionData));
    data->func = callback;
    data->filesel = filew;

    gtk_signal_connect (GTK_OBJECT (filew), "destroy",
            (GtkSignalFunc) destroy, data);

    gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
            "clicked", (GtkSignalFunc) FileOk, data);
    
    gtk_signal_connect_object (
             GTK_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
             "clicked", (GtkSignalFunc) gtk_widget_destroy, 
             (gpointer) filew);
    
    if (sFilename) {

        gtk_file_selection_set_filename (GTK_FILE_SELECTION (filew), 
                                         sFilename);
    }
   
    gtk_widget_show (filew);
    gtk_grab_add (filew);
}

gint ClosingAppWindow (GtkWidget *widget, gpointer data);
char *GetExistingFile ();


    GtkWidget *window;

void log_viewer()
{
    GtkWidget *main_vbox;
    
    window = gtk_window_new(GTK_WINDOW_DIALOG);

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
    
    CreateMenu (window, main_vbox);
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

void menu_Quit (GtkWidget *widget, gpointer data)
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
    char *buffer;

    buffer = gtk_editable_get_chars (
                 GTK_EDITABLE (text), 
                 (gint) 0,
                 (gint) gtk_text_get_length (GTK_TEXT (text)));
 
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
    button = gtk_button_new_with_label ("Find Next");
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
	                GTK_SIGNAL_FUNC (YesFunc),
	                dialog_window);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
			  button, TRUE, TRUE, 0);
    gtk_widget_show (button);

    button = gtk_button_new_with_label ("Cancel");
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

void ShowMessage (char *szTitle, char *szMessage)
{
    GtkWidget *label;
    GtkWidget *button;
    GtkWidget *dialog_window;

    dialog_window = gtk_dialog_new ();

    gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
              GTK_SIGNAL_FUNC (ClearShowMessage),
              NULL);

    gtk_window_set_title (GTK_WINDOW (dialog_window), szTitle);
    gtk_container_border_width (GTK_CONTAINER (dialog_window), 0);

    button = gtk_button_new_with_label ("OK");

    gtk_signal_connect (GTK_OBJECT (button), "clicked",
              GTK_SIGNAL_FUNC (CloseShowMessage),
              dialog_window);

    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
              button, TRUE, TRUE, 0);
    gtk_widget_grab_default (button);
    gtk_widget_show (button);

    label = gtk_label_new (szMessage);

    gtk_misc_set_padding (GTK_MISC (label), 10, 10);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->vbox), 
              label, TRUE, TRUE, 0);
    gtk_widget_show (label);

    gtk_widget_show (dialog_window);
    gtk_grab_add (dialog_window);
}

