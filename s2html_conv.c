#include <stdio.h>
#include <string.h>

#include "s2html_event.h"
#include "s2html_conv.h"


/*
 * Escape characters which have special meaning in HTML.
 *
 * For example:
 *
 *     <
 *     >
 *     &
 *
 * must not be written directly into HTML source
 * when they are part of the C program.
 */
static void html_escape(FILE *fp, const char *str)
{
    while (*str)
    {
        switch (*str)
        {
            case '<':
                fprintf(fp, "&lt;");
                break;

            case '>':
                fprintf(fp, "&gt;");
                break;

            case '&':
                fprintf(fp, "&amp;");
                break;

            case '"':
                fprintf(fp, "&quot;");
                break;

            default:
                fputc(*str, fp);
                break;
        }

        str++;
    }
}


/*
 * Write the HTML header.
 *
 * CSS is embedded inside the HTML file so that the
 * generated HTML works independently without needing
 * a separate styles.css file.
 */
void html_begin(FILE *dest_fp, const char *title)
{
    fprintf(dest_fp,
            "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<title>%s</title>\n"
            "<style>\n"
            "body {\n"
            "    background-color: lightgrey;\n"
            "    margin: 20px;\n"
            "}\n"
            "pre {\n"
            "    font-family: monospace;\n"
            "    font-size: 16px;\n"
            "}\n"
            ".keyword {\n"
            "    color: green;\n"
            "}\n"
            ".identifier {\n"
            "    color: black;\n"
            "}\n"
            ".comment {\n"
            "    color: blue;\n"
            "}\n"
            ".preprocessor {\n"
            "    color: purple;\n"
            "}\n"
            ".string {\n"
            "    color: magenta;\n"
            "}\n"
            ".number {\n"
            "    color: brown;\n"
            "}\n"
            ".char {\n"
            "    color: firebrick;\n"
            "}\n"
            ".operator {\n"
            "    color: black;\n"
            "}\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<pre>\n",
            title);
}


//Write the HTML footer.
void html_end(FILE *dest_fp)
{
    fprintf(dest_fp,
            "</pre>\n"
            "</body>\n"
            "</html>\n");
}



 // Write one parser event as HTML.

void html_write_event(FILE *dest_fp, const pevent_t *event)
{
    if (dest_fp == NULL || event == NULL)
        return;

    switch (event->type)
    {
        case PEVENT_RESERVED_KEYWORD:

            fprintf(dest_fp, "<span class=\"keyword\">");
            html_escape(dest_fp, event->data);
            fprintf(dest_fp, "</span>");

            break;


        case PEVENT_IDENTIFIER:

            fprintf(dest_fp, "<span class=\"identifier\">");
            html_escape(dest_fp, event->data);
            fprintf(dest_fp, "</span>");

            break;


        case PEVENT_PREPROCESSOR:

            fprintf(dest_fp, "<span class=\"preprocessor\">");
            html_escape(dest_fp, event->data);
            fprintf(dest_fp, "</span>");

            break;


        case PEVENT_SINGLE_LINE_COMMENT:
        case PEVENT_MULTI_LINE_COMMENT:

            fprintf(dest_fp, "<span class=\"comment\">");
            html_escape(dest_fp, event->data);
            fprintf(dest_fp, "</span>");

            break;


        case PEVENT_STRING:

            fprintf(dest_fp, "<span class=\"string\">");
            html_escape(dest_fp, event->data);
            fprintf(dest_fp, "</span>");

            break;


        case PEVENT_ASCII_CHAR:

            fprintf(dest_fp, "<span class=\"char\">");
            html_escape(dest_fp, event->data);
            fprintf(dest_fp, "</span>");

            break;


        case PEVENT_NUMERIC_CONSTANT:

            fprintf(dest_fp, "<span class=\"number\">");
            html_escape(dest_fp, event->data);
            fprintf(dest_fp, "</span>");

            break;


        case PEVENT_OPERATOR:

            fprintf(dest_fp, "<span class=\"operator\">");
            html_escape(dest_fp, event->data);
            fprintf(dest_fp, "</span>");

            break;


        case PEVENT_REGULAR_EXP:

            /*
             * Normal characters are written directly.
             *
             * This is important for:
             *
             *     spaces
             *     newline
             *     braces
             *     semicolon
             *     parentheses
             */
            html_escape(dest_fp, event->data);

            break;


        default:
            break;
    }
}