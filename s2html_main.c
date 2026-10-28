/*************************************************************************************************************************************************************************************
 * @author  SATHEESWARAN M
 * 
 * @name    Source to HTML
 * 
 * @breif   Source-to-HTML Converter (S2HTML) is a C programming project that converts a .c source file into an .html file with syntax highlighting.
            The project uses a state machine and event-driven approach to read the C source code character by character and identify elements such as 
            keywords, identifiers, numeric constants, strings, preprocessor directives, operators, and comments.
            These elements are converted into HTML tags with CSS styles to display them in different colors in a web browser.
            Files used:
            - s2html_main.c – Main program and file handling.
            - s2html_event.c – Parses source code using states and generates events.
            - s2html_event.h – Defines parser states, events, and structures.
            - s2html_conv.c – Converts events into HTML with syntax highlighting.
            - s2html_conv.h – Declares HTML conversion functions.
 *  
*************************************************************************************************************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "s2html_event.h"
#include "s2html_conv.h"


/*
 * Main function
 *
 * Usage:
 *
 *     ./s2html <source.c>
 *
 * Example:
 *
 *     ./s2html 123.c
 *
 *
 * Optional output name:
 *
 *     ./s2html 123.c output.html
 */
int main(int argc, char *argv[])
{
    FILE *sfp;
    FILE *dfp;

    char dest_file[256];

    pevent_t *event;


    /*
     * Check command-line arguments.
     */
    if (argc < 2)
    {
        printf("\nError: Please provide a C source file.\n");
        printf("Usage: %s <source.c> [output.html]\n", argv[0]);
        printf("Example: %s 123.c\n\n", argv[0]);

        return 1;
    }


    /*
     * Open source file in read mode.
     */
    sfp = fopen(argv[1], "r");

    if (sfp == NULL)
    {
        printf("Error: File %s could not be opened.\n", argv[1]);

        return 2;
    }



    if (argc >= 3)
    {
        snprintf(dest_file, sizeof(dest_file), "%s", argv[2]);
    }
    else
    {
        snprintf(dest_file, sizeof(dest_file), "%s.html", argv[1]);
    }


    /*
     * Open destination file.
     */
    dfp = fopen(dest_file, "w");

    if (dfp == NULL)
    {
        printf("Error: Output file %s could not be created.\n",
               dest_file);

        fclose(sfp);

        return 3;
    }


    /*
     * Initialize parser state machine.
     */
    parser_init();


    /*
     * Write HTML starting tags and CSS.
     */
    html_begin(dfp, argv[1]);


    /*
     * Parse the source file event by event.
     */
    while ((event = get_parser_event(sfp)) != NULL)
    {
        html_write_event(dfp, event);
    }



    //Write HTML closing tags.
    html_end(dfp);


    
    fclose(sfp);
    fclose(dfp);


 
    printf("Output file %s generated\n", dest_file);

    return 0;
}