#include <stdlib.h>
#include <string.h>

// Assume the C++ HeaderAnalyzer and its dependencies are properly included and linked.
#include "HeaderAnalyzer.h"
#include "c_wrapper.h"

c_header_analyzer* c_header_analyzer_create(const char* filename) {
    // Create a new instance of HeaderAnalyzer
    HeaderAnalyzer* analyzer = new HeaderAnalyzer(filename);
    c_header_analyzer* c_analyzer = (c_header_analyzer*)malloc(sizeof(c_header_analyzer));
    c_analyzer->header_analyzer = analyzer;
    return c_analyzer;
}

void c_header_analyzer_destroy(c_header_analyzer* analyzer) {
    if (analyzer) {
        delete static_cast<HeaderAnalyzer*>(analyzer->header_analyzer);
        free(analyzer);
    }
}

const c_enum_info* c_header_analyzer_get_enums(c_header_analyzer* analyzer, size_t* count) {
    const auto& enums = static_cast<HeaderAnalyzer*>(analyzer->header_analyzer)->getEnums();
    *count = enums.size();
    
    c_enum_info* c_enums = (c_enum_info*)malloc(sizeof(c_enum_info) * (*count));
    
    for (size_t i = 0; i < *count; ++i) {
        c_enums[i].name = strdup(enums[i].name.c_str());
        c_enums[i].underlying_type = strdup(enums[i].underlyingType.c_str());
        c_enums[i].comment = strdup(enums[i].comment.c_str());
        
        c_enums[i].enumerator_count = enums[i].enumerators.size();
        c_enums[i].enumerators = (char**)malloc(sizeof(char*) * c_enums[i].enumerator_count);
        c_enums[i].values = (long long*)malloc(sizeof(long long) * c_enums[i].enumerator_count);
        
        for (size_t j = 0; j < c_enums[i].enumerator_count; ++j) {
            c_enums[i].enumerators[j] = strdup(enums[i].enumerators[j].first.c_str());
            c_enums[i].values[j] = enums[i].enumerators[j].second;
        }
    }
    
    return c_enums;
}

void c_enum_info_destroy(c_enum_info* enums, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(enums[i].name);
        free(enums[i].underlying_type);
        free(enums[i].comment);
        for (size_t j = 0; j < enums[i].enumerator_count; ++j) {
            free(enums[i].enumerators[j]);
        }
        free(enums[i].enumerators);
        free(enums[i].values);
    }
    free(enums);
}

const c_struct_info* c_header_analyzer_get_structs(c_header_analyzer* analyzer, size_t* count) {
    const auto& structs = static_cast<HeaderAnalyzer*>(analyzer->header_analyzer)->getStructs();
    *count = structs.size();
    
    c_struct_info* c_structs = (c_struct_info*)malloc(sizeof(c_struct_info) * (*count));
    
    for (size_t i = 0; i < *count; ++i) {
        c_structs[i].name = strdup(structs[i].name.c_str());
        c_structs[i].comment = strdup(structs[i].comment.c_str());
        
        c_structs[i].member_count = structs[i].members.size();
        c_structs[i].members = (c_struct_member*)malloc(sizeof(c_struct_member) * c_structs[i].member_count);
        
        for (size_t j = 0; j < c_structs[i].member_count; ++j) {
            c_structs[i].members[j].name = strdup(structs[i].members[j].name.c_str());
            c_structs[i].members[j].type = strdup(structs[i].members[j].type.c_str());
            c_structs[i].members[j].bitfield_width = structs[i].members[j].bitfieldWidth;
        }
    }
    
    return c_structs;
}

void c_struct_info_destroy(c_struct_info* structs, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(structs[i].name);
        free(structs[i].comment);
        for (size_t j = 0; j < structs[i].member_count; ++j) {
            free(structs[i].members[j].name);
            free(structs[i].members[j].type);
        }
        free(structs[i].members);
    }
    free(structs);
}

