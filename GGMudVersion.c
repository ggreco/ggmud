#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>

char ROOTDIR[512] = "/var/www/html/ggmud";

typedef int(*compare_proto)(const void *, const void *);

int mycompare(char **str1, char **str2)
{
    return strcasecmp(*str1, *str2);
}

char *parse_arg(char *buf, const char *key)
{
    if (strncasecmp(buf, key, strlen(key)) != 0)
        return NULL;

    if (buf[strlen(key)] != '=')
        return NULL;

    return buf + strlen(key) + 1;
}

void check_args(char *ptr, char *version, char *cpu, char *arch)
{
    char *d;

    if ((d = parse_arg(ptr, "arch")))
        strncpy(arch, d, 31);
    else if ((d = parse_arg(ptr, "version")))
        strncpy(version, d, 31);
    else if ((d = parse_arg(ptr, "cpu")))
        strncpy(cpu, d, 31);
}

void parse_args(const char *c, char *version, char *cpu, char *arch)
{
    char buffer[512], *arg, *ptr = buffer;
    strcpy(buffer, c);

    while (*ptr && (arg = strchr(ptr, '&'))) {
        // solo un argomento
        *arg = 0;
        check_args(ptr, version, cpu, arch);
        ptr = arg + 1;
    }

    if (*ptr)
        check_args(ptr, version, cpu, arch);
}

int main(int argc, char *argv[])
{
    DIR *d;
    char *c = getenv("QUERY_STRING");
    char arch[32];
    char version_num[32];
    char cpu[32];

    *arch = *cpu = *version_num = 0;

    if (c && *c)  { // get args if available
        parse_args(c, version_num, cpu, arch);
    }

    printf("Content-Type: text/plain\n\n");

    // verifico l'esistenza di una configurazione specifica

    strcat(ROOTDIR, "/");

    if (*cpu) {
        char testdir[512];
        DIR *dd;

        sprintf(testdir, "%s%s", ROOTDIR, cpu);

        if ((dd = opendir(testdir))) {
            closedir(dd);
        }
        else
            strcpy(cpu, "source");
    }
    else
        strcpy(cpu, "source");

    strcat(ROOTDIR, cpu);

    if ((d = opendir(ROOTDIR))) {
        struct dirent *e;
        int size = 2;
        char **ver = malloc(size * sizeof(char *));
        int versions = 0;
#if 0

        while ((e = readdir(d))) {
            int len = strlen(e->d_name);

            if (*e->d_name == '.' || len < 5)
                continue;
            
            if (strcasecmp(&e->d_name[len - 4], ".zip") != 0)
                continue;

            ver[versions] = strdup(e->d_name);
            
            versions++;
            
            if(versions == size) {
                size *= 2;

                ver = realloc(ver, size * sizeof(char *));
            }
 
        }

        if (versions > 1) {
            ver[versions] = NULL;
            qsort(ver, versions, sizeof(char *), (compare_proto)mycompare);
        }

        if (ROOTDIR[strlen(ROOTDIR) -1] == '/')
            ROOTDIR[strlen(ROOTDIR) -1] = 0;

        // occhio che il checker ragiona sulle parole "newer", "notfound", altrimenti
        // pensa che l'URL sia valido!

#endif
        if (versions > 0) {
            if (*version_num && strstr(ver[versions - 1], version_num))
                printf("Already NEWEST version (%s)\n", version_num);
            else
                printf("%s/%s", ROOTDIR + strlen(TOSTRIP), ver[versions - 1]);
        }
        else
            printf("Version notfound\n");

        closedir(d);
    }
    else
        printf("Directory notfound\n");

    return 0;
}

