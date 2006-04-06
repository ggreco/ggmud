/*  Sclient
 *  Copyright (C) 1999 Heathen (the.heathen@swipnet.se)
 *		  1999 Drizzt  (doc.day@swipnet.se)
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
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include <stdlib.h>
#include "ggmud.h"

void hist_add (const char *x)
{
    extern int hide_input;

    if (mud->hist->max < 1 || hide_input)
        return;


// skip if the previous is equal
    if(mud->hist->pos > 0) {
        if(!strcmp(mud->hist->list[mud->hist->pos - 1], x))
            return;
    }
        
    /* free up 'lost' bit */
    if(mud->hist->list[mud->hist->pos])
        free(mud->hist->list[mud->hist->pos]);

    /* add our line to the history */
    mud->hist->list[mud->hist->pos] = strdup (x);

    /* update the rest of history */
    mud->hist->cur = mud->hist->pos;
    if (mud->hist->size < mud->hist->max)
        mud->hist->size++;

    mud->hist->pos=(mud->hist->pos+1)%mud->hist->size;

//    free(mud->hist->tmp); ????
    mud->hist->cur=mud->hist->pos;
}

static void hist_prev ()
{
    if (mud->hist->cyclic == 1)
        if((mud->hist->pos+1)%mud->hist->size==mud->hist->cur)  return;

    if (mud->hist->list[mud->hist->cur])
        free(mud->hist->list[mud->hist->cur]);
    
    mud->hist->list[mud->hist->cur] = strdup (gtk_entry_get_text(mud->ent));

    if (mud->hist->size>0) {
        mud->hist->cur--;
        if (mud->hist->cur < 0)
            mud->hist->cur=mud->hist->size-1;
    }
    gtk_entry_set_text(mud->ent, mud->hist->list[mud->hist->cur]);
}

static void hist_next ()
{
    if (mud->hist->cyclic == 1)
        if(mud->hist->cur == mud->hist->pos) { // it seems enough to have a line clear similar to zmud one
            gtk_entry_set_text(mud->ent,"");
            return;
        }
    
    if (mud->hist->list[mud->hist->cur])
        free(mud->hist->list[mud->hist->cur]);

    mud->hist->list[mud->hist->cur] = strdup (gtk_entry_get_text(mud->ent));

    if (mud->hist->size)
        mud->hist->cur = (mud->hist->cur+1)%mud->hist->size;

    gtk_entry_set_text(mud->ent,mud->hist->list[mud->hist->cur]);
}

#ifdef UNDEF
static void hist_clear ()
{
    int x;

    for (x = 0; x < mud->hist->max; x++)
        if(mud->hist->list[x]) {
            free (mud->hist->list[x]);
            mud->hist->list[x] = NULL;
        }
    mud->hist->cur = mud->hist->pos = 0;
}
#endif

gint change_focus(GtkWidget *w, GdkEventKey *event, gpointer data)
{
/*  char buffer[100];
  
  sprintf(buffer, "Ricevuto state: %ld keyval: %ld\n", event->state, event->keyval);
      textfield_add(mud->text, buffer, MESSAGE_NORMAL);
 */

  if ( (event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK ||
         event->keyval == GDK_Control_L ||
         event->keyval == GDK_Control_R ) {

      if(event->keyval == GDK_c ||
         event->keyval == GDK_C ) {
//        textfield_add(mud->text, "Grabbato ctrl+c\n", MESSAGE_NORMAL);
//        gtk_editable_copy_clipboard(GTK_EDITABLE(w)); // it was mud->text
      
        gtk_text_buffer_copy_clipboard(gtk_text_view_get_buffer(GTK_TEXT_VIEW(w)),
                gtk_clipboard_get(gdk_atom_intern("CLIPBOARD", TRUE)));
      }
      return 0;
  } else {
      GtkWidget *win = gtk_widget_get_toplevel((GtkWidget *)mud->ent);
      if (event->keyval == GDK_Tab) {
          extern void toggle_review();

          toggle_review();
      }
     
      if (win != gtk_widget_get_toplevel(w)) 
          gtk_window_present(GTK_WINDOW(win));
      
      gtk_widget_grab_default (GTK_WIDGET(mud->ent));
      gtk_widget_grab_focus (GTK_WIDGET(mud->ent));

      gtk_signal_emit_by_name(GTK_OBJECT(mud->ent), "key_press_event", event, data);

      gtk_signal_emit_stop_by_name(GTK_OBJECT(w), "key_press_event");
      return 1;
  }

}


gint hist_evt (GtkWidget* w, GdkEventKey* event, gpointer data)
{
    extern char *tab_complete(char *, int);
    static int tabbing = 0;
    char keypress = 0;
    
    switch (event->keyval) {
        case GDK_Tab:
            {
                char word[32], *result, *buf;
                int i, pos = gtk_editable_get_position(GTK_EDITABLE(w));
               
                if(tabbing && pos > tabbing) {
                    gtk_editable_delete_text(GTK_EDITABLE(w),
                            pos - tabbing - 1, pos);

                    pos = gtk_editable_get_position(GTK_EDITABLE(w));
                }

                
                buf = gtk_editable_get_chars(GTK_EDITABLE(w), 0, pos);

                i = pos - 1;
                
                while(i && buf[i] > ' ') i--;
                
                if(i == 0)
                    strcpy(word, buf);
                else {
                    i++;
                    strcpy(word, buf + i);
                }
                
                if(!*word) {
                    tabbing = 0;
                    return 0;
                }
                
                if ((result = tab_complete(word, tabbing))) {
                    int temp = pos;
 
                    result += strlen(word);
                    
                    gtk_editable_insert_text(GTK_EDITABLE(w),
                            result, strlen(result), &temp);
                    tabbing = strlen(result);

                    temp = pos + tabbing;

                    gtk_editable_insert_text(GTK_EDITABLE(w), 
                            " ", 1, &temp);

                    gtk_editable_set_position(GTK_EDITABLE(w), 
                            pos + tabbing + 1);
                }
                else
                    tabbing = 0;
            }
            break;
        case GDK_KP_2:
            keypress = 's';
            break;
        case GDK_KP_8:
            keypress = 'n';
            break;
        case GDK_KP_6:
            keypress = 'e';
            break;
        case GDK_KP_4:
            keypress = 'w';
            break;
        case GDK_KP_9:
            keypress = 'u';
            break;
        case GDK_KP_3:
            keypress = 'd';
            break;
        case GDK_KP_5:
            keypress = 'l';
            break;
            
        case GDK_Up: {
            hist_prev();
            break;
        }
        case GDK_Down: {
            hist_next();
            break;
        }
        case GDK_Page_Up: {
            GtkAdjustment *adj = mud->text->vadjustment;
            gtk_adjustment_set_value(adj,adj->value-adj->page_size);
            break;
        }
        case GDK_Page_Down: {
            GtkAdjustment *adj = mud->text->vadjustment;
            if (adj->value < adj->upper - adj->page_size)
                gtk_adjustment_set_value(adj,adj->value+adj->page_size);
            break;
        }
        default:
            /* normal handling */
            tabbing = 0;
            return 0;
            break;
    }

    if(keypress) {
        extern struct session *parse_input(char *, struct session *); // fake prototype
        
        char temp[20];

        temp[0] = keypress;
        temp[1] = 0;
        parse_input(temp, mud->activesession);
    }
    
    gtk_signal_emit_stop_by_name(GTK_OBJECT(w), "key_press_event");
    return 1;
}
