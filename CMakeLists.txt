cmake_minimum_required(VERSION 3.15)
project(DispatchQueue VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(dispatch_queue_app
    main.cc
    DispatchQueue.cc
    SerialWorker.cc
    DynamicThreadPool.cc
    StealingThreadPool.cc
    BoundedThreadPool.cc
    DispatchQueueWorkerCreator.cc
)

target_compile_features(dispatch_queue_app PRIVATE cxx_std_17)
