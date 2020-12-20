#ifndef __TEXT_PROCESSOR_H_
#define __TEXT_PROCESSOR_H_

#include <windows.h>

#include "menu.h"

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
BOOL readFile( char const *name, TEXTDATA *data );

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
int findMaxStrWidth( TEXTDATA *td, int yStart, int yEnd );

/* Free memory function.
 * ARGUMENTS:
 *   INOUT:
 *   - text representation:
 *       TEXTDATA *data;
 * RETURNS: None.
 */
void freeTextData( TEXTDATA *td );

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
int strByteLength( TEXTDATA *td, int number );

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
int strTextLength( TEXTDATA *td, int number );

/* Count of lines to be rendered in LAYOUT mode.
 * ARGUMENTS:
 *   - length of string in chars:
 *       int strTL;
 *   - text render representation:
 *       TEXTRNDDATA *trd;
 * RETURNS:
 *   (int) count of lines.
 */
int linesInCurStr( int strTL, TEXTRNDDATA *trd );

/* find document end method.
 * endYLeftUp, endCurLineInStr are TEXTRNDDATA parameters
 * which will let us see document finish*/
void endOfDocument( MODE m, TEXTDATA *td, TEXTRNDDATA *trd,
                    int *endYLeftUp, int *endCurLineInStr );

#endif
