
#ifndef __TEXT_PROCESSING_H_
#define __TEXT_PROCESSING_H_
#include <windows.h>

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#include "..\menu.h"

/* Render mode */
typedef enum
{
    MODE_ORIGINAL = MENU_ORIGINAL,
    MODE_FORMATTED = MENU_FORMATTED
} MODE;

typedef struct tagTEXTDATA {
    char *text;         /* text to output */
    int *strOffsets;    /* strings offsets */
    int strCount;       /* string count */
    int maxStrWidth;    /* length of the widest string */
} TEXTDATA;

typedef struct tagTEXTRNDDATA {
    int xLeftUp, yLeftUp;   /* Current upper left (x, y) point */
    int symsPerW, symsPerH; /* client region metrics */
    int curLineInStr;       /* current wrapped string in
                             * original string
                             *(actual in layout
                             * mode. in view - zero
                             */
} TEXTRNDDATA;




 #ifdef __cplusplus
extern "C"
{
#endif

/* Read file functions.
 * ARGUMENTS:
 *   IN:
 *   - file name:
 *       char const *name;
 *   OUT:
 *   - text representation:
 *       TEXTDATA *data;
 * RETURNS:
 *   (BOOL) TRUE if success, FALSE otherwise.
 */

BOOL DLL_EXPORT readFile( char const *name, TEXTDATA *data );



/* Find width of the widest string in text function.
 * ARGUMENTS:
 *   IN:
 *   - text representation:
 *       TEXTDATA *data;
 *   - edges of searching space:
 *       int yStart, yEnd;
 * RETURNS:
 *   (int) width of the widest string.
 */
int DLL_EXPORT findMaxStrWidth( TEXTDATA *td, int yStart, int yEnd );

/* Free memory function.
 * ARGUMENTS:
 *   INOUT:
 *   - text representation:
 *       TEXTDATA *data;
 * RETURNS: None.
 */
void DLL_EXPORT freeTextData( TEXTDATA *td );

/* Find string length in bytes function.
 * ARGUMENTS:
 *   IN:
 *   - text representation:
 *       TEXTDATA *data;
 *   - string number:
 *       int number;
 * RETURNS:
 *   (int) string length.
 */
int DLL_EXPORT strByteLength( TEXTDATA *td, int number );

/* Find string length in chars function.
 * ARGUMENTS:
 *   IN:
 *   - text representation:
 *       TEXTDATA *data;
 *   - string number:
 *       int number;
 * RETURNS:
 *   (int) string length.
 */
int DLL_EXPORT strTextLength( TEXTDATA *td, int number );

/* Count of lines to be rendered in LAYOUT mode.
 * ARGUMENTS:
 *   - length of string in chars:
 *       int strTL;
 *   - text render representation:
 *       TEXTRNDDATA *trd;
 * RETURNS:
 *   (int) count of lines.
 */
int DLL_EXPORT linesInCurStr( int strTL, TEXTRNDDATA *trd );

/* find document end method.
 * endYLeftUp, endCurLineInStr are TEXTRNDDATA parameters
 * which will let us see document finish*/
void DLL_EXPORT endOfDocument( MODE m, TEXTDATA *td, TEXTRNDDATA *trd,
                    int *endYLeftUp, int *endCurLineInStr );




#ifdef __cplusplus
}
#endif

#endif
