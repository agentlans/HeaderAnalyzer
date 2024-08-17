#include <stdio.h>
#include "c_wrapper.h" // Include the header file with the C wrapper declarations.

// Compile using
// g++ c_wrapper_example.c c_wrapper.cpp HeaderAnalyzer.cpp  -I/usr/lib/llvm-16/include -L/usr/lib/llvm-16/lib -lclang

int main() {
    const char* filename = "example_header.h"; // Path to the header file to analyze.
    
    // Create an instance of the C wrapper for HeaderAnalyzer.
    c_header_analyzer* analyzer = c_header_analyzer_create(filename);
    if (!analyzer) {
        fprintf(stderr, "Failed to create HeaderAnalyzer instance.\n");
        return 1;
    }

    // Retrieve and print enumerations.
    size_t enum_count;
    const c_enum_info* enums = c_header_analyzer_get_enums(analyzer, &enum_count);
    printf("Enumerations:\n");
    for (size_t i = 0; i < enum_count; ++i) {
        printf("  Name: %s\n", enums[i].name);
        printf("  Underlying Type: %s\n", enums[i].underlying_type);
        printf("  Comment: %s\n", enums[i].comment);
        for (size_t j = 0; j < enums[i].enumerator_count; ++j) {
            printf("    Enumerator: %s = %lld\n", enums[i].enumerators[j], enums[i].values[j]);
        }
    }
    c_enum_info_destroy((c_enum_info*)enums, enum_count); // Clean up enumerations.

    // Retrieve and print structures.
    size_t struct_count;
    const c_struct_info* structs = c_header_analyzer_get_structs(analyzer, &struct_count);
    printf("\nStructures:\n");
    for (size_t i = 0; i < struct_count; ++i) {
        printf("  Name: %s\n", structs[i].name);
        printf("  Comment: %s\n", structs[i].comment);
        for (size_t j = 0; j < structs[i].member_count; ++j) {
            printf("    Member: %s (%s), Bitfield Width: %d\n", structs[i].members[j].name,
                   structs[i].members[j].type, structs[i].members[j].bitfield_width);
        }
    }
    c_struct_info_destroy((c_struct_info*)structs, struct_count); // Clean up structures.

    // Retrieve and print functions.
    size_t function_count;
    const c_function_info* functions = c_header_analyzer_get_functions(analyzer, &function_count);
    printf("\nFunctions:\n");
    for (size_t i = 0; i < function_count; ++i) {
        printf("  Name: %s\n", functions[i].name);
        printf("  Return Type: %s\n", functions[i].return_type);
        printf("  Is Variadic: %s\n", functions[i].is_variadic ? "Yes" : "No");
        printf("  Comment: %s\n", functions[i].comment);
        for (size_t j = 0; j < functions[i].param_count; ++j) {
            printf("    Parameter: %s (%s)\n", functions[i].parameters[j], functions[i].param_types[j]);
        }
    }
    c_function_info_destroy((c_function_info*)functions, function_count); // Clean up functions.

    // Retrieve and print variables.
    size_t variable_count;
    const c_variable_info* variables = c_header_analyzer_get_variables(analyzer, &variable_count);
    printf("\nVariables:\n");
    for (size_t i = 0; i < variable_count; ++i) {
        printf("  Name: %s\n", variables[i].name);
        printf("  Type: %s\n", variables[i].type);
        printf("  Value: %s\n", variables[i].value);
        printf("  Storage Class: %s\n", variables[i].storage_class);
        printf("  Qualifiers: %s\n", variables[i].qualifiers);
    }
    c_variable_info_destroy((c_variable_info*)variables, variable_count); // Clean up variables.

    // Retrieve and print typedefs.
    size_t typedef_count;
    const c_typedef_info* typedefs = c_header_analyzer_get_typedefs(analyzer, &typedef_count);
    printf("\nTypedefs:\n");
    for (size_t i = 0; i < typedef_count; ++i) {
        printf("  New Name: %s\n", typedefs[i].new_name);
        printf("  Original Type: %s\n", typedefs[i].original_type);
        printf("  Qualifiers: %s\n", typedefs[i].qualifiers);
    }
    c_typedef_info_destroy((c_typedef_info*)typedefs, typedef_count); // Clean up typedefs.

    // Write analyzed information to XML.
    const char* output_filename = "output.xml";
    c_header_analyzer_write_to_xml(analyzer, output_filename);
    printf("\nAnalysis written to %s.\n", output_filename);

    // Clean up the HeaderAnalyzer instance.
    c_header_analyzer_destroy(analyzer);

    return 0;
}
