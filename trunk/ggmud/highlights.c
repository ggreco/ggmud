#include "ggmud.h"

GtkWidget *menu_Tools_Highlight;

#define HIGH_FILE "highlight"

void
highlights_window(GtkWidget *w, gpointer data)
{
    popup_window("HIGHLIGHTS settings: TODO");
}

static void
save_highlights (GtkWidget *button, gpointer data) {
    FILE *fp;
    gint done = FALSE;
    gchar *alias, *replace;
    gint  row = 0;

    if (fp = fileopen (HIGH_FILE, "w")) {
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

static void
add_highlight(int type, char *string)
{
    // TODO
}

void
load_highlights() 
{
    FILE *fp;

    if ((fp = fileopen(HIGH_FILE, "r"))) {
        parse_config(fp, NULL);   
        fclose(fp);
    }
}

