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

#ifndef _LOGVIEWER_H_
#define _LOGVIEWER_H_
#include "ggmud.h"

void ShowMessage (char *szTitle, char *szMessage);
GtkWidget *GetTextWidget ();
char *GetText ();
void menu_New ();
void menu_Find ();
void menu_Open ();
void menu_Save ();
void menu_SaveAs ();
void menu_Quit ();
void TextCut ();
void TextCopy ();
void TextPaste ();
void ClearFile ();
void LoadFile (char *sFilename);
void ImportFile (char *sFilename);
void SaveFile (char *sFilename);
//
// --- Function prototypes
//
void log_viewer();
void SelectMenu (GtkWidget *widget, gpointer data);
void DeSelectMenu (GtkWidget *widget, gpointer data);
void SetMenuButton (char *szButton, int nState) ;
GtkWidget *CreateMenuItem (GtkWidget *menu, 
                           char *szName, 
                           char *szAccel,
                           char *szTip, 
                           GtkSignalFunc func,
                           gpointer data);
GtkWidget *CreateMenuCheck (GtkWidget *menu, 
                            char *szName, 
                            GtkSignalFunc func, 
                            gpointer data);
GtkWidget *CreateSubMenu (GtkWidget *menubar, char *szName);
GtkWidget *CreateBarSubMenu (GtkWidget *menu, char *szName);
void CreateMenu (GtkWidget *window, GtkWidget *vbox_main);
void CreateText (GtkWidget *window, GtkWidget *container);
void GetFilename (char *sTitle, void (*func) (gchar *));
//
void UpdateProgress (long pos, long len);
void StartProgress ();
void EndProgress ();

#endif /* _LOGVIEWER_H_ */
