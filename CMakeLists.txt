cmake_minimum_required(VERSION 3.0.0)
project(stocktotum VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(external/crow)
add_subdirectory(external/tomlplusplus)
add_subdirectory(external/nlohmann)
add_subdirectory(external/cpr)

file(GLOB SOURCES
    src/main.cpp
    src/config.*
    src/alphvant/*.h
    src/stocktotum/*.h
)

add_executable(stocktotum src/main.cpp ${SOURCES})

target_link_libraries(stocktotum PRIVATE Crow::Crow tomlplusplus::tomlplusplus nlohmann_json::nlohmann_json cpr::cpr)

include(CTest)
enable_testing()

