#include "HeaderAnalyzer.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <unordered_set>

HeaderAnalyzer::HeaderAnalyzer(const std::string& filename) : m_filename(filename) {
    m_index = clang_createIndex(0, 0);
    m_translationUnit = clang_parseTranslationUnit(
        m_index,
        m_filename.c_str(), nullptr, 0,
        nullptr, 0,
        CXTranslationUnit_None);

    if (m_translationUnit == nullptr) {
        throw std::runtime_error("Unable to parse translation unit.");
    }

    CXCursor cursor = clang_getTranslationUnitCursor(m_translationUnit);
    clang_visitChildren(cursor, &HeaderAnalyzer::visitNode, this);
}

HeaderAnalyzer::~HeaderAnalyzer() {
    if (m_translationUnit)
        clang_disposeTranslationUnit(m_translationUnit);
    if (m_index)
        clang_disposeIndex(m_index);
}

const std::vector<HeaderAnalyzer::EnumInfo>& HeaderAnalyzer::getEnums() const { return m_enums; }
const std::vector<HeaderAnalyzer::StructInfo>& HeaderAnalyzer::getStructs() const { return m_structs; }
const std::vector<HeaderAnalyzer::FunctionInfo>& HeaderAnalyzer::getFunctions() const { return m_functions; }
const std::vector<HeaderAnalyzer::VariableInfo>& HeaderAnalyzer::getVariables() const { return m_variables; }
const std::vector<HeaderAnalyzer::TypedefInfo>& HeaderAnalyzer::getTypedefs() const { return m_typedefs; }

CXChildVisitResult HeaderAnalyzer::visitNode(CXCursor cursor, CXCursor parent, CXClientData client_data) {
	auto* analyzer = static_cast<HeaderAnalyzer*>(client_data);
	CXCursorKind kind = clang_getCursorKind(cursor);

	// Get the name of the cursor
	CXString cursorName = clang_getCursorSpelling(cursor);
	std::string name = clang_getCString(cursorName);
	clang_disposeString(cursorName); // Clean up the CXString

	// Check if the name has already been processed
	if (kind != CXCursor_TypedefDecl && analyzer->m_processedNames.find(name) != analyzer->m_processedNames.end()) {
	    return CXChildVisit_Recurse; // Skip processing if already processed
	}

	// Process the cursor and mark the name as processed
	switch (kind) {
	    case CXCursor_EnumDecl:
		analyzer->m_enums.push_back(processEnum(cursor));
		break;
	    case CXCursor_StructDecl:
		analyzer->m_structs.push_back(processStruct(cursor));
		break;
	    case CXCursor_FunctionDecl:
		analyzer->m_functions.push_back(processFunction(cursor));
		break;
	    case CXCursor_VarDecl:
		analyzer->m_variables.push_back(processVariable(cursor));
		break;
	    case CXCursor_TypedefDecl:
		analyzer->m_typedefs.push_back(processTypedef(cursor));
		break;
	}

	// Mark the name as processed
	analyzer->m_processedNames.insert(name);

	return CXChildVisit_Recurse;
}

std::string HeaderAnalyzer::getCursorSpelling(CXCursor cursor) {
    CXString spelling = clang_getCursorSpelling(cursor);
    const char* cStr = clang_getCString(spelling);
    std::string result = cStr ? cStr : ""; // Ensure we return an empty string if null
    clang_disposeString(spelling);
    return result;
}

std::string HeaderAnalyzer::getCursorType(CXCursor cursor) {
    CXType type = clang_getCursorType(cursor);
    CXString spelling = clang_getTypeSpelling(type);
    const char* cStr = clang_getCString(spelling);
    std::string result = cStr ? cStr : ""; // Ensure we return an empty string if null
    clang_disposeString(spelling);
    return result;
}

std::string HeaderAnalyzer::getCursorResultType(CXCursor cursor) {
    CXType type = clang_getCursorResultType(cursor);
    CXString spelling = clang_getTypeSpelling(type);
    const char* cStr = clang_getCString(spelling);
    std::string result = cStr ? cStr : ""; // Ensure we return an empty string if null
    clang_disposeString(spelling);
    return result;
}

