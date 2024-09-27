#ifndef PRINTTUPLE_H
#define PRINTTUPLE_H

#include <iostream>
#include <tuple>

template<class Ch, class Tr, class Tuple, size_t... indices>
void printTupleImplementation(std::basic_ostream<Ch, Tr>&os,
                              const Tuple&t,
                              std::index_sequence<indices...>) {
    ((os << (indices == 0 ? "" : ", ") << std::get<indices>(t)), ...);
}

template<class Ch, class Tr, class... Args>
auto& operator<<(std::basic_ostream<Ch, Tr>&os, std::tuple<Args...> const&t) {
    os << "Tuple is: ";
    printTupleImplementation(os, t, std::index_sequence_for<Args...>{});
    return os;
}

#endif //PRINTTUPLE_H
