/*
 * ini_parse.c
 *
 *
 * Parses an INI file into a linked list
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
#include <string.h>
#include <stdlib.h>

#include "ini_parse.h"


/* ini_free - frees the INI linked list
 * 
 * data - IN - the list to be freed
 *
 * returns - void
 */

void ini_free(ini_data_st *data)
{
  ini_section_st *sec;
  ini_section_st *last_sec;
  ini_property_st *prop;
  ini_property_st *last_prop;

  if (data) {
    sec = data->head;
    free(data);
  }

  while (sec) {
    free(sec->name);
    last_sec = sec;
    prop = sec->property;   
    sec = sec->next;
    free(last_sec); 
    
    while(prop) {
      free(prop->name);
      free(prop->value);
      last_prop = prop;
      prop = prop->next;
      free(last_prop);
    }
  }
}


/* ini_init - init the ini data structure by parsing the 
 *            supplied file. data will be malloc'd and 
 *            must be free'd via a call to ini_free.
 *
 * file_name - IN - INI file to be read and parsed
 *
 * returns - ini_section_st - pointer to the head of the 
 *                            parsed and allocated INI structure
 *
 *
 * Expected INI file structure:
 * 
 * Legal lines begin with ';', '[' or are empty lines. 
 * Anything else will cause a parse error. Section heads
 * are enclosed in [] and are followed by name/vaue pairs, 
 * separated by ='s
 *
 * example:
 *
 * [SECTION]
 * name1=value1
 * name2=value2
 * name3=value3
 *
 * [SECTION2]
 * name=value
 * name2=value2
 *
 */

ini_data_st* ini_init(const char *file_name)
{
  FILE *fp;
  char line[256] = {0};
  int line_num = 0;
  int i;
  ini_data_st *ret = NULL;
  ini_section_st *prev = NULL;
  ini_section_st *curr = NULL;
  ini_property_st *curr_p = NULL;
  ini_property_st *prev_p = NULL;
  int parse_section_flag = 0;
  
  if (!file_name) {
    fprintf(stderr, "invalid file name\n");
    return (NULL);
  }
  
  fp = fopen(file_name, "r");
  
  if (!fp) {
    perror("file open failed");
    return (NULL);
  }

  ret = malloc(sizeof(ini_data_st));
  if (!ret) {
    perror("malloc failed");
    fclose(fp);
    return (NULL);
  }
  
  ret->num_sections = 0;
  ret->num_properties = 0;
  
  ret->head = malloc(sizeof(ini_section_st));
  if (!ret->head) {
    perror("malloc failed");
    fclose(fp);
    free(ret);
    return (NULL);
  }
  curr = ret->head;
  curr->name = NULL;
  curr->next = NULL;
  curr->property = NULL;
  
  while (fgets(line, sizeof(line), fp) != NULL) {
    char *c = &line[0];
    ++line_num;

    if (*c != ';' && *c != '[' && *c != '\n') {
      if (parse_section_flag && (isalpha(*c) || isdigit(*c))) {
	char *ptr;

	curr_p = malloc(sizeof(ini_property_st));
	if (!curr_p) {
	  perror("malloc failed");
	  fclose(fp);
	  ini_free(ret);
	  return (NULL);
	}
	
	curr_p->name = NULL;
	curr_p->value = NULL;
	curr_p->next = NULL;
	
	if (prev_p) {
	  prev_p->next = curr_p;
	} else {
	  prev->property = curr_p;
	}

	ptr = strtok(line, "=");
	if (!ptr) {
	  fprintf(stderr, "parse error on line: %d\n", line_num);
	  fclose(fp);
	  ini_free(ret);
	  return (NULL);
	}
	
	curr_p->name = strdup(ptr);
	ptr = strtok(NULL, "\n; ");
	
	if (!ptr) {
	  fprintf(stderr, "parse error on line: %d\n", line_num);
	  fclose(fp);
	  ini_free(ret);
	  return (NULL);
	}

	curr_p->value = strdup(ptr);

	prev_p = curr_p;
	curr_p = NULL;
	
	ret->num_properties++;
      } else {
	fprintf(stderr, "parse error on line: %d\n", line_num);
	fclose(fp);
	ini_free(ret);
	return (NULL);
      }
    } else if (*c == '[') {
      char *ptr;
      parse_section_flag = 0;
      
      if (curr == NULL) {
	curr = malloc(sizeof(ini_section_st));
	if (!curr) {
	  perror("malloc failed");
	  fclose(fp);
	  ini_free(ret);
	  return (NULL);
	}
	prev->next = curr;
	curr->name = NULL;
	curr->property = NULL;
	curr->next = NULL;
      }

      ptr = strtok(&line[1], "]");

      if (!ptr) {
	fprintf(stderr, "parse error on line: %d\n", line_num);
	fclose(fp);
	ini_free(ret);
	return (NULL);
      }

      curr->name = strdup(ptr);
      ret->num_sections++;
      parse_section_flag = 1;

      prev = curr;
      curr = NULL;
      prev_p = NULL;
    }
    memset(line, 0, sizeof(line));
  }

  fclose(fp);
  return (ret);
}

// For test purposes
void ini_print(ini_data_st *data)
{
  ini_section_st *sec = data->head;
  ini_property_st *prop;

  while (sec) {
    printf("[%s]\n", sec->name);
    prop = sec->property;
    
    while(prop) {
      printf("%s=%s\n", prop->name, prop->value);
      prop = prop->next;
    }
    sec = sec->next;
  }
}

