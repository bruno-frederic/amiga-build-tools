#ifndef KICK_1_0_FUNCTIONS_H
#define KICK_1_0_FUNCTIONS_H

/*
**	$VER: dos.h 47.2 (16.16.2021)
**
**	Helper functions compatible with every kickstart version from kick 1.0+
**  support with minimal VBCC startup code.
**
**  If _DEBUG is defined, some assertion code is added to warn for common bug.
**
**	Copyright (C) 2022 Bruno FREDERIC.

Comparaison de la façon d'appeler les deux style de fonctions d'affichage :

1. avec un tableau d'arguments, comme RawDoFmt() de Dos.library (ou VPrintf() en KS 2.0+)  :
    const ULONG argarray[]= {(ULONG) myPointer, *myPointer};
    myVPrintf( "myPointer=0x%08.lx\t*myPointer=0x%08.lx\n", argarray);

2. avec une liste variable de paramètre lors de l'appel, comme printf() de stdio.h :
    myprintf("myPointer=0x%08.lx\t*myPointer=0x%08.lx\n", myPointer, *myPointer);
*/

#include <proto/exec.h> // RawDoFmt()
#include <proto/dos.h>  // Write()
#include <string.h>     // strlen()
#ifdef _DEBUG
    #include <stdlib.h>     // exit()
#endif

// Macros evaluated at compile time. Works on const char [] literals.
// More efficient than other printf() function and less risky of buffer overflow.
// Example : WRITE_CONST_STR("Hello!\n");
#define CONST_STRLEN(s)     (sizeof(s)/sizeof(s[0]))
#define WRITE_CONST_STR(s)  Write(out, s, CONST_STRLEN(s))

// Needed for Write(). DosLibrary must be opened and out initialized with Output() by caller.
struct DosLibrary *DOSBase;
BPTR out;

// Needed for RawDoFmt()
#define BUFFER_SIZE 128
static const ULONG PutChProc=0x16c04e75; // move.b d0,(a3)+ ; rts

// Prototype in 3.2 NDK : LONG VPrintf( CONST_STRPTR format, CONST_APTR argarray )
// Adapted to 1.3:
void myVPrintf( const STRPTR formatString, const APTR argarray)
{
#ifdef _DEBUG
    if (out != Output())  exit(RETURN_FAIL);
#endif

    unsigned char buf[BUFFER_SIZE];
    RawDoFmt(formatString, argarray, (void (*)())&PutChProc, buf);

#ifdef _DEBUG
    if ( strlen(buf) > sizeof(buf) )
    {
        WRITE_CONST_STR("ERROR: Buffer overflow in myVPrintf() !!!\n");
        exit(RETURN_FAIL);
    }
#endif

    Write(out, buf, strlen(buf));
}



/* printf() de bebbo qui prend un nombre variable de paramètre (comme printf() de stdio.h) :
    http://eab.abime.net/showthread.php?p=1318994#post1318994

    bebbo utilise une astuce : lorsque cette fonction est appelée, toutes les valeurs des
    arguments ont été mises l'une à la suite de l'autre dans la pile (registre a7) avec la
    convention d'appel __stdargs cf. 4.2 "ABI" du VBCC.pdf (Application binary interface)

    Il récupère donc un pointeur sur l'élément en mémoire juste après formatString :
    1. Il localise en mémoire formatString avec l'opérateur & : &formatString
    2. Il ajoute 1 à ce pointeur pour trouver l'élément juste après (les pointeurs font 32 bits par
       défaut)
    3. Enfin il caste cela en pointeur argarray acceptable pour le 2nd paramètre de RawDoFmt()

    Pour récupérer ce pointeur, on aurait pu aussi utiliser va_list et va_start() de stdarg.h
    comme dans l'exemple ci dessous inspiré de
    https://eab.abime.net/showthread.php?p=1177752#post1177752

    void a_b_printf(int first, const char* formatString, ...)  // variable number of arguments
    {
        va_list list;
        va_start(list, formatString); // formatString is the last non-variable argument
        unsigned char buf[256];

        RawDoFmt((STRPTR) formatString, list,
                (void (*)())&PutChProc, buf);

        Write(out, buf, strlen((char *)buf));

        va_end(list);
    }

    Dans un cas comme dans l'autre, le standard C ne garanti pas pour autant que les arguments se
    suivent en mémoire (il pourrait y en avoir quelques uns dans des registres et d'autre en mémoire
    principale, comme lorsque on utilise la convention d'appel __regargs de VBCC au lieu de
    __stdargs)
    cf. https://stackoverflow.com/a/48979920/11867971
*/
__stdargs void myprintf(const char *formatString, ...)
{
#ifdef _DEBUG
    if (out != Output())  exit(RETURN_FAIL);
#endif

    unsigned char buf[BUFFER_SIZE];

    STRPTR *argarray = (STRPTR *)(&formatString+1);

    RawDoFmt((STRPTR) formatString, argarray, (void (*)())&PutChProc, buf);

#ifdef _DEBUG
    if ( strlen(buf) > sizeof(buf) )
    {
        WRITE_CONST_STR("ERROR: Buffer overflow in myprintf() !!!\n");
        exit(RETURN_FAIL);
    }
#endif

    Write(out, buf, strlen((char *)buf));
}

#endif /* KICK_1_0_FUNCTIONS_H */



