cmake_minimum_required(VERSION 3.27)

project(strategies)

add_subdirectory(api)

add_subdirectory(all-cooperate)
add_subdirectory(all-defect)
add_subdirectory(go-by-majority)
add_subdirectory(random)
add_subdirectory(soft-tit-for-tat)
add_subdirectory(tough-tit-for-tat)
add_subdirectory(combined)
