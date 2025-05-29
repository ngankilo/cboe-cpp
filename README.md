# CBOE Feed Handler

Ứng dụng xử lý feed dữ liệu CBOE với hiệu năng cao và độ trễ thấp, được viết bằng C++.

## Kiến trúc

### Thành phần chính:

1. **UdpReceiver**: Nhận UDP packets với Real-time scheduling và CPU affinity
2. **MessageParser**: Phân tích cấu trúc message `<header>-<mstype>-<symbol>-data`
3. **Dispatcher**: Điều phối xử lý message với đảm bảo thứ tự theo symbol
4. **JsonKafkaHandler**: Chuyển đổi message thành JSON và gửi đến Kafka
5. **KafkaProducer**: Gửi message đến Kafka với cấu hình tối ưu

### Đặc điểm hiệu năng:

- **Xử lý đúng thứ tự**: Message cùng symbol được xử lý tuần tự
- **Xử lý song song**: Message khác symbol được xử lý song song
- **Real-time scheduling**: UDP receiver với SCHED_FIFO priority
- **CPU affinity**: Pin thread vào CPU core cụ thể
- **Zero-copy processing**: Tối ưu memory allocation
- **Kafka batching**: Cấu hình tối ưu cho throughput

## Yêu cầu

### Dependencies:
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    librdkafka-dev \
    nlohmann-json3-dev \
    pkg-config

# macOS
brew install boost librdkafka nlohmann-json cmake
```

## Build

```bash
mkdir build && cd build
cmake ..

# Linux
make -j$(nproc)

# macOS
make -j$(sysctl -n hw.ncpu)
```

## Sử dụng

### Chạy ứng dụng:
```bash
# Syntax: ./cboe_feed_handler <ip> <port> <kafka_brokers>
./cboe_feed_handler 0.0.0.0 9000 localhost:9092
```

### Message format:
```
<header>-<mstype>-<symbol>-<data>
```

Ví dụ:
```
HDR001-1-AAPL-{"price":150.25,"volume":1000}
HDR002-2-GOOGL-{"bid":2800.00,"ask":2801.00}
```

### Kafka output:
- **Topic**: Symbol (AAPL, GOOGL, etc.)
- **Partition**: Hash của mstype
- **Message**: JSON với metadata và data

## Cấu hình hiệu năng

### System tuning (Optional):
```bash
# Increase UDP buffer size
echo 'net.core.rmem_max = 268435456' >> /etc/sysctl.conf
echo 'net.core.rmem_default = 268435456' >> /etc/sysctl.conf

# Enable real-time scheduling (cần root)
echo '* soft rtprio 99' >> /etc/security/limits.conf
echo '* hard rtprio 99' >> /etc/security/limits.conf
```

### Kafka topic tạo trước:
```bash
# Tạo topic cho symbols với multiple partitions
kafka-topics --create --topic AAPL --partitions 32 --replication-factor 1 --bootstrap-server localhost:9092
kafka-topics --create --topic GOOGL --partitions 32 --replication-factor 1 --bootstrap-server localhost:9092
```

## Kiến trúc chi tiết

### Flow xử lý:
```
UDP Packet → UdpReceiver → FastMessageParser → Dispatcher → JsonKafkaHandler → KafkaProducer → Kafka
```

### Thread model:
- **1 thread**: UDP receiver (real-time priority)
- **N threads**: Dispatcher workers (N = số CPU cores)
- **1 thread**: Kafka producer background

### Đảm bảo thứ tự:
```
Messages: A₁ A₂ A₃ B₁ C₁ A₄

Processing:
- A₁ → A₂ → A₃ → A₄ (tuần tự)
- B₁ (song song với A₁)
- C₁ (song song với A₁, B₁)
```

## Monitoring

Ứng dụng in ra statistics mỗi 5 giây:
```
Starting CBOE Feed Handler...
UDP: 0.0.0.0:9000
Kafka: localhost:9092
Threads: 8
Server started. Press Ctrl+C to stop.
Running for 5s, processing messages...
```

## Troubleshooting

### Common issues:

1. **Permission denied for real-time priority**:
   - Chạy với sudo hoặc cấu hình limits.conf

2. **Kafka connection failed**:
   - Kiểm tra Kafka broker đang chạy
   - Kiểm tra network connectivity

3. **UDP packets dropped**:
   - Tăng UDP buffer size
   - Kiểm tra CPU usage

### Performance tuning:

1. **CPU cores**: Điều chỉnh số worker threads
2. **Kafka batching**: Tùy chỉnh batch.size và linger.ms
3. **Memory**: Kiểm tra memory allocation patterns

## Future enhancements

- [ ] Multicast UDP support
- [ ] Message compression
- [ ] Metrics và monitoring
- [ ] Configuration file support
- [ ] Multiple Kafka clusters
- [ ] Message persistence
