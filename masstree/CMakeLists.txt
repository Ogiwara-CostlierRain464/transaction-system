set(ENV{CC} gcc)
set(ENV{CXX} g++)

project(Masstree)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE PROJECT_SOURCES *.cpp)
file(GLOB_RECURSE PROJECT_HEADERS *.h)

add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
)