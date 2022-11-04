/*
    Pause
    by Bruno FREDERIC
    sept 2022

    Pause implementation using pure Amiga DOS library call, works on all kickstart version 1.0+.
*/

#include <proto/exec.h>          /* Exec Library needed for OldOpenLibrary() */
#include <proto/dos.h>           /* Dos  Library for Read() ; Write()        */

// Do not include "kick-1_0-functions.h" which would increase release executable size from 324
// to 552 bytes ! (228 bytes more)
#define CONST_STRLEN(s)     (sizeof(s)/sizeof(s[0]))


int main()
{
    /* OldOpenLibrary() to be KS 1.0 compatible */
    struct DosLibrary *DOSBase = (struct DosLibrary *) OldOpenLibrary("dos.library");
    if (! DOSBase) return RETURN_FAIL;

    static const char msg[] = "Press Return to continue...\n";
    Write(Output(), msg, CONST_STRLEN(msg));

    char c;

    while (Read(Input(), &c, 1) > 0 && c != '\n')
    {
        // looping empties the buffer before getting back to shell
    }

    CloseLibrary((struct Library *)DOSBase);
    return RETURN_OK;
}
