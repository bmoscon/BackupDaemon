/*
 * ini_parse.h
 *
 * Parses an INI file into a liniked list
 * 
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


#ifndef __INI_PARSE__
#define __INI_PARSE__


typedef struct ini_property_st {
  char* name;
  char* value;
  
  struct ini_property_st *next;
} ini_property_st;


typedef struct ini_section_st {
  char* name;
  ini_property_st *property;
  
  struct ini_section_st *next;
} ini_section_st;


typedef struct ini_data_st {
  int num_sections;
  int num_properties;

  ini_property_st *iter;
  
  ini_section_st *head;
  ini_property_st *global;
  
} ini_data_st;


typedef struct ini_pair {
  char *n;
  char *v;
} ini_pair;


void ini_free(ini_data_st *data);
ini_data_st* ini_init(const char *file_name); 
void ini_print(ini_data_st *data);

char *ini_get_data(ini_data_st *data, char *sec, char *prop);
ini_pair ini_iter_init(ini_data_st *data, char *sec);
ini_pair ini_iter_next(ini_data_st *data);
#endif
