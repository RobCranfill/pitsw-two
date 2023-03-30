#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

// adapted from https://github.com/ambotaku/pico-getLine

const uint startLineLength = 8; // the linebuffer will automatically grow for longer lines
const char eof = 255;           // EOF in stdio.h -is -1, but getchar returns int 255 to avoid blocking
/*
 *  read a line of any  length from stdio (grows)
 *
 *  @param fullDuplex input will echo on entry (terminal mode) when false
 *  @param linebreak  "\r" may be needed for terminals
 *  @return entered line on heap - don't forget calling free() to get memory back
 */
// static char * getLine(bool fullDuplex = true, char lineBreak = '\n') {
static char * getLine(bool fullDuplex, char lineBreak) {
    // th line buffer
    // will allocated by pico_malloc module if <cstdlib> gets included
    char * pStart = (char*)malloc(startLineLength); 
    char * pPos = pStart;  // next character position
    size_t maxLen = startLineLength; // current max buffer size
    size_t len = maxLen; // current max length
    int c;
    if (!pStart) {
        return NULL; // out of memory or dysfunctional heap
    }
    while (1) {
        c = getchar(); // expect next character entry
        if (c == eof || c == lineBreak) {
            break;     // non blocking exit
        }
        if (fullDuplex) {
            putchar(c); // echo for fullDuplex terminals
        }
        if (--len == 0) { // allow larger buffer
            len = maxLen;
            // double the current line buffer size
            char *pNew  = (char*)realloc(pStart, maxLen *= 2);
            if (!pNew) {
                free(pStart);
                return NULL; // out of memory abort
            }
            // fix pointer for new buffer
            pPos = pNew + (pPos - pStart);
            pStart = pNew;
        }
        // stop reading if lineBreak character entered 
        if ((*pPos++ = c) == lineBreak) {
            break;
        }
    }
    *pPos = '\0';   // set string end mark
    return pStart;
}

