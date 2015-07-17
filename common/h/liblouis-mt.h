/**
 * @file liblouis-mt.h
 * Minimal header providing declarations for the LibLouis library, since the 
 * full LibLouis headers conflict badly with boost headers.
 * 
 * @copyright Copyright 2015 Anthony Tibbs.
 * This project is released under the GNU General Public License.
*/

#ifndef __LIBLOUIS_MT_H__
#define __LIBLOUIS_MT_H__



// Braille dots
#define BD_123456 "="
#define BD_12346 "&"
#define BD_12356 "("
#define BD_1246 "$"
#define BD_12456 "]"
#define BD_1256 "\\"
#define BD_126 "<"
#define BD_1456 "?"
#define BD_146 "%"
#define BD_156 ":"
#define BD_16 "*"
#define BD_2 "1"
#define BD_23 "2"
#define BD_2346 "!"
#define BD_23456 ")"
#define BD_235 "6"
#define BD_2356 "7"
#define BD_236 "8"
#define BD_246 "["
#define BD_25 "3"
#define BD_256 "4"
#define BD_26 "5"
#define BD_34 "/"
#define BD_345 ">"
#define BD_346 "+"
#define BD_35 "9"
#define BD_356 "0"
#define BD_36 "-"
#define BD_3456 "#"
#define BD_4 "@"
#define BD_45 "^"
#define BD_456 "_"
#define BD_46 "."
#define BD_5 "\""
#define BD_56 ";"
#define BD_6 ","


// Type maps
#define ll_widechar unsigned short int
#define ll_formtype unsigned char

// Tables to use
#define LIBLOUIS_UEB_G1_TABLE "en-ueb-g1.ctb"

// LibLouiS uses the MAXSTRING value internally for certain buffers and so 
// we cannot exceed that here.  
#define LIBLOUIS_MAXSTRING 512

extern "C" {

  int extParseChars (const char *inString, ll_widechar * outString);
  char *showString (ll_widechar const *chars, int length);
  int lou_translateString (const char *tableList, 
			 const ll_widechar * inbuf, 
			 int *inlen, 
			 ll_widechar * outbuf, 
			 int *outlen, 
			 ll_formtype *typeform, 
			 char *spacing, 
			 int mode);
  void lou_free (void);
}

#endif
