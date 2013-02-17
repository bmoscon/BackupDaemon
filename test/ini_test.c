/*
 * ini_test.c
 *
 *
 * test INI parsing
 *
 *
 * Copyright (C) 2013  Bryant Moscon - bmoscon@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution, and in the same 
 *    place and form as other copyright,
 *    license and disclaimer information.
 *
 * 3. The end-user documentation included with the redistribution, if any, must 
 *    include the following acknowledgment: "This product includes software 
 *    developed by Bryant Moscon (http://www.bryantmoscon.org/)", in the same 
 *    place and form as other third-party acknowledgments. Alternately, this 
 *    acknowledgment may appear in the software itself, in the same form and 
 *    location as other such third-party acknowledgments.
 *
 * 4. Except as contained in this notice, the name of the author, Bryant Moscon,
 *    shall not be used in advertising or otherwise to promote the sale, use or 
 *    other dealings in this Software without prior written authorization from 
 *    the author.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "../src/ini_parse.h"


int main(int argc, char *argv[])
{
  ini_data_st *data;
  char *sec = "section6";
  char *prop = "p1";
  ini_pair pair;
  
  if (argc != 2) {
    fprintf(stderr, "usage: ini_test <INI filename>\n");
    exit(1);
  }

  
  data = ini_init(argv[1]);
  
  if (data) {
    ini_print(data);

    printf("\n\nSearching for section %s, property %s: %s\n", sec, prop, 
	                                                        ini_get_data(data, sec, prop));
    printf("\nSearching for global property %s: %s\n\n", prop, 
	                                                   ini_get_data(data, NULL, prop));

    printf("testing iterator, printing list of all properties in \"section2\":\n");
    
    pair = ini_iter_init(data, "section2");
    printf("%s = %s\n", pair.n, pair.v);
    
    pair = ini_iter_next(data);
    while (pair.n) {
      printf("%s = %s\n", pair.n, pair.v);
      pair = ini_iter_next(data);
    }
      
    ini_free(data);
  }

  return (0);
}
