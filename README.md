# Source to HTML Converter (S2HTML)

A modular C programming project that converts C source files into HTML files with syntax highlighting. It uses a state-machine-based, event-driven parser to identify C language elements and applies CSS styles so they appear in different colors in a web browser.

## Features

- Reads a C source file provided through the command line.
- Identifies reserved keywords, identifiers, numeric constants, string literals, character constants, operators, comments, and preprocessor directives.
- Uses a state-machine-based parser to process source code character by character and generate parser events.
- Converts parser events into HTML elements with CSS-based syntax highlighting.
- Preserves whitespace, newlines, braces, parentheses, and other regular characters in the generated output.
- Escapes special HTML characters such as `<`, `>`, `&`, and `"` to display source code safely as text.
- Supports a custom output filename; otherwise, generates an output filename by appending `.html` to the input filename.
- Uses separate source and header files for modular design.

## Technologies Used

- **Programming Language:** C
- **Standard Libraries:** `stdio.h`, `string.h`, `ctype.h`
- **Output:** HTML and embedded CSS
- **Compiler:** GCC or Clang

## Project Structure

| File | Responsibility |
|---|---|
| `s2html_main.c` | Entry point. Validates command-line arguments, opens the input and output files, initializes the parser, processes parser events, and closes the files. |
| `s2html_event.c` | Implements the state-machine-based parser. Reads source code character by character and generates events for supported C language elements. |
| `s2html_event.h` | Defines parser states, parser event types, the event structure, and parser function declarations. |
| `s2html_conv.c` | Converts parser events into HTML, embeds CSS styles, applies syntax highlighting, and escapes special HTML characters. |
| `s2html_conv.h` | Declares the HTML generation and event conversion functions. |

## How It Works

1. **Input:** The program receives a C source filename as a command-line argument.
2. **File Handling:** It opens the source file and creates an HTML output file. An optional second argument can specify the output filename.
3. **Parsing:** The parser reads the source file character by character and identifies supported source elements.
4. **Event Generation:** Each recognized element is returned as a parser event with an event type and its corresponding text.
5. **HTML Conversion:** The converter wraps supported elements in HTML tags and applies embedded CSS classes for syntax highlighting.
6. **Output:** The generated HTML file can be opened in a web browser to view the C source code with color-coded syntax.

## Build and Run

Compile the source files using GCC:

```bash
gcc s2html_main.c s2html_event.c s2html_conv.c -o s2html
```

Run the converter by providing a C source file:

```bash
./s2html sample.c
```

To specify an output filename:

```bash
./s2html sample.c output.html
```

On Windows, run the compiled executable from Command Prompt or PowerShell, for example:

```powershell
s2html.exe sample.c output.html
```

## Syntax Highlighting

The converter applies different colors to supported source elements:

| Element | CSS Class | Display Color |
|---|---|---|
| Reserved keywords | `keyword` | Green |
| Identifiers | `identifier` | Black |
| Preprocessor directives | `preprocessor` | Purple |
| Comments | `comment` | Blue |
| String literals | `string` | Magenta |
| Character constants | `char` | Firebrick |
| Numeric constants | `number` | Brown |
| Operators | `operator` | Black |

Regular characters such as spaces, newlines, braces, parentheses, and semicolons are written without syntax-highlighting wrappers to preserve the source layout.

## Example

Given an input file named `sample.c`:

```c
#include <stdio.h>

int main(void)
{
    int count = 10;
    printf("Hello");
    return 0;
}
```

Run:

```bash
./s2html sample.c
```

By default, the program generates `sample.c.html`. Open that file in a web browser to view the source with syntax highlighting.

## Skills Demonstrated

- C programming and modular program design
- File handling using standard I/O
- State-machine-based parsing
- Event-driven processing
- Character-by-character scanning
- String processing and character classification
- HTML generation and CSS styling
- Escaping special HTML characters
- Command-line argument handling
- Header files and function interfaces

## Scope and Limitations

This project is a basic source-to-HTML converter for learning and demonstration. It highlights the C language elements explicitly supported by its parser; it is not a complete C compiler or parser. It does not perform syntax analysis or validate the full C grammar. The parser recognizes selected two-character operators, and its event buffer has a fixed maximum size. Preprocessor directives are handled as lines beginning with `#`.

## Author

**Satheeswaran M**

- **Email:** [satheeswaran.vnr@gmail.com](mailto:satheeswaran.vnr@gmail.com)
- **GitHub:** [sathees-waran](https://github.com/sathees-waran)
- **LinkedIn:** [sathees-waran](https://www.linkedin.com/in/sathees-waran/)
