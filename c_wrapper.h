#pragma once

#include <stddef.h>
#include <stdbool.h>

// Forward declarations of C wrapper structs
typedef struct {
    char* name; // The name of the enumeration.
    char** enumerators; // An array of enumerator names.
    long long* values; // An array of associated values for enumerators.
    size_t enumerator_count; // Count of enumerators.
    char* underlying_type; // The underlying type of the enumeration.
    char* comment; // An optional comment describing the enumeration.
} c_enum_info;

typedef struct {
    char* name; // The name of the structure member.
    char* type; // The type of the structure member.
    int bitfield_width; // The width of the bitfield, if applicable.
} c_struct_member;

typedef struct {
    char* name; // The name of the structure.
    c_struct_member* members; // An array of members belonging to the structure.
    size_t member_count; // Count of members.
    char* comment; // An optional comment describing the structure.
} c_struct_info;

typedef struct {
    char* name; // The name of the function.
    char* return_type; // The return type of the function.
    char** parameters; // An array of parameter names.
    char** param_types; // An array of parameter types.
    size_t param_count; // Count of parameters.
    char* attributes; // Any attributes associated with the function.
    bool is_variadic; // Indicates whether the function is variadic.
    char* comment; // An optional comment describing the function.
} c_function_info;

typedef struct {
    char* name; // The name of the variable.
    char* type; // The type of the variable.
    char* value; // The value of the variable, if applicable.
    char* storage_class; // The storage class of the variable (e.g., static, extern).
    char* qualifiers; // Any qualifiers associated with the variable (e.g., const, volatile).
    int* array_dimensions; // An array representing the dimensions of the array.
    size_t array_dimension_count; // Count of array dimensions.
    char* comment; // An optional comment describing the variable.
} c_variable_info;

typedef struct {
    char* new_name; // The new name for the typedef.
    char* original_type; // The original type that the typedef refers to.
    char* qualifiers; // Any qualifiers associated with the typedef.
    char* comment; // An optional comment describing the typedef.
} c_typedef_info;

typedef struct {
    void* header_analyzer; // Pointer to the C++ HeaderAnalyzer instance.
} c_header_analyzer;

// Function declarations for the C wrapper

c_header_analyzer* c_header_analyzer_create(const char* filename);
void c_header_analyzer_destroy(c_header_analyzer* analyzer);

const c_enum_info* c_header_analyzer_get_enums(c_header_analyzer* analyzer, size_t* count);
void c_enum_info_destroy(c_enum_info* enums, size_t count);

const c_struct_info* c_header_analyzer_get_structs(c_header_analyzer* analyzer, size_t* count);
void c_struct_info_destroy(c_struct_info* structs, size_t count);

const c_function_info* c_header_analyzer_get_functions(c_header_analyzer* analyzer, size_t* count);
void c_function_info_destroy(c_function_info* functions, size_t count);

const c_variable_info* c_header_analyzer_get_variables(c_header_analyzer* analyzer, size_t* count);
void c_variable_info_destroy(c_variable_info* variables, size_t count);

const c_typedef_info* c_header_analyzer_get_typedefs(c_header_analyzer* analyzer, size_t* count);
void c_typedef_info_destroy(c_typedef_info* typedefs, size_t count);

void c_header_analyzer_write_to_xml(c_header_analyzer* analyzer, const char* output_filename);
