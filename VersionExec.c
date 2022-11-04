/*
    VersionExec
    by Bruno FREDERIC
    sept 2022
    Works on all kickstart version 1.0+.

    Looks at value SysBase->LibNode.lib_Version from Exec


    What I saw for SysBase->LibNode.lib_Version :
        1  for Kickstart 1.0 → transformed in 30
        31               1.1
        33               1.2
        34               1.3
        36               2.0
        36               2.02
        37               2.04 and 2.05
        39               3.0
        40               3.1
        46               3.1.4
        47               3.2 and 3.2.1
*/

#include <proto/exec.h>
#include <exec/execbase.h>
#include <proto/dos.h>
#include <stdlib.h>                     // strtol()
#include <errno.h>                      // strtol() error value
#include <string.h>                     // strlen()
#ifdef _DEBUG
#include "kick-1_0-functions.h"         // myprintf()
#else
// inclure "kick-1_0-functions.h" ferait  passer le binaire release de 336 à 616 octets !
#define CONST_STRLEN(s)     (sizeof(s)/sizeof(s[0]))
#endif

struct DosLibrary *DOSBase;
BPTR out;

int main(char *command)
{
        /* OldOpenLibrary() to be KS 1.0 compatible */
        DOSBase = (struct DosLibrary *) OldOpenLibrary("dos.library");
        if (! DOSBase) return RETURN_FAIL;

        out = Output();

        errno = 0;
        char * endPtr;
        long versionRequired = strtol(command, &endPtr, 10);

        #ifdef _DEBUG
        myprintf(SysBase->LibNode.lib_IdString);
        myprintf("argument = %s\n", command);
        myprintf("argument strtol() -> %ld\n", versionRequired);
        #endif

        // Checking conversion :
        const char *err_str = NULL;
        const char err_not_reconized [] = "L'argument n'est pas reconnu comme un nombre !\n";
        const char err_range []         = "Le nombre est trop grand !\n";
        if ((*endPtr) == '\0')
        {
                if (endPtr == command)          // Nothing recognized
                {
                        Write(out, err_not_reconized, CONST_STRLEN(err_not_reconized));
                }
                else if (errno == ERANGE)       // Reconized but number too big
                {
                        Write(out, err_range, CONST_STRLEN(err_range));
                }

                // else ->string has been completely recognized as a number
        }
        else  // Partially recognized
        {
                Write(out, err_not_reconized, CONST_STRLEN(err_not_reconized));
        }

        if (DOSBase) CloseLibrary((struct Library *)DOSBase);

        if (err_str != NULL)
        {
                return RETURN_FAIL;
        }
        else
        {
                UWORD versionExec = SysBase->LibNode.lib_Version;
                if (versionExec == 1 && SysBase->LibNode.lib_Revision == 2)
                {
                        versionExec = 30;  // exec v1.2 that is Kickstart 1.0, actually !
                }

                // Comparing to exec library major version (equivalent to KS version on classic Amiga) :
                if (versionExec >= versionRequired)
                {
                        return RETURN_OK;
                }
                else
                {
                        return RETURN_WARN; // Like the VERSION command
                        // https://wiki.amigaos.net/wiki/AmigaOS_Manual:_AmigaDOS_Command_Reference#VERSION
                }
        }
}
