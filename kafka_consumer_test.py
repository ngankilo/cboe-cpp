#!/usr/bin/env python3
"""
Kafka consumer test for CBOE Feed Handler
Verifies that messages are correctly sent to Kafka
"""

import json
import time
from kafka import KafkaConsumer
from kafka.errors import KafkaError
import argparse

def test_kafka_consumer(brokers, topics, timeout=30):
    """Test Kafka consumer to verify message flow"""
    
    print(f"Connecting to Kafka: {brokers}")
    print(f"Listening to topics: {topics}")
    
    try:
        consumer = KafkaConsumer(
            *topics,
            bootstrap_servers=brokers,
            value_deserializer=lambda x: x.decode('utf-8'),
            consumer_timeout_ms=timeout * 1000,
            auto_offset_reset='latest'  # Only new messages
        )
        
        print("Connected successfully. Waiting for messages...")
        print("=" * 50)
        
        message_count = 0
        symbol_stats = {}
        
        for message in consumer:
            message_count += 1
            
            topic = message.topic
            partition = message.partition
            offset = message.offset
            value = message.value
            
            try:
                # Parse JSON
                data = json.loads(value)
                symbol = data.get('symbol', 'unknown')
                msg_type = data.get('msgType', 'unknown')
                timestamp = data.get('timestamp', 'unknown')
                
                # Update statistics
                if symbol not in symbol_stats:
                    symbol_stats[symbol] = []
                symbol_stats[symbol].append(message_count)
                
                print(f"Message {message_count}:")
                print(f"  Topic: {topic}, Partition: {partition}, Offset: {offset}")
                print(f"  Symbol: {symbol}, MsgType: {msg_type}")
                print(f"  Data: {json.dumps(data, indent=2)}")
                print("-" * 40)
                
            except json.JSONDecodeError:
                print(f"Message {message_count}: Invalid JSON")
                print(f"  Raw: {value}")
                print("-" * 40)
                
    except KafkaError as e:
        print(f"Kafka error: {e}")
    except KeyboardInterrupt:
        print("\nStopping consumer...")
    finally:
        print("\n" + "=" * 50)
        print("SUMMARY:")
        print(f"Total messages received: {message_count}")
        print("\nMessages per symbol:")
        for symbol, messages in symbol_stats.items():
            print(f"  {symbol}: {len(messages)} messages")
            if len(messages) > 1:
                print(f"    Sequence: {messages}")

def main():
    parser = argparse.ArgumentParser(description='Test Kafka consumer for CBOE Feed Handler')
    parser.add_argument('--brokers', default='localhost:9092', 
                       help='Kafka broker addresses (default: localhost:9092)')
    parser.add_argument('--topics', nargs='+', 
                       default=['AAPL', 'GOOGL', 'MSFT', 'TSLA', 'AMZN'],
                       help='Topics to subscribe to')
    parser.add_argument('--timeout', type=int, default=30,
                       help='Consumer timeout in seconds (default: 30)')
    
    args = parser.parse_args()
    
    print("CBOE Feed Handler - Kafka Consumer Test")
    print("======================================")
    
    test_kafka_consumer(args.brokers.split(','), args.topics, args.timeout)

if __name__ == "__main__":
    main()