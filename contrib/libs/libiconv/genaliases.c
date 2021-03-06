/* Copyright (C) 1999-2001, 2003, 2005, 2008 Free Software Foundation, Inc.
   This file is part of the GNU LIBICONV Library.

   The GNU LIBICONV Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   The GNU LIBICONV Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU LIBICONV Library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Creates the aliases.gperf table. */

#include <stdio.h>
#include <stdlib.h>

static void emit_alias (FILE* out1, const char* alias, const char* c_name)
{
  /* Output alias in upper case. */
  const char* s = alias;
  for (; *s; s++) {
    unsigned char c = * (unsigned char *) s;
    if (c >= 0x80)
      exit(1);
    if (c >= 'a' && c <= 'z')
      c -= 'a'-'A';
    putc(c, out1);
  }
  fprintf(out1,", ei_%s\n", c_name);
}

static void emit_encoding (FILE* out1, FILE* out2, const char* const* names, size_t n, const char* c_name)
{
  fprintf(out2,"grep 'sizeof(\"");
  /* Output *names in upper case. */
  {
    const char* s = *names;
    for (; *s; s++) {
      unsigned char c = * (unsigned char *) s;
      if (c >= 0x80)
        exit(1);
      if (c >= 'a' && c <= 'z')
        c -= 'a'-'A';
      putc(c, out2);
    }
  }
  fprintf(out2,"\")' tmp.h | sed -e 's|^.*\\(stringpool_str[0-9]*\\).*$|  (int)(long)\\&((struct stringpool_t *)0)->\\1,|'\n");
  for (; n > 0; names++, n--)
    emit_alias(out1, *names, c_name);
}
