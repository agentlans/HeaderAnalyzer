#pragma once

#include <clang-c/Index.h>
#include <string>
#include <vector>
#include <unordered_set>

/**
 * @class HeaderAnalyzer
 * @brief Analyzes C/C++ header files to extract information about enums, structs, functions, variables, and typedefs.
 * 
 * This class provides methods to parse a specified header file and store information about its 
 * components, allowing users to retrieve details about enums, structs, functions, variables, 
 * and typedefs in a structured format.
 */
class HeaderAnalyzer {
public:
    /**
     * @struct EnumInfo
     * @brief Represents information about an enumeration type.
     * 
     * This structure holds the name of the enum, its enumerators, the underlying type, 
     * and an optional comment describing the enum.
     */
    struct EnumInfo {
        std::string name; /**< The name of the enumeration. */
        std::vector<std::pair<std::string, long long>> enumerators; /**< A vector of enumerator names and their associated values. */
        std::string underlyingType; /**< The underlying type of the enumeration. */
        std::string comment; /**< An optional comment describing the enumeration. */
    };

    /**
     * @struct StructMember
     * @brief Represents a member of a structure.
     * 
     * This structure contains the name, type, and bitfield width of a member within a struct.
     */
    struct StructMember {
        std::string name; /**< The name of the structure member. */
        std::string type; /**< The type of the structure member. */
        int bitfieldWidth; /**< The width of the bitfield, if applicable. */
    };

    /**
     * @struct StructInfo
     * @brief Represents information about a structure type.
     * 
     * This structure includes the name of the struct, its members, and an optional comment.
     */
    struct StructInfo {
        std::string name; /**< The name of the structure. */
        std::vector<StructMember> members; /**< A vector of members belonging to the structure. */
        std::string comment; /**< An optional comment describing the structure. */
    };

    /**
     * @struct FunctionInfo
     * @brief Represents information about a function.
     * 
     * This structure contains the function's name, return type, parameters, attributes, 
     * variadic status, and an optional comment.
     */
    struct FunctionInfo {
        std::string name; /**< The name of the function. */
        std::string returnType; /**< The return type of the function. */
        std::vector<std::pair<std::string, std::string>> parameters; /**< A vector of parameter names and their types. */
        std::string attributes; /**< Any attributes associated with the function. */
        bool isVariadic; /**< Indicates whether the function is variadic. */
        std::string comment; /**< An optional comment describing the function. */
    };

    /**
     * @struct VariableInfo
     * @brief Represents information about a variable.
     * 
     * This structure includes the variable's name, type, value, storage class, qualifiers, 
     * array dimensions, and an optional comment.
     */
    struct VariableInfo {
        std::string name; /**< The name of the variable. */
        std::string type; /**< The type of the variable. */
        std::string value; /**< The value of the variable, if applicable. */
        std::string storageClass; /**< The storage class of the variable (e.g., static, extern). */
        std::string qualifiers; /**< Any qualifiers associated with the variable (e.g., const, volatile). */
        std::vector<int> arrayDimensions; /**< A vector representing the dimensions of the array, if applicable. */
        std::string comment; /**< An optional comment describing the variable. */
    };

    /**
     * @struct TypedefInfo
     * @brief Represents information about a typedef.
     * 
     * This structure contains the new name for the typedef, the original type it refers to, 
     * any qualifiers, and an optional comment.
     */
    struct TypedefInfo {
        std::string newName; /**< The new name for the typedef. */
        std::string originalType; /**< The original type that the typedef refers to. */
        std::string qualifiers; /**< Any qualifiers associated with the typedef. */
        std::string comment; /**< An optional comment describing the typedef. */
    };

    /**
     * @brief Constructs a HeaderAnalyzer for the specified header file.
     * @param filename The path to the header file to analyze.
     */
    HeaderAnalyzer(const std::string& filename);

    /**
     * @brief Destructor for the HeaderAnalyzer.
     */
    ~HeaderAnalyzer();

    /**
     * @brief Retrieves a list of enumerations found in the analyzed header file.
     * @return A constant reference to a vector of EnumInfo structures.
     */
    const std::vector<EnumInfo>& getEnums() const;

    /**
     * @brief Retrieves a list of structures found in the analyzed header file.
     * @return A constant reference to a vector of StructInfo structures.
     */
    const std::vector<StructInfo>& getStructs() const;

    /**
     * @brief Retrieves a list of functions found in the analyzed header file.
     * @return A constant reference to a vector of FunctionInfo structures.
     */
    const std::vector<FunctionInfo>& getFunctions() const;

    /**
     * @brief Retrieves a list of variables found in the analyzed header file.
     * @return A constant reference to a vector of VariableInfo structures.
     */
    const std::vector<VariableInfo>& getVariables() const;

    /**
     * @brief Retrieves a list of typedefs found in the analyzed header file.
     * @return A constant reference to a vector of TypedefInfo structures.
     */
    const std::vector<TypedefInfo>& getTypedefs() const;

    /**
     * @brief Writes the analyzed information to an XML file.
     * @param outputFilename The name of the output XML file.
     */
    void writeToXML(const std::string& outputFilename) const;

private:
    std::string m_filename;
    CXIndex m_index;
    CXTranslationUnit m_translationUnit;

    std::vector<EnumInfo> m_enums;
    std::vector<StructInfo> m_structs;
    std::vector<FunctionInfo> m_functions;
    std::vector<VariableInfo> m_variables;
    std::vector<TypedefInfo> m_typedefs;

    // For tracking processed names
    std::unordered_set<std::string> m_processedNames;

    static CXChildVisitResult visitNode(CXCursor cursor, CXCursor parent, CXClientData client_data);
    static std::string getCursorSpelling(CXCursor cursor);
    static std::string getCursorType(CXCursor cursor);
    static std::string getCursorResultType(CXCursor cursor);
    static std::string getComment(CXCursor cursor);
    static EnumInfo processEnum(CXCursor cursor);
    static StructInfo processStruct(CXCursor cursor);
    static FunctionInfo processFunction(CXCursor cursor);
    static VariableInfo processVariable(CXCursor cursor);
    static TypedefInfo processTypedef(CXCursor cursor);

    // Helper function declarations for variable info
    static VariableInfo initializeVariableInfo(CXCursor cursor);
    static std::string getStorageClass(CXCursor cursor);
    static std::string getTypeQualifiers(CXCursor cursor);
    static std::vector<int> getArrayDimensions(CXCursor cursor);
    static std::string evaluateVariable(CXCursor cursor);

    // XML conversion methods
    std::string enumToXML(const EnumInfo& enumInfo) const;
    std::string structToXML(const StructInfo& structInfo) const;
    std::string functionToXML(const FunctionInfo& functionInfo) const;
    std::string variableToXML(const VariableInfo& variableInfo) const;
    std::string typedefToXML(const TypedefInfo& typedefInfo) const;
};
