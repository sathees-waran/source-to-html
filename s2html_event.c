#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "s2html_event.h"


/*
 * Current parser state.
 */
static pstate_e state = PSTATE_IDLE;


/*
 * Event object used to return parsed data.
 */
static pevent_t event_data;


/*
 * Append one character to event buffer.
 */
static void append_char(char ch)
{
    int len = strlen(event_data.data);

    if (len < EVENT_DATA_SIZE - 1)
    {
        event_data.data[len] = ch;
        event_data.data[len + 1] = '\0';
    }
}


/*
 * Reset event buffer and assign event type.
 */
static void set_event(pevent_e type)
{
    event_data.type = type;
    event_data.data[0] = '\0';
}


/*
 * Initialize parser.
 */
void parser_init(void)
{
    state = PSTATE_IDLE;

    event_data.type = PEVENT_NONE;
    event_data.data[0] = '\0';
}


/*
 * Read an identifier / keyword.
 *
 * Example:
 *
 *     int
 *     main
 *     counter
 */
static pevent_t *parse_identifier(FILE *fd, int first_char)
{
    static const char *keywords[] =
    {
        "auto", "break", "case", "char",
        "const", "continue", "default", "do",
        "double", "else", "enum", "extern",
        "float", "for", "goto", "if",
        "int", "long", "register", "return",
        "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union",
        "unsigned", "void", "volatile", "while",
        NULL
    };

    char word[EVENT_DATA_SIZE];
    int i = 0;
    int ch;
    int is_keyword = 0;

    word[i++] = first_char;

    while ((ch = fgetc(fd)) != EOF)
    {
        if (isalnum(ch) || ch == '_')
        {
            if (i < EVENT_DATA_SIZE - 1)
                word[i++] = ch;
        }
        else
        {

            ungetc(ch, fd);
            break;
        }
    }

    word[i] = '\0';

    /*
     * Check whether the word is a C reserved keyword.
     */
    for (i = 0; keywords[i] != NULL; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            is_keyword = 1;
            break;
        }
    }

    if (is_keyword)
        set_event(PEVENT_RESERVED_KEYWORD);
    else
        set_event(PEVENT_IDENTIFIER);

    strcpy(event_data.data, word);

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Parse numeric constant.
 *
 * Examples:
 *
 *     10
 *     100
 *     3.14
 */
