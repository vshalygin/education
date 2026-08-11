add_library(VendorLib INTERFACE)
target_compile_definitions(VendorLib INTERFACE TUTORIAL_USE_VENDORLIB)

target_include_directories(VendorLib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_directories(VendorLib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib")

target_link_libraries(VendorLib INTERFACE Vendor)
