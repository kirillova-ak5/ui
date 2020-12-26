#ifndef __UTILS_H_
#define __UTILS_H_

#include "text_processing\text_processing.h"

/* Move text with controls functions group.
 * ARGUMENTS:
 *  IN:
 *   - mode:
 *       MODE m;
 *   - text representation:
 *       TEXTDATA *td;
 *  INOUT:
 *   - render representation:
 *       TEXTRNDDATA *trd;
 * RETURNS: None.
 */
void LineUp( MODE m, TEXTDATA *td, TEXTRNDDATA *trd );
void LineDown( MODE m, TEXTDATA *td, TEXTRNDDATA *trd );
void PageUp( MODE m, TEXTDATA *td, TEXTRNDDATA *trd );
void PageDown( MODE m, TEXTDATA *td, TEXTRNDDATA *trd );

/* Mapping from text representation to scroll space functions group.
 * ARGUMENTS:
 *   IN:
 *   - text representation:
 *       TEXTDATA *td;
 *   - render representation:
 *       TEXTRNDDATA *trd;
 *   - scroll limits:
 *       int minScroll, maxScroll;
 * RETURNS:
 *   (int) left x position in scroll space.
 */
int textWidthToHScroll( TEXTDATA *td, TEXTRNDDATA *trd, int minScroll, int maxScroll );
int textHeightToVScroll( TEXTDATA *td, TEXTRNDDATA *trd, int minScroll, int maxScroll );

/* Inverse mapping functions group.
 * ARGUMENTS:
 *   IN:
 *   - text representation:
 *       TEXTDATA *td;
 *   - render representation:
 *       TEXTRNDDATA *trd;
 *   - current scroll positions:
 *        int scrollPos;
 *   - scroll limits:
 *       int minScroll, maxScroll;
 * RETURNS:
 *   (int) left x position in text space.
 */
int hScrollToTextWidth( TEXTDATA *td, TEXTRNDDATA *trd, int scrollPos, int minScroll, int maxScroll );
int vScrollToTextHeight( int endYLeftUp, int scrollPos, int minScroll, int maxScroll );

/* Invalidate screen function.
 * ARGUMENTS:
 *   IN:
 *   - window handler:
 *       HWND hWnd;
 *   - text representation:
 *       TEXTDATA *td;
 *   - render representation:
 *       TEXTRNDDATA *trd;
 * RETURNS: None.
 */
void invalidateScreen( HWND hWnd, TEXTRNDDATA *trd, TEXTMETRIC *tm );

#endif // __UTILS_H_
