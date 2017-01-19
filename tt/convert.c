/*
TinTin++
Copyright (C) 2001 Davin Chan, Robert Ellsworth, etc. (See CREDITS file)

This program is protected under the GNU GPL (See COPYING)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
/* Autoconf patching by David Hedbor, neotron@lysator.liu.se */
#include "tintin.h"

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

char *get_arg_stop_spaces();
char *get_arg_in_quotes();
void fix_vars();
void convert();
int is_abrev();
char tintin_char, dc, dop,sc[5],so[5];
char highlight_type[80];
main ()
{
	FILE *fin, *fout;
	FILE *fopen();
        char ifilename[80], ofilename[80], strng[1024], arg1[1024];
	char arg2[1024],comm[80], *sptr;
        dop=DEFAULT_OPEN;
	dc=DEFAULT_CLOSE;
        sprintf(so,"%c",dop);
	sprintf(sc,"%c",dc);
	
	printf("default highlight type: (bold, reverse, blink, italic, faint, or 1 through 8)\n");
        read (0,strng,80);
        sscanf(strng,"%s\n",highlight_type);
        printf("Source coms file name:\n");
        read(0,ifilename,80); 
	sptr=ifilename;
        sscanf(ifilename,"%s\n",strng);
	fin=fopen(strng,"r");
        printf("Destination coms file name: (do not use the same name)\n");
        read(0,ofilename,80);
	sscanf(ofilename,"%s\n",strng);
	fout=fopen(strng,"w");
        while(fgets(strng,1024,fin)) {
          printf("%s",strng);
	  sptr=strng;
          while(*sptr) {
            if (*sptr=='&') 
              *sptr='%';
            sptr++;
          }
          sscanf(strng,"%c%s ",&tintin_char,comm);
          if (is_abrev(comm,"nop"))
            fprintf(fout,"%s\n",strng);
          else {
            sptr=(strng+strlen(comm)+2);
            if (*sptr!='"') {
               sptr=get_arg_stop_spaces(sptr,arg1);
            }
            else {
               sptr=get_arg_in_quotes(sptr,arg1);
            } 
            if (*sptr!='"') {
               strcpy(arg2,sptr);
               *(arg2+strlen(arg2)-1)='\0';
            }
            else {
               strcpy(arg2,sptr+1);
               *(arg2+strlen(arg2)-1)='\0';
	       if (*(arg2+strlen(arg2)-1)=='"') {
	         *(arg2+strlen(arg2)-1)='\0';
	       }
	    }
            if (is_abrev(comm,"antisubstitute")) {
               fprintf(fout,"%c%s %c%s%c\n",tintin_char,comm,dop,arg1,dc);
            }
            else if(is_abrev(comm,"highlight")) {
              strcpy(arg2,arg1);
              strcpy(arg1,highlight_type);
              fprintf(fout,"%c%s %c%s%c %c%s%c\n",tintin_char,comm,dop,arg1,dc,dop,arg2,dc);
            }
            else {
              convert(arg2);   
              fprintf(fout,"%c%s %c%s%c %c%s%c\n",tintin_char,comm,dop,arg1,dc,dop,arg2,dc);
            }
          }  
        }
        close(fin);
        close(fout);
}
int is_abrev(s1, s2)
     char *s1;
     char *s2;
{
  return !strncmp(s1,s2,strlen(s1));
}
char *get_arg_stop_spaces(line, arg)
     char *line;
     char *arg;
{
     	while(*line!=' ' && *line!='\0' && *line!='\n')
          *arg++= *line++;
        *arg='\0';
        line++;
        return line;
}
char *get_arg_in_quotes(line, arg)
     char *line;
     char *arg;
{
        int nest=0;
        line++;
        while(!(nest==0 && *line=='"') && *line) {
		if (*line=='{')
                  nest++;
                if (*line=='}')
                  nest--;
                if (*line=='\\' && *(line+1)=='"')
		  line++;
	        *arg++= *line++;
        }
   	*arg='\0';
        line+=2;
        return line;
}
void fix_vars(arg2)
     char *arg2;
{
  int nest;
  char result[1024], *ptr,*arg, temp[1024];
  strcpy(temp,arg2);
  ptr=result;
  arg=temp;
  nest=0;
  while (*arg) {
    if (*arg=='{') {
      nest++;
      arg++;
    }
    else if (*arg=='}') {
      nest--;
      arg++;
    }
    else if ((*arg=='%' || *arg=='$') && nest==0) {
      *ptr++= *arg;
      *ptr++= *arg++;
    }
    else if ((*arg=='%' || *arg=='$') && nest>1) {
      *ptr++='\\';
      *ptr++= *arg++;
    }
    else  
      *ptr++= *arg++;
  }
  *ptr='\0';
  strcpy(arg2,result);
}
void convert(targ)
     char *targ;
{
 	char *ptr, result[1024], *ptr2, strng[1024], comm[1024], arg2[1024];
        char line[1024], temp[1024], *arg;
	int nest=0;
        char t_char;
        ptr=strng;
        arg=targ;
	*result='\0';
	while (*arg) {
          ptr=strng;
	  while (*arg==' ')
            arg++;
          while (!(*arg==';' && nest==0) && *arg!='\0') { 
            if(*arg=='{') {
	      nest++;
	      *ptr++= *arg++;
	    }
	    else if (*arg=='}') {
	      nest--;
	      *ptr++= *arg++;
	    }
	    else *ptr++= *arg++;
          } 
	  *ptr='\0';
	  sscanf(strng,"%c%s ",&t_char,comm);
	  if (t_char!=tintin_char) {
            strcpy(comm,"{");
	    strcat(comm,strng);
	    fix_vars(comm);
            strcat(result,comm);
	  }
          else {
	   if (is_abrev(comm, "nop")) {
	     strcat(result,strng);
           }
	   else if(isdigit(*comm) || is_abrev(comm,"system") || is_abrev("un", comm)) {
             sprintf(temp, "%c%s ",t_char, comm);
             strcat(result,temp);
             ptr2=strng+strlen(comm)+2;
             fix_vars(ptr2);
             if (*(ptr2)!='"')
	       strcpy(arg2,ptr2);
             else 
	       ptr2=get_arg_in_quotes(ptr2,arg2);
             
             strcat(result, " ");
             strcat(result, so);
	     strcat(result, arg2);
             strcat(result, sc);
           }
           else {
	    strcpy(line,strng);
	    fix_vars(line);
	    sprintf(temp, "%c%s ", t_char, comm);
	    strcat(result,temp);
	    ptr2=line+strlen(comm)+2;
            if (*ptr2!='"')
	      ptr2=get_arg_stop_spaces(ptr2,arg2);
            else 
	      ptr2=get_arg_in_quotes(ptr2,arg2);
            if(is_abrev(comm,"highlight")) {
	      strcat(result, so);
              strcat(result, highlight_type);
              strcat(result, sc);
	      strcat(result, " ");
	      strcat(result, so);
	      strcat(result, arg2);
	      strcat(result, sc);
            }
	    else if (is_abrev(comm,"antisubstitute")) {
	      strcat(result, so);
	      strcat(result, arg2);
	      strcat(result, sc);
	    }
	    else {
	      if(*arg2) {
	        strcat(result, so);
	        strcat(result, arg2);
	        strcat(result, sc);
	        strcat(result, " ");
	      } 
	      if(*ptr2=='"')
	        get_arg_in_quotes(ptr2,arg2);
	      else
	        strcpy(arg2,ptr2);
	      if(*arg2) {
	        strcat(result, so);
	        strcat(result, arg2);
	        strcat(result, sc);
	      }
	     }
	    }
         }
         if (*arg==';') {
	   strcat(result,";");
	   arg++;
         }
       }
       strcpy(targ, result);
}
