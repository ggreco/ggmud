#ifdef WITH_LUA

#include "ggmud.h"
#include "config.h"
#include <string.h>
#include <ctype.h>
#include "include/parse.h"

int do_luashow(lua_State *s)
{
  const char *msg = luaL_checkstring(s, 1);
  char *d = ParseAnsiColors(msg);

  strcat(d, "\n");
  textfield_add(mud->text, d, MESSAGE_LOCAL);

  return 0;
}

int do_luawindow(lua_State *s)
{
    window_entry *entry;
    const char *left = luaL_checkstring(s,1), 
	       *right = luaL_checkstring(s,2);

    if(!*left || strlen(left) > 31)
        return 0;

    if(!(entry = in_window_list(left))) {
        int width = 400, height = 300;
        // anche qui metto la possibilita' di settare le dimensioni?        
        entry = create_new_entry(left, width, height);

        gtk_window_activate_focus(GTK_WINDOW(mud->window));
    }


    if(*right && entry) {
        char *result;
        result = ParseAnsiColors(right);
        strcat(result, "\n");
        textfield_add((GtkTextView *)entry->listptr, result , MESSAGE_LOCAL);
    }

    return 0;
}

int do_luasend(lua_State *s)
{
    if (mud->activesession) {
        const char *t = luaL_checkstring(s, 1);
        mud->activesession = parse_input(t, mud->activesession);
    }

    return 0;
}

void script_command(char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], buffer[BUFFER_SIZE];

  get_arg_in_braces(arg, left, 1);

  if (!strstr(left, ".lua") && !strchr(left, '.')) {
      strcat(left, ".lua");
  }
  
  if (luaL_loadfile(mud->lua, left)) {

      snprintf(buffer, BUFFER_SIZE, "Unable to load LUA script %s: %s\n", left,
              lua_tostring(mud->lua, -1));
      textfield_add(mud->text, buffer, MESSAGE_ERR);
      lua_pop(mud->lua, 1);
      return;
  }

  if (lua_pcall(mud->lua, 0, 0, 0)) {
      snprintf(buffer, BUFFER_SIZE, "Error in LUA script %s: %s\n", left,
		      lua_tostring(mud->lua, -1));
      lua_pop(mud->lua, 1);
      textfield_add(mud->text, buffer, MESSAGE_ERR);
  }
}

void lua_command(char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], buffer[BUFFER_SIZE];

  get_arg_all(arg, left);

  if (luaL_loadstring(mud->lua, left)) {
      snprintf(buffer, BUFFER_SIZE, "Unable to load LUA script %s: %s\n", left,
              lua_tostring(mud->lua, -1));
      textfield_add(mud->text, buffer, MESSAGE_ERR);
      lua_pop(mud->lua, 1);
      return;
  }

  if (lua_pcall(mud->lua, 0, 0, 0)) {
      snprintf(buffer, BUFFER_SIZE, "Error in LUA script %s: %s\n", left,
		      lua_tostring(mud->lua, -1));
      lua_pop(mud->lua, 1);
      textfield_add(mud->text, buffer, MESSAGE_ERR);
  }
}

void execute_luatrigger(const char *name, char **vars, int *var_len)
{
    int i, args = 0;
    
    lua_getglobal(mud->lua, name);

    for (i = 0; i < 10; i++) {
        if (var_len[i] > 0) {
            lua_pushlstring(mud->lua, vars[i], var_len[i]);
            args++;
        }
    }

    if (lua_pcall(mud->lua, args, 0, 0) != 0) {
      char buffer[1024];
      sprintf(buffer, "Error running function %s with %d args: %s\n", 
              name, args, lua_tostring(mud->lua, -1));
      textfield_add(mud->text, buffer, MESSAGE_ERR);
      lua_pop(mud->lua, 1);
    }
}

int do_luatrigger(lua_State *s)
{
    char buffer[1024];
    const char *trig = luaL_checkstring(s, 1);  
    const char *func = luaL_checkstring(s, 2);
    
    sprintf(buffer, "#action {%s} {&%s} {scripting}",
            trig, func);
            
    parse_input(buffer, NULL);

    return 0;
}

int do_luaclear(lua_State *s)
{
    if (lua_gettop(s) == 0)
        clear_text_widget(mud->text);
    else {
        const char *win = luaL_checkstring(s, 1);  
        int width, height;
        window_entry *entry;

        if (lua_gettop(s) == 2)
            width = lua_tonumber(s, 2);
        else
            width = 400;
        
        if (lua_gettop(s) == 3)
            height = lua_tonumber(s, 3);
        else
            height = 300;
        
        if(!(entry = in_window_list(win))) {
            create_new_entry(win, width, height); // creo la nuova finestra nel caso non ci sia
            gtk_window_activate_focus(GTK_WINDOW(mud->window));
        }
        else
            clear_text_widget(GTK_TEXT_VIEW(entry->listptr));

    }

    return 0;
}

