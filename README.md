# HeaderAnalyzer

HeaderAnalyzer is a powerful tool designed for processing C and C++ header files. It extracts essential information about enums, structs, functions, variables, and typedefs, allowing developers to analyze and document their code more effectively.

## Installation

To compile the HeaderAnalyzer program, use the following command:

```bash
# Compile the program
g++ -o HeaderAnalyzer Main.cpp HeaderAnalyzer.cpp -I/usr/lib/llvm-16/include -L/usr/lib/llvm-16/lib -lclang
```

Ensure that you have the necessary LLVM and Clang libraries installed on your system.

## Usage

Once compiled, you can use the HeaderAnalyzer to process a C header file and generate an XML output. Here’s how to do it:

```bash
# Process the header and write its output to XML
./HeaderAnalyzer example_header.h output.xml
```

Replace `example_header.h` with the path to your header file, and `output.xml` will be the name of the generated XML file.

## Output Format

The output XML file contains structured information about the analyzed header file. Here’s an example snippet of what the output might look like:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<header>
  <enums>
    <enum name="Genre" underlying-type="unsigned int">
      <enumerators>
        <enumerator name="FICTION" value="0"/>
        <enumerator name="NON_FICTION" value="1"/>
        <enumerator name="MYSTERY" value="2"/>
        <enumerator name="FANTASY" value="3"/>
        <enumerator name="SCIENCE_FICTION" value="4"/>
        <enumerator name="BIOGRAPHY" value="5"/>
        <enumerator name="HISTORY" value="6"/>
        <enumerator name="ROMANCE" value="7"/>
        <enumerator name="OTHER" value="8"/>
      </enumerators>
    </enum>
  </enums>
  <typedefs>
    <typedef new-name="Genre" original-type="Genre">
      <!-- More omitted -->
    </typedef>
  </typedefs>
</header>
```

## Class Overview

The `HeaderAnalyzer` class is the core component of this tool, designed to analyze C/C++ header files. Below is a brief overview of its key structures and methods:

### Structures

- **EnumInfo**: Represents information about an enumeration type, including its name, enumerators, underlying type, and an optional comment.
  
- **StructMember**: Represents a member of a structure, including its name, type, and bitfield width.

- **StructInfo**: Represents information about a structure type, including its name, members, and an optional comment.

- **FunctionInfo**: Represents information about a function, including its name, return type, parameters, attributes, variadic status, and an optional comment.

- **VariableInfo**: Represents information about a variable, including its name, type, value, storage class, qualifiers, array dimensions, and an optional comment.

- **TypedefInfo**: Represents information about a typedef, including the new name, original type, qualifiers, and an optional comment.

### Methods

- **HeaderAnalyzer(const std::string& filename)**: Constructs a HeaderAnalyzer for the specified header file.

- **~HeaderAnalyzer()**: Destructor for the HeaderAnalyzer.

- **getEnums()**: Retrieves a list of enumerations found in the analyzed header file.

- **getStructs()**: Retrieves a list of structures found in the analyzed header file.

- **getFunctions()**: Retrieves a list of functions found in the analyzed header file.

- **getVariables()**: Retrieves a list of variables found in the analyzed header file.

- **getTypedefs()**: Retrieves a list of typedefs found in the analyzed header file.

- **writeToXML(const std::string& outputFilename)**: Writes the analyzed information to an XML file.

## Author, License

Copyright :copyright: 2024 by Alan Tseng

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

