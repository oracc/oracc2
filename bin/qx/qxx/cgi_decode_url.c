#include <oraccsys.h>
#include "px.h"

/*
 * Cut from ccgi.c.
 */

/*
 * hex() returns the numeric value of hexadecimal digit "digit"
 * or returns -1 if "digit" is not a hexadecimal digit.
 */

static int
hex(int digit) {
    switch(digit) {

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return digit - '0';

    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return 10 + digit - 'A';

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return 10 + digit - 'a';

    default:
        return -1;
    }
}

/*
 *
 * CGI_decode_url() returns a new string which is a copy of the input
 * string with '+' converted to ' ' and %xx converted to the character
 * whose hex numeric value is xx.
 */

char *
CGI_decode_url(const char *p)
{
    char *out;
    int i, k, L, R;

    if (p == 0) {
        return 0;
    }
    out = malloc(strlen(p) + 1);
    for (i = k = 0; p[i] != 0; i++) {
        switch(p[i]) {

        case '+':
            out[k++] = ' ';
            continue;

        case '%':
            if ((L = hex(p[i + 1])) >= 0 &&
                (R = hex(p[i + 2])) >= 0)
            {
                out[k++] = (L << 4) + R;
                i += 2;
                continue;
            }
            break;
        }
        out[k++] = p[i];
    }
    out[k] = 0;
    return out;
}
