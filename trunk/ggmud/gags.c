#include "ggmud.h"

GtkWidget *menu_Tools_Gag;	

#define GAG_FILE "gag"

void
gags_window(GtkWidget *w, gpointer data)
{
    popup_window("GAGS settings: TODO");
}

static void
save_gags (GtkWidget *button, gpointer data) {
    FILE *fp;
    gint done = FALSE;
    gchar *alias, *replace;
    gint  row = 0;

    if (fp = fileopen (GAG_FILE, "w")) {
    	while ( !done && (GtkCList*) data) {
            if ( !gtk_clist_get_text ((GtkCList*) data, row, 0, &alias)
                || !gtk_clist_get_text ((GtkCList*) data, row, 1, &replace) )
                break;
            
            if (!alias[0]) {
                done = TRUE;
                break;
            }
            fprintf (fp, "%s %s\n", alias, replace);
            row++;
        }
        fclose (fp);
    }    
}
