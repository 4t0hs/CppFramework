
set(TARGET ${PROJECT_NAME})

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${TARGET} STATIC ${SOURCES})

target_compile_features(${TARGET} PUBLIC cxx_std_20)

target_compile_options(${TARGET} PUBLIC
	-Wall -Wextra
	$<$<CONFIG:Debug>:-O0 -ggdb -g3>
	$<$<CONFIG:TestPlus>:-fsanitize=address --coverage>
)

target_link_libraries(${TARGET} PRIVATE
	atomic
	$<$<CONFIG:TestPlus>:asan>
	$<$<CONFIG:TestPlus>:gcov>
)

add_subdirectory(Sample)
add_subdirectory(AsyncQueue)
add_subdirectory(Timer)
add_subdirectory(Sync)
add_subdirectory(ThreadPool)
add_subdirectory(Message)
