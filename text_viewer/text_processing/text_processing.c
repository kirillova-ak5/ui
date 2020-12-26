
#include <stdio.h>
#include <stdlib.h>
#include "text_processing.h"

int symsCount( char *arr, int len, char sym )
{
    int i;
    int cnt = 0;
    for (i = 0; i < len; i++)
        cnt += arr[i] == sym;

    return cnt;
}

BOOL DLL_EXPORT readFile( char const *name, TEXTDATA *data )
{
    int i, curStr = 1, len;
    char *buf;
    FILE *F = fopen(name, "rb");

    if (F == NULL)
        return FALSE;

    fseek(F, 0, SEEK_END);
    len = ftell(F);
    fseek(F, SEEK_SET, 0);

    freeTextData(data);

    buf = malloc(len + 1);
    if (buf == NULL)
        return FALSE;

    int resLen = fread(buf, 1, len, F);
    if (resLen != len)
    {
        free(buf);
        return FALSE;
    }

    data->strCount = symsCount(buf, len, '\n') + 1;
    data->text = buf;
    // +1 cause save last sym offset
    data->strOffsets = malloc(sizeof(int) * (data->strCount + 1));

    if (data->strOffsets == NULL)
    {
        free(data->text);
        return FALSE;
    }

    data->strOffsets[0] = 0;
    for (i = 1; i < len; i++)
        if (buf[i] == '\n')
            data->strOffsets[curStr++] = i + 1;
    data->strOffsets[data->strCount] = len;

    data->maxStrWidth = findMaxStrWidth(data, 0, data->strCount);
    return TRUE;
}

void DLL_EXPORT freeTextData( TEXTDATA *td )
{
    if (td->strOffsets != NULL)
    {
        free(td->strOffsets);
        td->strOffsets = NULL;
    }

    if (td->text != NULL)
    {
        free(td->text);
        td->text = NULL;
    }
}

int DLL_EXPORT strByteLength( TEXTDATA *td, int number )
{
    return td->strOffsets[number + 1] - td->strOffsets[number];
}

int DLL_EXPORT strTextLength( TEXTDATA *td, int number )
{
    return td->strOffsets[number + 1] - td->strOffsets[number] - (td->text[td->strOffsets[number + 1] - 2] == '\r');
}

int DLL_EXPORT linesInCurStr( int strTL, TEXTRNDDATA *trd )
{
    return strTL / trd->symsPerW + (strTL % trd->symsPerW != 0);
}

int DLL_EXPORT findMaxStrWidth( TEXTDATA *td, int yStart, int yEnd )
{
    int i;
    int maxLen = 0;
    for (i = yStart; i < yEnd; i++)
        if (td->strOffsets[i + 1] - td->strOffsets[i] > maxLen)
            maxLen = strTextLength(td, i);

    return maxLen;
}

void DLL_EXPORT endOfDocument( MODE m, TEXTDATA *td, TEXTRNDDATA *trd,
                    int *endYLeftUp, int *endCurLineInStr )
{
    int passed = 0;
    int passedOnIteration;
    int lines;
    if (m == MODE_FORMATTED)
    {
        *endYLeftUp = td->strCount - 1;
        for (; passed < trd->symsPerH;)
        {
            int
                strTL = strTextLength(td, *endYLeftUp);

            lines = linesInCurStr(strTL, trd);
            passedOnIteration = min(lines, trd->symsPerH - passed);
            passed += passedOnIteration;

            if (passed == trd->symsPerH)
                break;

            *endYLeftUp -= (passedOnIteration == lines);
            if (*endYLeftUp < 0)
            {
                *endYLeftUp = 0;
                passedOnIteration = lines; // for *endCurLineInStr = ... make 0
                break;
            }
        }
        *endCurLineInStr = lines - passedOnIteration;
    }
    else
    {
        *endYLeftUp = max(0, td->strCount - trd->symsPerH);
        *endCurLineInStr = 0;
    }
}
