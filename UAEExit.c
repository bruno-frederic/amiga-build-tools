/*
    UAEExit

    Utilise UAE.resource qui n'existe pas pour les machines sous KS 1.0 et KS 1.1, dans ce cas
    regarde s'il y a une calltrap à l'adresse $00F0FF60

    Dans la configuration UAE, écran "Hardware info", on trouve l'adresse de "UAE Boot ROM".

    Pour que cela marche, il faut au moins un hard drive déclaré dans la configuration WinUAE :
    http://eab.abime.net/showthread.php?p=1441505#post1441505


    J'ai pu constaté que pour toutes les machines uae_rombase c'est 0xF00000.
        VPrintf("uae_rombase=0x%lx\n", &(UAEResource->uae_rombase));
    sauf sur l'A3000 qui a une ROM "A3000 SCSI" avant. C'est pour cela que ça fasait planter dessus.
        A3000, 2.0  ROM (r36.143) uae_rombase -> 0xEF0000
        A3000, 2.02 ROM (r36.207) uae_rombase -> 0xEF0000
        A3000, 3.1  ROM uae_rombase -> 0xEF0000

    Pour regarder aux alentours de l'adresse habituelle de UAE base rom, avec le debuggeur :
    m $00f0FD00,1024
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include "kick-1_0-functions.h"  // WRITE_CONST_STR(), myprintf()
#include "uae_pragmas.h"

struct DosLibrary *DOSBase;
BPTR out;

int main (char *command)
{
    enum { INVALID_PARAMETER, EXIT, RESET, HARDRESET } mode = INVALID_PARAMETER;
    int result = RETURN_FAIL;

    /* OldOpenLibrary() to be KS 1.0 compatible */
    DOSBase = (struct DosLibrary *) OldOpenLibrary("dos.library");
    if (! DOSBase) return RETURN_FAIL;

    out = Output();

    if      (strcmp(command,"EXIT"     ) == 0)  mode=EXIT;
    else if (strcmp(command,"RESET"    ) == 0)  mode=RESET;
    else if (strcmp(command,"HARDRESET") == 0)  mode=HARDRESET;
    else
    {
        WRITE_CONST_STR("Error: invalid parameter!\nUsage (case-sensitive):\nUAEexit EXIT\nUAEexit RESET\nUAEexit HARDRESET\n");
    }


    // La fonction InitUAEResource tente d'ouvrir "UAE.resource" par une appel Exec.
    // Cela permet de retrouver la bonne adresse pour le calltrap, même sur A3000 où elle est
    // à une adresse inhabituelle.

    BOOL bUAEcalltrapFound = InitUAEResource();
    if ( bUAEcalltrapFound )
    {
        #ifdef _DEBUG
            myprintf( "UAEResource found: UAEResource->uae_rombase=0x%08.lx\n",
                     UAEResource->uae_rombase);
        #endif
    }
    else
    {
        // Si la méthode propre ne fonctionne pas, on tente une autre méthode en lisant à l'adresse
        // mémoire 0x00F0FF60 qui habituellement celle du calltrap de UAE.
        // Cela permet de faire fonctioner en Kickstart 1.0 et 1.1.
        // Inspiré de : https://eab.abime.net/showthread.php?p=913298#post913298

        WRITE_CONST_STR("UAE resource not found.\n");

        // Bon exemple de lecture d'une adresse mémoire et de son affichage en language C
        // inspiré de https://stackoverflow.com/a/2389273/11867971
        volatile ULONG* myPointer = (volatile unsigned long *)0x00F0FF60UL;
        #ifdef _DEBUG
            myprintf("myPointer=0x%08.lx\t*myPointer=0x%08.lx\n", myPointer, *myPointer);
        #endif // _DEBUG

        /* Pour info : les fonctions de bartman dans gcc8_c_support.c teste un autre truc aussi :
            long(*UaeDbgLog)(long mode, const char* string) = (long(*)(long, const char*))0xf0ff60;
            if(*((UWORD *)UaeDbgLog) == 0x4eb9 || *((UWORD *)UaeDbgLog) == 0xa00e) {
        */
        if (*myPointer == 0xA00E4E75UL)
        {
            WRITE_CONST_STR("However UAE calltrap has been found at memory address 0x00F0FF60!\n");
            bUAEcalltrapFound = TRUE;
        }
    }

    if (bUAEcalltrapFound)
    {
        result = RETURN_OK;

        #ifdef _DEBUG
            // Pause en debugging pour lire les messages :
            WRITE_CONST_STR("Press Return to continue...\n");
            char c;
            Read(Input(), &c, 1);
        #endif

        switch (mode)
        {
        case EXIT:
            ExitEmu();
            break;
        case RESET:
            Reset();
            break;
        case HARDRESET:
            HardReset();
            break;

        default:
            result = RETURN_ERROR;
            break;
        }
    }
    else
    {
        WRITE_CONST_STR("UAE calltrap not found at all!\n");
        result = RETURN_ERROR;
    }

    if (DOSBase) CloseLibrary((struct Library *)DOSBase);
    return result;
}
