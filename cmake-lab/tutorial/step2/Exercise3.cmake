cmake_minimum_required(VERSION 3.23)


set(SKIP_TESTS TRUE)

include(Exercise1.cmake)
include(Exercise2.cmake)

set(InList FooBar QuxBar)

FuncAppend(InList FooBaz)
FuncAppend(InList QuxBaz)


if(NOT InList STREQUAL "FooBar;QuxBar;FooBaz;QuxBaz")
  message(WARNING "Append failed, InList contains: ${InList}")
endif()


FilterFoo(OutList "${InList}")

check_contains(FooBar)
check_contains(FooBaz)
check_nonfoo(${OutList})

if(NOT Failed)
  message("Success!")
endif()
