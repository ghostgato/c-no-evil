#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

int
isvalid_file (const char *filename)
{
    if (!filename)
    {
        fprintf(stderr, "[ERR] No filename provided\n");
        return 0;
    }

    int status = 1; // start valid file status

    // check filename for illegal chars
    for (int idx = 0; filename[idx] != '\0'; idx++)
    {
        char curr = filename[idx];

        // find ASCII chars below letter table or look for invalid chars
        // allows forward slash and period
        if ((unsigned)curr < 32 || strchr("<>:\"\\|?*", curr))
        {
            status = 0;
            break;
        }
    }

    // check if file exists
    struct stat filestat;
    if (stat(filename, &filestat) != 0)
    {
        status = 0;
    }

    // check for regular file
    if (!S_ISREG(filestat.st_mode))
    {
        status = 0;
    }

    // check empty file
    if (!filestat.st_size)
    {
        status = 0;
    }

    return status;
}