#ifndef S2HTML_CONV_H
#define S2HTML_CONV_H

#include <stdio.h>

#include "s2html_event.h"


/*
 * Write HTML beginning tags.
 */
void html_begin(FILE *dest_fp, const char *title);


/*
 * Write HTML ending tags.
 */
void html_end(FILE *dest_fp);


/*
 * Convert one parser event into HTML.
 */
void html_write_event(FILE *dest_fp, const pevent_t *event);

#endif