std::string HeaderAnalyzer::getComment(CXCursor cursor) {
    CXString comment = clang_Cursor_getBriefCommentText(cursor);
    const char* cStr = clang_getCString(comment);
    std::string result = cStr ? cStr : ""; // Ensure we return an empty string if null
    clang_disposeString(comment);
    return result;
}

HeaderAnalyzer::EnumInfo HeaderAnalyzer::processEnum(CXCursor cursor) {
    EnumInfo info;
    info.name = getCursorSpelling(cursor);
    info.comment = getComment(cursor);

    clang_visitChildren(
        cursor,
        [](CXCursor c, CXCursor parent, CXClientData client_data) {
            auto* info = static_cast<EnumInfo*>(client_data);
            if (clang_getCursorKind(c) == CXCursor_EnumConstantDecl) {
                std::string name = getCursorSpelling(c);
                long long value = clang_getEnumConstantDeclValue(c);
                info->enumerators.emplace_back(name, value);
            }
            return CXChildVisit_Continue;
        },
        &info
    );

    CXType enumType = clang_getCursorType(cursor);
    CXType underlyingType = clang_getEnumDeclIntegerType(cursor);
    CXString underlyingTypeSpelling = clang_getTypeSpelling(underlyingType);
    info.underlyingType = clang_getCString(underlyingTypeSpelling);
    clang_disposeString(underlyingTypeSpelling);

    return info;
}

HeaderAnalyzer::StructInfo HeaderAnalyzer::processStruct(CXCursor cursor) {
    StructInfo info;
    info.name = getCursorSpelling(cursor);
    info.comment = getComment(cursor);

    clang_visitChildren(
        cursor,
        [](CXCursor c, CXCursor parent, CXClientData client_data) {
            auto* info = static_cast<StructInfo*>(client_data);
            if (clang_getCursorKind(c) == CXCursor_FieldDecl) {
                StructMember member;
                member.name = getCursorSpelling(c);
                member.type = getCursorType(c);
                member.bitfieldWidth = clang_getFieldDeclBitWidth(c);
                info->members.push_back(member);
            }
            return CXChildVisit_Continue;
        },
        &info
    );

    return info;
}

HeaderAnalyzer::FunctionInfo HeaderAnalyzer::processFunction(CXCursor cursor) {
    FunctionInfo info;
    info.name = getCursorSpelling(cursor);
    info.returnType = getCursorResultType(cursor);
    info.comment = getComment(cursor);

    int numArgs = clang_Cursor_getNumArguments(cursor);
    for (int i = 0; i < numArgs; ++i) {
        CXCursor arg = clang_Cursor_getArgument(cursor, i);
        std::string argName = getCursorSpelling(arg);
        std::string argType = getCursorType(arg);
        info.parameters.emplace_back(argName, argType);
    }

    info.isVariadic = clang_isFunctionTypeVariadic(clang_getCursorType(cursor));

    CXString attrSpelling = clang_getCursorDisplayName(cursor);
    const char* cStr = clang_getCString(attrSpelling);
    info.attributes = cStr ? cStr : ""; // Ensure we return an empty string if null
    clang_disposeString(attrSpelling);

    return info;
}

HeaderAnalyzer::VariableInfo HeaderAnalyzer::processVariable(CXCursor cursor) {
    VariableInfo info = initializeVariableInfo(cursor);
    info.storageClass = getStorageClass(cursor);
    info.qualifiers = getTypeQualifiers(cursor);
    info.arrayDimensions = getArrayDimensions(cursor);
    info.value = evaluateVariable(cursor);
    
    return info;
}

HeaderAnalyzer::VariableInfo HeaderAnalyzer::initializeVariableInfo(CXCursor cursor) {
    VariableInfo info;
    info.name = getCursorSpelling(cursor);
    info.type = getCursorType(cursor);
    info.comment = getComment(cursor);
    return info;
}