const c_function_info* c_header_analyzer_get_functions(c_header_analyzer* analyzer, size_t* count) {
    const auto& functions = static_cast<HeaderAnalyzer*>(analyzer->header_analyzer)->getFunctions();
    *count = functions.size();
    
    c_function_info* c_functions = (c_function_info*)malloc(sizeof(c_function_info) * (*count));
    
    for (size_t i = 0; i < *count; ++i) {
        c_functions[i].name = strdup(functions[i].name.c_str());
        c_functions[i].return_type = strdup(functions[i].returnType.c_str());
        c_functions[i].attributes = strdup(functions[i].attributes.c_str());
        c_functions[i].is_variadic = functions[i].isVariadic;
        c_functions[i].comment = strdup(functions[i].comment.c_str());
        
        c_functions[i].param_count = functions[i].parameters.size();
        c_functions[i].parameters = (char**)malloc(sizeof(char*) * c_functions[i].param_count);
        c_functions[i].param_types = (char**)malloc(sizeof(char*) * c_functions[i].param_count);
        
        for (size_t j = 0; j < c_functions[i].param_count; ++j) {
            c_functions[i].parameters[j] = strdup(functions[i].parameters[j].first.c_str());
            c_functions[i].param_types[j] = strdup(functions[i].parameters[j].second.c_str());
        }
    }
    
    return c_functions;
}

void c_function_info_destroy(c_function_info* functions, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(functions[i].name);
        free(functions[i].return_type);
        free(functions[i].attributes);
        free(functions[i].comment);
        for (size_t j = 0; j < functions[i].param_count; ++j) {
            free(functions[i].parameters[j]);
            free(functions[i].param_types[j]);
        }
        free(functions[i].parameters);
        free(functions[i].param_types);
    }
    free(functions);
}

const c_variable_info* c_header_analyzer_get_variables(c_header_analyzer* analyzer, size_t* count) {
    const auto& variables = static_cast<HeaderAnalyzer*>(analyzer->header_analyzer)->getVariables();
    *count = variables.size();
    
    c_variable_info* c_variables = (c_variable_info*)malloc(sizeof(c_variable_info) * (*count));
    
    for (size_t i = 0; i < *count; ++i) {
        c_variables[i].name = strdup(variables[i].name.c_str());
        c_variables[i].type = strdup(variables[i].type.c_str());
        c_variables[i].value = strdup(variables[i].value.c_str());
        c_variables[i].storage_class = strdup(variables[i].storageClass.c_str());
        c_variables[i].qualifiers = strdup(variables[i].qualifiers.c_str());
        
        c_variables[i].array_dimension_count = variables[i].arrayDimensions.size();
        c_variables[i].array_dimensions = (int*)malloc(sizeof(int) * c_variables[i].array_dimension_count);
        
        for (size_t j = 0; j < c_variables[i].array_dimension_count; ++j) {
            c_variables[i].array_dimensions[j] = variables[i].arrayDimensions[j];
        }
        
        c_variables[i].comment = strdup(variables[i].comment.c_str());
    }
    
    return c_variables;
}

void c_variable_info_destroy(c_variable_info* variables, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(variables[i].name);
        free(variables[i].type);
        free(variables[i].value);
        free(variables[i].storage_class);
        free(variables[i].qualifiers);
        free(variables[i].array_dimensions);
        free(variables[i].comment);
    }
    free(variables);
}

const c_typedef_info* c_header_analyzer_get_typedefs(c_header_analyzer* analyzer, size_t* count) {
    const auto& typedefs = static_cast<HeaderAnalyzer*>(analyzer->header_analyzer)->getTypedefs();
    *count = typedefs.size();
    
    c_typedef_info* c_typedefs = (c_typedef_info*)malloc(sizeof(c_typedef_info) * (*count));
    
    for (size_t i = 0; i < *count; ++i) {
        c_typedefs[i].new_name = strdup(typedefs[i].newName.c_str());
        c_typedefs[i].original_type = strdup(typedefs[i].originalType.c_str());
        c_typedefs[i].qualifiers = strdup(typedefs[i].qualifiers.c_str());
        c_typedefs[i].comment = strdup(typedefs[i].comment.c_str());
    }
    
    return c_typedefs;
}

void c_typedef_info_destroy(c_typedef_info* typedefs, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        free(typedefs[i].new_name);
        free(typedefs[i].original_type);
        free(typedefs[i].qualifiers);
        free(typedefs[i].comment);
    }
    free(typedefs);
}

void c_header_analyzer_write_to_xml(c_header_analyzer* analyzer, const char* output_filename) {
    static_cast<HeaderAnalyzer*>(analyzer->header_analyzer)->writeToXML(output_filename);
}
