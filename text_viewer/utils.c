#include "utils.h"

int textWidthToHScroll( TEXTDATA *td, TEXTRNDDATA *trd, int minScroll, int maxScroll )
{
    if (td->maxStrWidth - trd->symsPerW - 1 > 0)
        return minScroll + trd->xLeftUp * (float)(maxScroll - minScroll) /
                    (td->maxStrWidth - trd->symsPerW - 1);
    return minScroll + trd->xLeftUp * (float)(maxScroll - minScroll) /
                    (trd->symsPerW - 1);
}

int textHeightToVScroll( TEXTDATA *td, TEXTRNDDATA *trd, int minScroll, int maxScroll )
{
    if (td->strCount - 1 - trd->symsPerH > 0)
        return minScroll + trd->yLeftUp * (float)(maxScroll - minScroll) /
                    (td->strCount - 1 - trd->symsPerH);
    return minScroll + trd->yLeftUp * (float)(maxScroll - minScroll) /
                (td->strCount - 1);
}

int hScrollToTextWidth( TEXTDATA *td, TEXTRNDDATA *trd, int scrollPos, int minScroll, int maxScroll )
{
    if (td->maxStrWidth - trd->symsPerW - 1 > 0)
            return (float)(scrollPos - minScroll) / (maxScroll - minScroll) *
                (td->maxStrWidth - trd->symsPerW - 1);

    return 0;
}

int vScrollToTextHeight( int endYLeftUp, int scrollPos, int minScroll, int maxScroll )
{
    return (float)(scrollPos - minScroll) / (maxScroll - minScroll) * endYLeftUp;
}

void LineUp( MODE m, TEXTDATA *td, TEXTRNDDATA *trd )
{
    if (m == MODE_ORIGINAL)
        trd->yLeftUp = max(0, trd->yLeftUp - 1);
    else
    {
        int strTL;
        if (trd->curLineInStr <= 0)
        {
            trd->yLeftUp--;
            if (trd->yLeftUp < 0)
            {
                trd->yLeftUp = 0;
                trd->curLineInStr = 0;
                return;
            }
            strTL = strTextLength(td, trd->yLeftUp);
            trd->curLineInStr = max(0, linesInCurStr(strTL, trd) - 1);
        }
        else
            trd->curLineInStr--;
    }
}

void LineDown( MODE m, TEXTDATA *td, TEXTRNDDATA *trd )
{
    int endYLeftUp, endCurLineInStr;
    endOfDocument(m, td, trd, &endYLeftUp, &endCurLineInStr);
    if (m == MODE_ORIGINAL)
        trd->yLeftUp = min(trd->yLeftUp + 1, endYLeftUp);
    else
    {
        int strTL = strTextLength(td, trd->yLeftUp);

        if (trd->curLineInStr >= linesInCurStr(strTL, trd) - 1 ||
            trd->yLeftUp == endYLeftUp)
        {
            trd->yLeftUp = min(endYLeftUp, trd->yLeftUp + 1);
            trd->curLineInStr = (trd->yLeftUp == endYLeftUp) * endCurLineInStr;
        }
        else if (!(trd->yLeftUp == endYLeftUp &&
                   trd->curLineInStr == endCurLineInStr))
            trd->curLineInStr++;
    }
}

void PageUp( MODE m, TEXTDATA *td, TEXTRNDDATA *trd )
{
    if (m == MODE_ORIGINAL)
        trd->yLeftUp -= min(trd->yLeftUp, trd->symsPerH - 1);
    else
    {
        int skipped = 0, toSkip = trd->symsPerH - 1; // how many strings are printed
        for (; skipped < toSkip;)
        {
            int skippedOnIteration = min(trd->curLineInStr + 1, toSkip - skipped);

            if (skippedOnIteration == toSkip - skipped && trd->curLineInStr != 0)
                trd->curLineInStr -= skippedOnIteration;
            else
            {
                trd->yLeftUp--;
                /* start of document */
                if (trd->yLeftUp < 0)
                {
                    trd->yLeftUp = 0;
                    trd->curLineInStr = 0;
                    return;
                }
                int strTL = strTextLength(td, trd->yLeftUp);
                trd->curLineInStr = linesInCurStr(strTL, trd) - 1;
            }

            skipped += skippedOnIteration;
        }
    }
}

void PageDown( MODE m, TEXTDATA *td, TEXTRNDDATA *trd )
{
    int endYLeftUp, endCurLineInStr;
                endOfDocument(m, td, trd, &endYLeftUp, &endCurLineInStr);

    if (m == MODE_ORIGINAL)
        trd->yLeftUp = min(endYLeftUp,
                            trd->yLeftUp + trd->symsPerH - 1);
    else
    {
        int skipped = 0, toSkip = trd->symsPerH - 1; // how many strings are printed
        for (; skipped < toSkip;)
        {
            int strTL = strTextLength(td, trd->yLeftUp);
            int lineCount = linesInCurStr(strTL, trd);
            int skippedOnIteration = min(lineCount - 1 - trd->curLineInStr + 1, toSkip - skipped);

            trd->curLineInStr =
                /*(skippedOnIteration == lineCount - trd->curLineInStr) * 0 +*/
                (skippedOnIteration == toSkip - skipped) * (trd->curLineInStr + skippedOnIteration);
            skipped += skippedOnIteration;
            trd->yLeftUp += (trd->curLineInStr == 0);

            // finish of document
            if (trd->yLeftUp >= endYLeftUp)
            {
                trd->yLeftUp = endYLeftUp;
                trd->curLineInStr = endCurLineInStr;
            }
        }
    }
}

void invalidateScreen( HWND hWnd, TEXTRNDDATA *trd, TEXTMETRIC *tm )
{
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = (trd->symsPerW + 1) * tm->tmAveCharWidth;
    rc.bottom = (trd->symsPerH + 1) * tm->tmHeight;
    InvalidateRect(hWnd, &rc, TRUE);
}
