#ifdef WITH_LUA

#include "ggmud.h"
#include "config.h"

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
        char t1[BUFFER_SIZE], t2[BUFFER_SIZE];
        char *result;
        substitute_myvars(right, t1, s);
        substitute_vars(t1, t2, s);
        result = ParseAnsiColors(t2);
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

  if (luaL_loadfile(mud->lua, left)) {

      sprintf(buffer, "Unable to load LUA script: %s\n", left);
      textfield_add(mud->text, buffer, MESSAGE_ERR);
      return;
  }

  if (lua_pcall(mud->lua, 0, 0, 0)) {
      sprintf(buffer, "Error in LUA script %s: %s\n", left,
		      lua_tostring(L, -1));
      textfield_add(mud->text, buffer, MESSAGE_ERR);
  }
}

void execute_luatrigger(const char *name, char **vars, int *var_len)
{
    int i, args = 0;
    
    lua_getglobal(mud->lua, name);

    for (i = 0; i < 10; i++) {
        if (var_len[i] > 0) {
            lua_pushstring(mud->lua, vars[i]);
            args++;
        }
    }

    if (lua_pcall(mud->lua, args, 0, 0) != 0) {
      char buffer[1024];
      sprintf(buffer, "Error running function %s with %d args\n", name, args);
      textfield_add(mud->text, buffer, MESSAGE_ERR);
    }
}

int do_luatrigger(lua_State *s)
{
    char buffer[1024];
    const char *trig = luaL_checkstring(s, 1);  
    
    if (!lua_isfunction(s, 2)) {
	    char buffer[512];
	    sprintf(buffer, "%s is not a function!\n", lua_tostring(s, 2));
	    textfield_add(mud->text, buffer, MESSAGE_ERR);
	    return 0;
    }
    
    sprintf(buffer, "#action {%s} {\\%s} {scripting}",
            trig, lua_tostring(s, 2));
            
    parse_input(buffer, NULL);

    return 0;
}

int do_luatimer(lua_State *s)
{
	return 0;
}

void init_lua()
{
    luaopen_base(mud->lua);

    // set version number

    lua_pushstring(mud->lua, "VERSION");
    lua_pushstring(mud->lua, VERSION);
    lua_settable(mud->lua, LUA_GLOBALSINDEX);

    // register available commands

    lua_register(mud->lua, "show", do_luashow);
    lua_register(mud->lua, "window", do_luawindow);

    lua_register(mud->lua, "trigger", do_luatrigger);
    lua_register(mud->lua, "send", do_luasend);
    lua_register(mud->lua, "timer", do_luatimer);
}

void add_lua_global(const char *v1, const char *v2)
{
    if (!mud->lua)
        return;
            
    lua_pushstring(mud->lua, v1);
    lua_pushstring(mud->lua, v2);
    lua_settable(mud->lua, LUA_GLOBALSINDEX);
}

#endif
