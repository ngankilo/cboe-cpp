#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include "message_parser.hpp"
#include "message_router.hpp"
#include "kafka_producer.hpp"

using boost::asio::ip::udp;
using json = nlohmann::json;

class UdpServer {
public:
    UdpServer(boost::asio::io_context& io_context, unsigned short port,
              MessageRouter& router, KafkaProducer& producer)
        : socket_(io_context, udp::endpoint(udp::v4(), port)),
          router_(router),
          producer_(producer) {
        do_receive();
    }

private:
    void do_receive() {
        socket_.async_receive_from(
            boost::asio::buffer(data_, max_length), sender_endpoint_,
            [this](boost::system::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    std::string message(data_, bytes_recvd);
                    try {
                        auto parsed = MessageParser::parse(message);
                        
                        // Convert data to JSON
                        json j = json::parse(parsed.data);
                        
                        // Route message
                        router_.route_message(parsed.mstype, parsed.symbol, j.dump());
                        
                        // Produce to Kafka
                        producer_.produce(parsed.symbol, j.dump(), std::stoi(parsed.mstype));
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing message: " << e.what() << std::endl;
                    }
                }
                do_receive();
            });
    }

    udp::socket socket_;
    udp::endpoint sender_endpoint_;
    enum { max_length = 65536 };
    char data_[max_length];
    MessageRouter& router_;
    KafkaProducer& producer_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " <port> <kafka_brokers>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;
        MessageRouter router;
        KafkaProducer producer(argv[2]);
        
        UdpServer server(io_context, std::atoi(argv[1]), router, producer);
        
        std::cout << "Server started on port " << argv[1] << std::endl;
        
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}