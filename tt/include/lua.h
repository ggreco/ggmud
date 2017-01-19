#ifndef MY_LUA_INTERFACE_H

#define MY_LUA_INTERFACE_H

extern void execute_luatrigger(const char *name, char **vars, int *var_len);
extern void script_command(char *arg, struct session *ses);
extern void get_lua_global(const char *key, char **value);
extern const char *call_luafunction(const char *string);
void add_lua_global(const char *v1, char **v2);
void lua_command(char *arg, struct session *ses);

#endif
