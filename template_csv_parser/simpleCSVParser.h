#ifndef SIMPLECSVPARSER_H
#define SIMPLECSVPARSER_H
#define FMT_HEADER_ONLY

#include <fstream>
#include <sstream>
#include <tuple>
#include <string>
#include <limits>
#include <fmt/format.h>
#include <fmt/core.h>
#include <fmt/args.h>

#include "simpleCSVParser.h"

namespace CSVParserError {
    class TypeMismatch : public std::runtime_error {
    public:
        TypeMismatch(int row, int column, const std::string&type,
                     const std::string&tupleElem);
    };

    class BadSkippedLinesNumber : public std::runtime_error {
    public:
        BadSkippedLinesNumber();
    };

    class FileError : public std::runtime_error {
    public:
        explicit FileError(const std::string&message);
    };
}

template<class... Args>
class CSVParser {
public:
    explicit
    CSVParser(std::ifstream&outputFile, int skips = 0, char escape = '\\', char columnDelim = ',',
              char rowDelim = '\n') : file{outputFile}, escapeCharacter{escape}, columnDelimeter{columnDelim},
                                      rowDelimeter{rowDelim} {
        if (skips < 0) {
            throw CSVParserError::BadSkippedLinesNumber();
        }
        if (!file.is_open()) {
            throw CSVParserError::FileError("file's not open");
        }
        for (int i = 0; i < skips; ++i) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), rowDelim);
        }
    }

    CSVParser& operator>>(std::tuple<Args...>&tuple) {
        if (!file.is_open()) {
            throw CSVParserError::FileError("file's not open");
        }
        column = 1;
        std::getline(file, line, rowDelimeter);
        if (line.empty()) {
            return *this;
        }
        std::apply([&](auto&... tupleElems) {
            ((tupleElems = GetTupleElement<Args>()), ...);
        }, tuple);
        row++;
        return *this;
    }

    class InputIterator;

    auto begin() {
        return InputIterator(this);
    }

    auto end() {
        return InputIterator();
    }

private:
    std::ifstream&file;
    int column = 1, row = 1;
    std::string line;
    char escapeCharacter, columnDelimeter, rowDelimeter;

    template<class t>
    t GetTupleElement() {
        std::string tupleElement;
        for (int commaIndex = line.find(columnDelimeter);
             commaIndex != std::string::npos;
             commaIndex = line.find(columnDelimeter)) {
            if (line[commaIndex - 1] == escapeCharacter) {
                auto tupleElemPart = std::string(line.substr(0, commaIndex - 1)) + columnDelimeter;
                line = line.substr(commaIndex + 1);
                tupleElement += tupleElemPart;
            }
            else {
                auto tmp = line.substr(0, commaIndex);
                tupleElement += tmp;
                line = line.substr(commaIndex + 1);
                //for windows files
                if (tupleElement.ends_with('\r')) {
                    tupleElement.pop_back();
                }
                if constexpr (std::is_same<t, std::string>()) {
                    return tupleElement;
                }
                t res;
                std::basic_stringstream<char, std::char_traits<char>> stream(tupleElement);
                stream >> res;
                ++column;
                if (stream.bad() || !stream.eof() || stream.fail()) {
                    throw CSVParserError::TypeMismatch(row, column, typeid(t).name(), tupleElement);
                }
                return res;
            }
        }
        tupleElement += line;
        //for windows files
        if (tupleElement.ends_with('\r')) {
            tupleElement.pop_back();
        }
        if constexpr (std::is_same<t, std::string>()) {
            return tupleElement;
        }
        t res;
        std::basic_stringstream<char, std::char_traits<char>> stream(tupleElement);
        stream >> res;
        ++column;
        if (stream.bad() || !stream.eof() || stream.fail()) {
            throw CSVParserError::TypeMismatch(row, column, typeid(t).name(), tupleElement);
        }
        return res;
    }
};

template<class... Args>
class CSVParser<Args...>::InputIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::tuple<Args...>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    InputIterator() = default;

    explicit InputIterator(CSVParser<Args...>* pars) : parser(pars) {
        if (pars->file.eof()) {
            //eof
            parser = nullptr;
        }
        else {
            *parser >> tuple;
        }
    }

    reference operator*() {
        return tuple;
    }

    pointer operator->() {
        return &tuple;
    }

    //prefix increment
    InputIterator& operator++() {
        // https://en.cppreference.com/w/cpp/io/basic_streambuf/in_avail
        if (parser->file.rdbuf()->in_avail() == 0) {
            parser = nullptr;
            return *this;
        }
        *parser >> tuple;
        return *this;
    }

    //postfix increment
    InputIterator operator++(int) {
        InputIterator res = *this;
        ++(*this);
        return res;
    }

    friend bool operator==(const InputIterator&a, const InputIterator&b) {
        return (a.parser == b.parser);
    };

    friend bool operator!=(const InputIterator&a, const InputIterator&b) {
        return (a.parser != b.parser);
    };

private:
    CSVParser<Args...>* parser = nullptr;
    std::tuple<Args...> tuple;
};

#endif //SIMPLECSVPARSER_H
