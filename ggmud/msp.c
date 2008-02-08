#include <ctype.h>
#include <stdio.h>
#ifndef WIN32
#include <sys/dir.h>
#else
#include <dirent.h>
#endif
#include "ggmud.h"

char default_sound_url[256] = "";
char default_music_url[256] = "";

#define RANDOM_RAGNAR(n) ((int) ((float)(n)*rand()/(RAND_MAX+1.0)))

msp_control *init_msp()
{
    return calloc(sizeof(msp_control), 1);
}

void msp_find_file (const char *file, char *dest, int sound)
{
  DIR *d;
  struct dirent *de;
  char files[256][32];
  int max = 0;
  char buf[256];
  char *p = &buf[0];
  
  strcpy (dest, prefs.SoundPath);
  strcat (dest, sound ? "/samples/" : "/modules/");
  strcpy (buf, file);

  while (*p && *p != '*') {
    ++p;
  }

  if (!p) {
    p = &buf[0];
    while (*p && *p != '.') {
      ++p;
    }
    strcat (dest, file);
    if (!p) {
      strcat (dest, sound ? ".wav" : ".mid");
    }
    return;
  }
  
  *p = 0;  
  
  if (!(d = opendir (dest)))
      return;

  // cheap way.. forget about glob or scandir, mallocs or frees
  while (1) {
    de = readdir (d);
    if (!de || max == 256) {
      break;
    }
    if (strstr (de->d_name, file) == &de->d_name[0]) {
      strncpy (files[max], de->d_name, 32);
      files[max++][31] = 0;
    }
  }
  
  strcat (dest, max ? files[RANDOM_RAGNAR(max)] : file );
  closedir (d);
}

void msp_request_sample (char *file, int vol, int repeats, int priority, int cont, char *url)
{
    if (!strcmp (file, "Off")) {
        if (url)
            strncpy(mud->msp->default_sound_url, url, sizeof(default_sound_url) - 1);

        *mud->msp->actual_sound = 0;
        mud->msp->sound_priority = -1;
        fprintf(stderr, "Stopped PLAYING SAMPLEs!\n");
    }
    else if (priority >= mud->msp->sound_priority) {
        char buf[512];

        msp_find_file(file, buf, 1);
        mud->msp->sounds_played++;
        fprintf(stderr, "Playing SOUND %s -> %s...\n", file, buf);
    }
}

void msp_request_music (char *file, int vol, int repeats, int priority, int cont, char *url)
{
    if (!strcmp (file, "Off")) {
        if (url)
            strncpy(mud->msp->default_music_url, url, sizeof(default_music_url) - 1);

        *mud->msp->actual_music = 0;
        mud->msp->music_priority = -1;

        fprintf(stderr, "Stopped PLAYING MUSIC!\n");
    }
    else if (priority >= mud->msp->music_priority) {
        char buf[512];
        strncpy(mud->msp->actual_music, file, strlen(mud->msp->actual_music) - 1);

        msp_find_file(file, buf, 0);
        mud->msp->musics_played++;
        fprintf(stderr, "Playing MUSIC %s -> %s...\n", file, buf);
    }
}

int check_msp (char *arg)
{
  char sound;
  char file[0xff];
  char buf[0xff];
  int vol = 100;
  int repeats = 1;
  int priority = 50;
  int cont = 1;
  int *what;
  char *url = NULL;
  char *h = arg;
  char *p;
      
  if (!prefs.UseMSP) 
    return FALSE;
  
  if (strstr(arg, "!!SOUND(")) 
    sound = 1; 
  else if (strstr(arg, "!!MUSIC(")) 
    sound = 0;
  else 
    return FALSE;

  // sound triggers MUST begin with ! or ESC (this to support medievia)
  if (arg[0] != '!' && arg[0] != 0x1b)
      return FALSE;

  if (!mud->msp) {
      textfield_add(mud->text, "\n-> music/sound trigger forced MSP activation.\n", MESSAGE_TICK);
      mud->msp = init_msp();
  }

  fprintf(stderr, "Parsing MSP line: %s (%02x%02x%02x%02x)\n", arg,
          arg[0], arg[1], arg[2], arg[3]);

  while (*h && *h != ')') ++h;
  
  if (!h) 
    return FALSE;

  h = arg;
    
  while (isspace(*h) || *h != '(')  ++h;

  ++h;
  
  p = &file[0];
  while (*h != ')' && !isspace(*h)) 
    *(p++) = *(h++);
  
  *p = 0;

  while (1) {
    p = &buf[0];
    while (*h && (isspace(*h) || *h == '=')) {
      ++h;
    }
    if (!*h || *h == ')') {
      break;
    }
    switch (*h) {
      case 'U':
        what = (int *)&url; break;
      case 'V':
        what = &vol; break;
      case 'L':
        what = &repeats; break;
      case 'P':
        what = &priority; break;
      case 'C':
        what = &cont; break;
      case 'T':
      default:
//        what = NULL; return FALSE;
        while (*h && !isspace(*(h++))) ;
        continue;
    }
   
    if (what != (int *)&url) {
        while (!isdigit(*h) && *h != '-') {
          ++h;
        }
      
        while (!isspace(*h) && *h != ')') {
          *(p++) = *(h++);
        }
        *p = 0;  
        *what = atoi (buf);
    }
    else {
         while (*h != '=' && *h) ++h;
         
         if (!*h)
             return FALSE;
         else
             ++h;
         
         while (!isspace(*h) && *h != ')') {
          *(p++) = *(h++);
         }
         *p = 0;
         *((char **)what) = strdup(buf);
    }
  }
  
  if (sound)
    msp_request_sample (file, vol, repeats, priority, cont, url);
  else 
    msp_request_music (file, vol, repeats, priority, cont, url);
 
  if (url) 
      free(url);

  // we choose to ignore the rest of the line
  arg[0] = '.'; arg[1] = 0;
  return TRUE;
}

