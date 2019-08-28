cmake_minimum_required(VERSION 3.0)

project( RawEncoder CXX )

#### Environment
if (UNIX AND NOT APPLE) #Ubuntu...
	message(STATUS ">>> Linux")

	set(OpenCV_DIR /home/mhirano/opencv-4.1.1/build)

	include_directories(/usr/include)
	link_directories(/usr/lib/x86_64-linux-gnu)

else(APPLE)
	message(STATUS ">>> Mac")

	set(OpenCV_DIR /Users/mhirano/opencv-3.4.1-tuned/release)

	# Add include directory	(environment specific)
	include_directories(/usr/local/include)

endif()

set(mylibs "")

# OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include )

#
# OpenMP
#
find_package(OpenMP)
if (OPENMP_FOUND)
	if(UNIX AND NOT APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		# set(autocalib_lib "${mylibs};fopenmp")
	elseif(APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp")
		set(autocalib_lib "${mylibs};omp;")
	endif()
	message(STATUS "OpenMP CXX flags: ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)



######## ######## ######## ######## ######## ######## ######## ########
# Compiler settings
######## ######## ######## ######## ######## ######## ######## ########
# Check C++14 support and activate

set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
	set(CMAKE_CXX_STANDARD 17) # C++17...
	set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
	set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++17 support.")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-DRELEASE -O3 -DNDEBUG -march=native")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
######## ######## ######## ######## ######## ######## ######## ########

set( src
		${PROJECT_SOURCE_DIR}/src/main.cpp
		${PROJECT_SOURCE_DIR}/include/VideoManager.h
		${PROJECT_SOURCE_DIR}/src/VideoManager.cpp
		${PROJECT_SOURCE_DIR}/include/Utility.h
		)

add_executable( ${PROJECT_NAME}  ${src} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} ${mylibs} -lstdc++fs)

