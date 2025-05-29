cmake_minimum_required(VERSION 3.10)
project(cboe_feed_handler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

# Find required packages
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

# Try to find nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # If not found, use header-only version
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
        DOWNLOAD_NO_EXTRACT TRUE
        DOWNLOAD_NAME json.hpp
    )
    FetchContent_Populate(nlohmann_json)
    file(COPY ${nlohmann_json_SOURCE_DIR}/json.hpp DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann/)
endif()

# Optional: Try to find librdkafka
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(RDKAFKA rdkafka++)
endif()

# Add source files (exclude the new main files for now)
file(GLOB SOURCES 
    "src/UdpReceiver.cpp"
    "src/MessageFactory.cpp"
)

# Create executable with original main
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Threads::Threads
    ${Boost_LIBRARIES}
)

# Link Kafka if available
if(RDKAFKA_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${RDKAFKA_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${RDKAFKA_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_KAFKA)
endif()

# Link nlohmann_json if found
if(nlohmann_json_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
endif() 