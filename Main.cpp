#include <iostream>
#include <string>
#include "HeaderAnalyzer.h" // Make sure to include the header file where HeaderAnalyzer class is defined

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_header_file> <output_xml_file>" << std::endl;
        return 1;
    }

    // Retrieve the input and output file names from command line arguments
    std::string inputHeaderFile = argv[1];
    std::string outputXMLFile = argv[2];

    try {
        // Create an instance of HeaderAnalyzer with the input header file
        HeaderAnalyzer analyzer(inputHeaderFile);

        // Write the analyzed information to the specified XML output file
        analyzer.writeToXML(outputXMLFile);

        // Indicate successful processing
        // std::cout << "Successfully processed " << inputHeaderFile << " and wrote to " << outputXMLFile << std::endl;
    } catch (const std::exception& e) {
        // Handle any exceptions that may occur during processing
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