int do_luauntrigger(lua_State *s)
{
    char buffer[1024];
    const char *trig = luaL_checkstring(s, 1);  
    
    sprintf(buffer, "#unaction {%s}", trig);
            
    parse_input(buffer, NULL);

    return 0;
}

int do_luaidle(lua_State *s)
{
    const char *trig = luaL_checkstring(s, 1);  
    
    if (mud->lua_idle_function)
        free(mud->lua_idle_function);
    
    mud->lua_idle_function = strdup(trig);
    
    return 0;
}

int do_luatimer(lua_State *s)
{
	return 0;
}

const char *call_luafunction(const char *string)
{
    char name[256], arg[256];
    int i = 0, args = 0;
    
    while (*string != '(' && *string != ' ' && *string != '\t' && *string != 0)
        name[i++] = *string++;
    
    if (!i)
        return string;

    name[i] = 0;

    lua_getglobal(mud->lua, name);
   
    while (*string == ' ' || *string == '\t')
        string++;

    if (*string) {
        if (*string != '(') {
            lua_pop(mud->lua, 1);
            return string;
        }
        else
            string++;

        while (*string && *string != ')') {
            int quoted = 0;

            if (*string == ',')
                string++;

            i = 0;

            while (*string == ' ')
                string++;
           
            if (*string == '\"' || *string == '[')
                quoted = *string++;

            while (quoted || (*string != ',' && *string != ')')) {
                if (!*string) {
                    char buffer[1024];
                    sprintf(buffer, "Error in lua function %s args formatting!\n", name);
                    textfield_add(mud->text, buffer, MESSAGE_ERR);
                    return string;
                }
                if (quoted &&
                    ( (quoted == '\"' && *string == '\"') ||
                      (quoted == '[' && *string == ']')) ) {
                    string++;
                    quoted = 0;
                }
                else
                    arg[i++] = *string++;

            }

            arg[i] = 0;

            lua_pushstring(mud->lua, arg);
            
            args++;
        }

        if (*string != ')') {
            lua_pop(mud->lua, 1 + args);
            return string;
        }
    }
    
    if (lua_pcall(mud->lua, args, 0, 0) != 0) {
      char buffer[1024];
      sprintf(buffer, "Error running function %s with %d args: %s\n", 
              name, args, lua_tostring(mud->lua, -1));
      textfield_add(mud->text, buffer, MESSAGE_ERR);
      lua_pop(mud->lua, 1);
    }

    return string + 1;
}

#if 0

// actually not used and maybe they'll never will

static int get_string(lua_State* L)
{
  printf("Chiamata get_string!\n");
  char **var=(char **) lua_touserdata(L,2);
  lua_pushstring(L,*var);
  return 1;
}

static int set_string(lua_State* L)
{
  printf("Chiamata set_string!\n");
  char ** var=(char **) lua_touserdata(L,2);
  const char *c =  lua_tostring(L,3);
  free(*var);
  *var = strdup(c);
  return 0;
}

// function __index(object, key)
static int __index(lua_State* L) // object, key
{
    printf("Chiamata index!\n");
    // get object's metatable
    lua_getmetatable(L, 1);      // object, key, mt

    // lookup key inside mt
    lua_pushvalue(L, 2);          // object, key, mt, key
    lua_rawget(L, -2);           // object, key, mt, mt[key]

    // return one value from the top of the stack
    return 1;
}
#endif



void init_lua()
{
    /* load libraries */
    luaL_openlibs(mud->lua);

    // set version number

    lua_pushstring(mud->lua, "VERSION");
    lua_pushstring(mud->lua, VERSION);
    lua_settable(mud->lua, LUA_GLOBALSINDEX);

    // register available commands

    lua_register(mud->lua, "show", do_luashow);
    lua_register(mud->lua, "window", do_luawindow);

    lua_register(mud->lua, "trigger", do_luatrigger);
    lua_register(mud->lua, "untrigger", do_luauntrigger);
    lua_register(mud->lua, "send", do_luasend);
    lua_register(mud->lua, "timer", do_luatimer);
    lua_register(mud->lua, "idle_function", do_luaidle);
    lua_register(mud->lua, "clear", do_luaclear);
}

void add_lua_global(const char *v1, char **v2)
{
    if (!mud->lua)
        return;

    lua_pushstring(mud->lua, v1);
    lua_pushstring(mud->lua, *v2);
    lua_settable(mud->lua, LUA_GLOBALSINDEX);
}

void get_lua_global(const char *key, char **value)
{
	const char *v;
	
	lua_getglobal(mud->lua, key);

    if ((v = lua_tostring(mud->lua, -1))) {

        if (strcmp(v, *value)) {
            if (*value)
                free(*value);
            *value = strdup(v);
        }

    }
    else 
        add_lua_global(key, value);

    lua_pop(mud->lua, 1);
}

#endif
