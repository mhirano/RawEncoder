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

#	set(OpenCV_DIR /Users/mhirano/opencv-3.4.1-tuned/release)
	set(OpenCV_DIR /Users/mhirano/opencv-3.4.10/build)
#	set(OpenCV_DIR /Users/mhirano/opencv-4.3.0/build)

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
if(UNIX AND NOT APPLE)
	find_package(OpenMP)
	if (OpenMP_FOUND)
		message(STATUS "YES!! OpenMP found.")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		message(STATUS "OpenMP CXX flags: ${OpenMP_CXX_FLAGS}")
	else (OpenMP_FOUND)
		message(STATUS "No!!   OpenMP did not find.")
	endif (OpenMP_FOUND)
elseif(APPLE)
	# libompのインストール先パスをbrewコマンドを実行して取得する.
	execute_process(COMMAND brew --prefix libomp
			OUTPUT_VARIABLE OpenMP_HOME
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	message(STATUS "OpenMP Root : ${OpenMP_HOME}")
	# OpenMPのインストールチェック
	set(OpenMP_C_LIB_NAMES "omp")
	set(OpenMP_CXX_LIB_NAMES "omp")
	set(OpenMP_omp_LIBRARY "${OpenMP_HOME}/lib/")
	# キャッシュ変数なのでFORCEオプションをつける
	set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -Wno-unused-command-line-argument -I${OpenMP_HOME}/include -lomp -L${OpenMP_omp_LIBRARY}" CACHE STRING "" FORCE)
	# 同じくキャッシュ変数なのでFORCEオプションをつける
	set(OpenMP_C_FLAGS "-fopenmp -Wno-unused-command-line-argument -I${OpenMP_HOME}/include -lomp -L${OpenMP_omp_LIBRARY}" CACHE STRING "" FORCE)

	# libompがインストールされているかどうか.
	find_package(OpenMP)
	# 見つかったか？
	message(STATUS "OpenMP_C_Found   : ${OpenMP_C_FOUND}")
	message(STATUS "OpenMP_CXX_Found : ${OpenMP_CXX_FOUND}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

	if (OpenMP_FOUND)
		message(STATUS "YES!! OpenMP found.")
	else (OpenMP_FOUND)
		message(STATUS "No!!   OpenMP did not find.")
	endif (OpenMP_FOUND)
endif()

# cvut
include_directories(3rdparty/cvut/include)


######## ######## ######## ######## ######## ######## ######## ########
# Compiler settings
######## ######## ######## ######## ######## ######## ######## ########
# Check C++17 support and activate

#if(UNIX AND NOT APPLE)
#	set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")
#elseif(APPLE)
#	set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-9")
#endif()

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

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-DRELEASE -O3 -DNDEBUG -march=native")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
######## ######## ######## ######## ######## ######## ######## ########

set( src
		${PROJECT_SOURCE_DIR}/src/main.cpp
		)

add_executable( ${PROJECT_NAME}  ${src} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} ${mylibs})
#target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} ${mylibs} -lstdc++fs)

