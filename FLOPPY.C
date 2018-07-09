/*
 *	FLOPPY.C -- Version 0.9 (c) 1992 Emrys Software
 *  Programmed by Wynne Stepp
 *
 *  FLOPPY is designed to be used in batch files to allow the user to proceed
 *  immediately after the diskette has been changed.  It may take one or two
 *  parameters: a required drive letter and an optional volume name.  FLOPPY
 *  will pause until the diskette is changed.  If the volume name was specified
 *  then FLOPPY will continue pausing until the new volume is equal to the one
 *  requested.
 *
 *  It, unfortunately, relies on polling the drive since their is no documented
 *  way to block until the removable medium has been removed.  To prevent this
 *	from seriously affecting system performance, FLOPPY sleeps in the interval
 *  between polls.  I have found that 7 seconds is a reasonable amount of time
 *  to wait from a user's perspective.
 *
 *  A couple fixes from the previous version have been made on recommendation
 *  from members of the FidoNet OS2PROG echo.  First, it is no longer necessary
 *  to include the AUTOFAIL=YES parameter in CONFIG.SYS.  Second, FLOPPY now
 *  checks the volume's serial number to detect whether or not the volume has
 *  changed, so it will now work with different volumes with the same label.
 *
 */

#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define WAIT_INTERVAL	7000
#define MAXVOLUME		12

void beep (void)
    {
    DosBeep (800, 100) ;
    DosBeep (1200, 100) ;
    DosBeep (1600, 200) ;
    }
	
int QueryVolumeLabel (char chDrive, char *pszVolume, ULONG *pidVolume)
    {
    FSINFO  fsinf ;
    int     iError ;

    if (0 == (iError = DosQFSInfo (toupper (chDrive) - 'A' + 1, 2, (PBYTE) &fsinf, sizeof (fsinf))))
        {
        strcpy (pszVolume, fsinf.vol.szVolLabel) ;
        *pidVolume = fsinf.ulVSN ;
        }

    return iError ;
    }

void WaitUntilVolumeChanged (char chDrive, char *pszVolume)
    {
    char    szVolume [MAXVOLUME] ;
    ULONG   idOldVolume, idVolume ;

    DosError (HARDERROR_DISABLE) ;

    QueryVolumeLabel (chDrive, szVolume, &idVolume) ;
    idOldVolume = idVolume ;

    while ((pszVolume && strcmpi (szVolume, pszVolume)) || (!pszVolume && idOldVolume == idVolume))
        {
        DosSleep (WAIT_INTERVAL) ;
        QueryVolumeLabel (chDrive, szVolume, &idVolume) ;
        }
	}

int main (int argc, char *argv[])
    {
    if (argc < 2)
        {
        printf ("FLOPPY 0.9\n") ;
        printf ("Copyright (c) 1992 Emrys Software & Wynne Stepp\n\n") ;
        printf ("Usage:  FLOPPY drv [volume label]\n\n") ;
        printf ("* AUTOFAIL=YES is no longer needed in CONFIG.SYS\n") ;

        return 1 ;
        }

    printf ("Please insert the %s diskette now.\n", argc > 2 ? argv [2] : "new") ;
    beep () ;

	WaitUntilVolumeChanged (*argv [1], argc > 2 ? argv [2] : NULL) ;
	
    return 0 ;
    }







