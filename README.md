# What is MathText?

MathText aims to serve as a convenient means for blind and partially 
sighted users to write and communicate technical and mathematical material.
This tool converts a specialized, text-based representation of such material
into alternative formats, including:

- LaTeX (for printing as a conventional document);
- Unified English Braille; and,
- Nemeth Code

The syntax of the language is designed in part to allow for a custom 
dictionary to be used with a screen reader (such as JAWS), permitting the
mathematical material to be read in a manner that is easily understood 
by a user listening to the information.

# Project dependencies

Compilation of MathText requires:

- [BOOST](http://www.boost.org) (tested with 1.58.0)
- [liblouis](http://www.liblouis.org) (tested with 2.6.3)
- [Qt](http://www.qt.io) (tested with 5.5) _(for the GUI only, tested on Windows)_

# Compilation Notes

On Linux, Automake/Autoconf scripts are used to facilitate building. 

In Visual Studio, a custom property sheet has been setup with user macros
for BOOST_PATH (point to root directory) and LOUIS_PATH (point to root 
directory). The VS projects assume Boost 1.58 for the libraries. Note 
that as of September 2015, building with Boost 1.59 was not possible 
due to some bug in the log-related templates.


# Why use MathText?

A variety of methods have been used by the blind and visually impaired 
over the years in order to write and communicate mathematical material. 
For primary, secondary, high school, and college/university students in 
particular, it is not uncommon for students to write their maths in 
braille and then rely on a sighted volunteer or staff member to back-
transcribe the information into regular print. In the worst case scenario, 
some studnets wind up dictating to a scribe, and are therefore completely 
unable to look at or review their work as they proceed. Performing complex 
calculations this is a great mental feat, but one that many cannot 
adequately master. 

Some automated computer-based transcription tools exist to convert 
electronic braille files into LaTeX documents for printing as a 
conventional document. However, these tools are highly sensitive to 
mistakes or errors in the braille code, and the mixing of text and math 
material together presents significant conversion challenges. 

MathText not only permits a user (equipped only with a computer and 
screen reader) to read, write, and review their mathematical material, but 
when the time comes to convert that into print or braille, the tool can 
provide feedback to the user about possible syntax problems or errors that
might otherwise result in an unreadable or confused document. 

# State of the project

This is a complete, ground-up rewrite of the code that I initially hacked 
together circa 2001. The core translation engine is intended to be 
fully independent of the user interface, and both command-line, Linux 
clients as well as a Windows-based client are envisioned. 

At present, the command-line client and command-line test suites are 
functional and operational. They can produce Braille or LaTeX output 
accordingly. 

Development of a Windows client has been started (based on Qt for potential 
portability to other platforms) but is not yet part of the core project 
yet.

From a functionality standpoint, most of the core language constructs 
have been implemented, including:

- math/text parsing
- numbers
- common symbols, operators, and comparators
- Greek alphabet recognition
- fractions
- roots (square roots and otherwise)
- subscripts and exponents
- various 'character modifiers' (vectors, bars-over, etc.)
- sigma/summation notation

The following functionality (which existed in the original code) has not 
yet been integrated here:

- matrices
- binomials
- column vectors
- integrals


