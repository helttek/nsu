#include "simpleCSVParser.h"

#include <filesystem>

CSVParserError::TypeMismatch::TypeMismatch(const int row, const int column, const std::string&type,
                                           const std::string&tupleElem): runtime_error(
    fmt::format("Error: tuple element type mismatch at row {}, column {}. Expecte type {} at tuple element ({})\n", row,
                column, type, tupleElem)) {
}

CSVParserError::BadSkippedLinesNumber::BadSkippedLinesNumber(): runtime_error(fmt::format(
    "Error: negative number of skipped lines\n")) {
}

CSVParserError::FileError::FileError(const std::string&message): runtime_error(fmt::format(
    "Error: {}\n",
    message)) {
}
