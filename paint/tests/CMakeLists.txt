# Tests sin framework externo: solo <cassert>. Suficiente para lo didáctico y
# se integra con CTest (ejecútalos con `ctest` tras compilar).
add_executable(paint_tests test_paint.cpp)
target_link_libraries(paint_tests PRIVATE paint_core)

add_test(NAME paint_tests COMMAND paint_tests)
