cmake_minimum_required(VERSION 3.10)
project(cboe-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
        src/main.cpp
        src/UdpReceiver.cpp
        src/message_factory.cpp
        src/gap_login.cpp
        src/login_response.cpp
        src/gap_request.cpp
        src/gap_response.cpp
        src/unit_clear.cpp
        src/trading_status.cpp
        src/add_order.cpp
        src/order_executed.cpp
        src/order_executed_at_price.cpp
        src/reduce_size.cpp
        src/modify_order.cpp
        src/delete_order.cpp
        src/trade.cpp
        src/trade_break.cpp
        src/calculated_value.cpp
        src/end_of_session.cpp
        src/auction_update.cpp
        src/auction_summary.cpp
        src/seq_unit_header.cpp
)

add_executable(test_cboe ${SOURCES})