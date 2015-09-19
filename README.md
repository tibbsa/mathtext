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

On Linux, Automake/Autoconf scripts are used to facilitate building. 

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

At present, the Linux command-line client is functional. A starting point 
for the Windows client has been developed but significantly more effort is 
required there.

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