std::string HeaderAnalyzer::getStorageClass(CXCursor cursor) {
    CX_StorageClass storageClass = clang_Cursor_getStorageClass(cursor);
    switch (storageClass) {
        case CX_SC_None: return "";
        case CX_SC_Extern: return "extern";
        case CX_SC_Static: return "static";
        case CX_SC_PrivateExtern: return "private extern";
        case CX_SC_Auto: return "auto";
        case CX_SC_Register: return "register";
        default: return "unknown";
    }
}

std::string HeaderAnalyzer::getTypeQualifiers(CXCursor cursor) {
    CXType type = clang_getCursorType(cursor);
    std::string qualifiers;

    if (clang_isConstQualifiedType(type)) qualifiers += "const ";
    if (clang_isVolatileQualifiedType(type)) qualifiers += "volatile ";

    return qualifiers;
}

std::vector<int> HeaderAnalyzer::getArrayDimensions(CXCursor cursor) {
    CXType type = clang_getCursorType(cursor);
    std::vector<int> dimensions;

    while (clang_getArraySize(type) != -1) {
        dimensions.push_back(clang_getArraySize(type));
        type = clang_getArrayElementType(type);
    }

    return dimensions;
}

std::string HeaderAnalyzer::evaluateVariable(CXCursor cursor) {
    CXEvalResult evalResult = clang_Cursor_Evaluate(cursor);
    CXEvalResultKind kind = clang_EvalResult_getKind(evalResult);
    std::string value;

    switch (kind) {
        case CXEval_Int:
            value = std::to_string(clang_EvalResult_getAsInt(evalResult));
            break;
        case CXEval_Float:
            value = std::to_string(clang_EvalResult_getAsDouble(evalResult));
            break;
        case CXEval_ObjCStrLiteral:
        case CXEval_StrLiteral:
        case CXEval_CFStr:
            value = clang_EvalResult_getAsStr(evalResult);
            break;
        default:
            value = "Unable to evaluate";
    }
    
    clang_EvalResult_dispose(evalResult);
    return value;
}

HeaderAnalyzer::TypedefInfo HeaderAnalyzer::processTypedef(CXCursor cursor) {
    TypedefInfo info;
    info.newName = getCursorSpelling(cursor);
    info.originalType = getCursorType(cursor);
    info.comment = getComment(cursor);

    CXType type = clang_getCursorType(cursor);
    if (clang_isConstQualifiedType(type)) info.qualifiers += "const ";
    if (clang_isVolatileQualifiedType(type)) info.qualifiers += "volatile ";

    return info;
}

// Implementation of XML conversion methods
std::string HeaderAnalyzer::enumToXML(const EnumInfo& enumInfo) const {
    std::ostringstream xml;
    xml << "    <enum name=\"" << enumInfo.name << "\" underlying-type=\"" << enumInfo.underlyingType << "\">\n";
    if (!enumInfo.comment.empty()) {
        xml << "      <comment>" << enumInfo.comment << "</comment>\n";
    }
    xml << "      <enumerators>\n";
    for (const auto& enumerator : enumInfo.enumerators) {
        xml << "        <enumerator name=\"" << enumerator.first << "\" value=\"" << enumerator.second << "\"/>\n";
    }
    xml << "      </enumerators>\n";
    xml << "    </enum>\n";
    return xml.str();
}

std::string HeaderAnalyzer::structToXML(const StructInfo& structInfo) const {
    std::ostringstream xml;
    xml << "    <struct name=\"" << structInfo.name << "\">\n";
    if (!structInfo.comment.empty()) {
        xml << "      <comment>" << structInfo.comment << "</comment>\n";
    }
    xml << "      <members>\n";
    for (const auto& member : structInfo.members) {
        xml << "        <member name=\"" << member.name << "\" type=\"" << member.type << "\" bitfield-width=\"" << member.bitfieldWidth << "\"/>\n";
    }
    xml << "      </members>\n";
    xml << "    </struct>\n";
    return xml.str();
}

