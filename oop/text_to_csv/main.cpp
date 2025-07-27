/*

Требования к программе:

Для работы со строками используйте класс стандартной библиотеки std::string (см. http://www.cplusplus.com/reference/string/string/)

Работа с файлами должна осуществляться с помощью классов стандартной библиотеки из модуля fstream (http://www.cplusplus.com/reference/fstream/)

Строки из файла должны зачитываться с помощью метода std::getline (см. http://www.cplusplus.com/reference/string/string/getline/)

Сохраняйте зачитанные строки в контейнере стандартной библиотеки std::list (http://www.cplusplus.com/reference/list/list/), std::map (http://www.cplusplus.com/reference/map/map/)


Задача 0б
Напишите программу, которая будет принимать в качестве аргумента имя текстового файла, и выводить CSV файл (http://ru.wikipedia.org/wiki/CSV) с колонками:
1. Слово.
2. Частота.
3. Частота (в %).
CSV файл должен быть упорядочен по убыванию частоты, то есть самые частые слова
должны идти в начале. Разделителями считать все символы кроме букв и цифр.


Аргументы командной строки программы:
> word_count.exe input.txt output.csv


*/
#include "util.h"

int main(int argc, char const *argv[]) {
    std::locale::global(std::locale("ru_RU.UTF8").combine<std::numpunct<wchar_t>>(std::locale("")));
    if (CheckFiles(argc, argv[1], argv[2]) == EXIT_FAILURE)
        return EXIT_FAILURE;
    std::wofstream out(argv[2]);
    if (out.fail()) {
        std::cerr << "Couldn't open output file.\n";
        return EXIT_FAILURE;
    }
    std::wstring line;
    std::map<std::wstring, int> words;
    int wordCount=0;
    if (ReadInput(line, words, argv[1], wordCount) == EXIT_FAILURE) {
        std::cerr << "Input was read incorrectly\n";
        return EXIT_FAILURE;
    }
    std::vector<std::pair<std::wstring, int>> vec = SortWords(words);
    PrintVector(vec,argv[2], wordCount);
    return 0;
}