static pevent_t *parse_number(FILE *fd, int first_char)
{
    int ch;

    set_event(PEVENT_NUMERIC_CONSTANT);

    append_char(first_char);

    while ((ch = fgetc(fd)) != EOF)
    {
        if (isdigit(ch) || ch == '.')
        {
            append_char(ch);
        }
        else
        {
            ungetc(ch, fd);
            break;
        }
    }

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Parse string literal.
 */
static pevent_t *parse_string(FILE *fd, int first_char)
{
    int ch;
    int escape = 0;

    set_event(PEVENT_STRING);

    append_char(first_char);

    while ((ch = fgetc(fd)) != EOF)
    {
        append_char(ch);

        /*
         * Handle escaped characters such as:
         *
         *     \" 
         *     \n
         *     \\
         */
        if (escape)
        {
            escape = 0;
            continue;
        }

        if (ch == '\\')
        {
            escape = 1;
        }
        else if (ch == '"')
        {
            break;
        }
    }

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Parse character constant.
 *
 * Example:
 *
 *     'A'
 *     '\n'
 */
static pevent_t *parse_ascii_char(FILE *fd, int first_char)
{
    int ch;
    int escape = 0;

    set_event(PEVENT_ASCII_CHAR);

    append_char(first_char);

    while ((ch = fgetc(fd)) != EOF)
    {
        append_char(ch);

        if (escape)
        {
            escape = 0;
            continue;
        }

        if (ch == '\\')
        {
            escape = 1;
        }
        else if (ch == '\'')
        {
            break;
        }
    }

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Parse single-line comment.
 *
 * Example:
 *
 *     // this is a comment
 */
static pevent_t *parse_single_line_comment(FILE *fd)
{
    int ch;

    set_event(PEVENT_SINGLE_LINE_COMMENT);

    append_char('/');
    append_char('/');

    while ((ch = fgetc(fd)) != EOF)
    {
        append_char(ch);

        if (ch == '\n')
            break;
    }

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Parse multi-line comment.
 *
 * Example:
 *
 *     /*
 *         comment
 *     *\/
 */
static pevent_t *parse_multi_line_comment(FILE *fd)
{
    int ch;
    int previous = 0;

    set_event(PEVENT_MULTI_LINE_COMMENT);

    append_char('/');
    append_char('*');

    while ((ch = fgetc(fd)) != EOF)
    {
        append_char(ch);

        if (previous == '*' && ch == '/')
            break;

        previous = ch;
    }

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Parse preprocessor directive.
 *
 * Example:
 *
 *     #include <stdio.h>
 *     #define SIZE 10
 */
static pevent_t *parse_preprocessor(FILE *fd)
{
    int ch;

    set_event(PEVENT_PREPROCESSOR);

    append_char('#');

    while ((ch = fgetc(fd)) != EOF)
    {
        append_char(ch);

        if (ch == '\n')
            break;
    }

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Parse operators.
 *
 * Supports both one-character and two-character
 * C operators.
 */
static pevent_t *parse_operator(FILE *fd, int first_char)
{
    int ch;

    set_event(PEVENT_OPERATOR);

    append_char(first_char);

    ch = fgetc(fd);

    if (ch != EOF)
    {
        /*
         * Two-character operators.
         */
        if ((first_char == '=' && ch == '=') ||
            (first_char == '!' && ch == '=') ||
            (first_char == '<' && ch == '=') ||
            (first_char == '>' && ch == '=') ||
            (first_char == '+' && ch == '+') ||
            (first_char == '-' && ch == '-') ||
            (first_char == '+' && ch == '=') ||
            (first_char == '-' && ch == '=') ||
            (first_char == '*' && ch == '=') ||
            (first_char == '/' && ch == '=') ||
            (first_char == '%' && ch == '=') ||
            (first_char == '&' && ch == '&') ||
            (first_char == '|' && ch == '|'))
        {
            append_char(ch);
        }
        else
        {
            ungetc(ch, fd);
        }
    }

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Parse regular characters.
 *
 * These include:
 *
 *     spaces
 *     tabs
 *     newline
 *     brackets
 *     semicolon
 *     comma
 *     braces
 */
static pevent_t *parse_regular(FILE *fd, int first_char)
{
    (void)fd;

    set_event(PEVENT_REGULAR_EXP);

    append_char(first_char);

    state = PSTATE_IDLE;

    return &event_data;
}


/*
 * Main parser state machine.
 *
 * One character is read at a time.
 *
 * Based on that character, the parser identifies
 * which state/event should be generated.
 */
pevent_t *get_parser_event(FILE *fd)
{
    int ch;
    int next;

    if (fd == NULL)
        return NULL;

    while ((ch = fgetc(fd)) != EOF)
    {
        state = PSTATE_IDLE;

        /*
         * ----------------------------------------
         * Preprocessor
         * ----------------------------------------
         */
        if (ch == '#')
        {
            state = PSTATE_PREPROCESSOR;
            return parse_preprocessor(fd);
        }


        /*
         * ----------------------------------------
         * Identifier / keyword
         * ----------------------------------------
         */
        if (isalpha(ch) || ch == '_')
        {
            state = PSTATE_IDENTIFIER;
            return parse_identifier(fd, ch);
        }


        /*
         * ----------------------------------------
         * Numeric constant
         * ----------------------------------------
         */
        if (isdigit(ch))
        {
            state = PSTATE_NUMERIC_CONSTANT;
            return parse_number(fd, ch);
        }


        /*
         * ----------------------------------------
         * String literal
         * ----------------------------------------
         */
        if (ch == '"')
        {
            state = PSTATE_STRING;
            return parse_string(fd, ch);
        }


        /*
         * ----------------------------------------
         * ASCII / character literal
         * ----------------------------------------
         */
        if (ch == '\'')
        {
            state = PSTATE_ASCII_CHAR;
            return parse_ascii_char(fd, ch);
        }


        /*
         * ----------------------------------------
         * Comment / division operator
         * ----------------------------------------
         */
        if (ch == '/')
        {
            next = fgetc(fd);

            /*
             * Single-line comment
             */
            if (next == '/')
            {
                state = PSTATE_SINGLE_LINE_COMMENT;
                return parse_single_line_comment(fd);
            }

            /*
             * Multi-line comment
             */
            if (next == '*')
            {
                state = PSTATE_MULTI_LINE_COMMENT;
                return parse_multi_line_comment(fd);
            }

            /*
             * It was only '/'.
             *
             * Put the second character back because
             * it belongs to the next event.
             */
            if (next != EOF)
                ungetc(next, fd);

            state = PSTATE_OPERATOR;
            return parse_operator(fd, ch);
        }


        /*
         * ----------------------------------------
         * Operators
         * ----------------------------------------
         */
        if (ch == '=' || ch == '!' ||
            ch == '<' || ch == '>' ||
            ch == '+' || ch == '-' ||
            ch == '*' || ch == '%' ||
            ch == '&' || ch == '|')
        {
            state = PSTATE_OPERATOR;
            return parse_operator(fd, ch);
        }


        /*
         * ----------------------------------------
         * Anything else
         * ----------------------------------------
         *
         * Spaces, tabs, newline, brackets,
         * braces, comma, semicolon etc.
         */
        return parse_regular(fd, ch);
    }

    return NULL;
}


/*
 * Return current parser state.
 */
pstate_e get_parser_state(void)
{
    return state;
}