std::string HeaderAnalyzer::functionToXML(const FunctionInfo& functionInfo) const {
    std::ostringstream xml;
    xml << "    <function name=\"" << functionInfo.name << "\" return-type=\"" << functionInfo.returnType << "\" is-variadic=\"" << (functionInfo.isVariadic ? "true" : "false") << "\">\n";
    if (!functionInfo.comment.empty()) {
        xml << "      <comment>" << functionInfo.comment << "</comment>\n";
    }
    xml << "      <parameters>\n";
    for (const auto& param : functionInfo.parameters) {
        xml << "        <parameter name=\"" << param.first << "\" type=\"" << param.second << "\"/>\n";
    }
    xml << "      </parameters>\n";
    if (!functionInfo.attributes.empty()) {
        xml << "      <attributes>" << functionInfo.attributes << "</attributes>\n";
    }
    xml << "    </function>\n";
    return xml.str();
}

std::string HeaderAnalyzer::variableToXML(const VariableInfo& variableInfo) const {
    std::ostringstream xml;
    xml << "    <variable name=\"" << variableInfo.name << "\" type=\"" << variableInfo.type << "\" value=\"" << variableInfo.value << "\" storage-class=\"" << variableInfo.storageClass << "\">\n";
    if (!variableInfo.arrayDimensions.empty()) {
        xml << "      <array-dimensions>\n";
        for (const auto& dim : variableInfo.arrayDimensions) {
            xml << "        <dimension>" << dim << "</dimension>\n";
        }
        xml << "      </array-dimensions>\n";
    }
    if (!variableInfo.comment.empty()) {
        xml << "      <comment>" << variableInfo.comment << "</comment>\n";
    }
    xml << "    </variable>\n";
    return xml.str();
}

std::string HeaderAnalyzer::typedefToXML(const TypedefInfo& typedefInfo) const {
    std::ostringstream xml;
    xml << "    <typedef new-name=\"" << typedefInfo.newName << "\" original-type=\"" << typedefInfo.originalType << "\">\n";
    if (!typedefInfo.qualifiers.empty()) {
        xml << "      <qualifiers>" << typedefInfo.qualifiers << "</qualifiers>\n";
    }
    if (!typedefInfo.comment.empty()) {
        xml << "      <comment>" << typedefInfo.comment << "</comment>\n";
    }
    xml << "    </typedef>\n";
    return xml.str();
}

// Main function to write HeaderAnalyzer info to XML
void HeaderAnalyzer::writeToXML(const std::string& outputFilename) const {
    std::ostringstream xmlStream;

    // Start XML document
    xmlStream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xmlStream << "<header>\n";

    // Write Enums
    xmlStream << "  <enums>\n";
    for (const auto& enumInfo : getEnums()) {
        xmlStream << enumToXML(enumInfo);
    }
    xmlStream << "  </enums>\n";

    // Write Typedefs
    xmlStream << "  <typedefs>\n";
    for (const auto& typedefInfo : getTypedefs()) {
        xmlStream << typedefToXML(typedefInfo);
    }
    xmlStream << "  </typedefs>\n";

    // Write Structs
    xmlStream << "  <structs>\n";
    for (const auto& structInfo : getStructs()) {
        xmlStream << structToXML(structInfo);
    }
    xmlStream << "  </structs>\n";

    // Write Variables
    xmlStream << "  <variables>\n";
    for (const auto& variableInfo : getVariables()) {
        xmlStream << variableToXML(variableInfo);
    }
    xmlStream << "  </variables>\n";

    // Write Functions
    xmlStream << "  <functions>\n";
    for (const auto& functionInfo : getFunctions()) {
        xmlStream << functionToXML(functionInfo);
    }
    xmlStream << "  </functions>\n";

    // End XML document
    xmlStream << "</header>\n";

    // Write to file
    std::ofstream outFile(outputFilename);
    if (outFile.is_open()) {
        outFile << xmlStream.str();
        outFile.close();
        // std::cout << "XML written to " << outputFilename << std::endl;
    } else {
        std::cerr << "Error opening file for writing: " << outputFilename << std::endl;
    }
}
