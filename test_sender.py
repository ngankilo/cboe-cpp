#!/usr/bin/env python3
"""
Test UDP sender for CBOE Feed Handler
Sends test messages to verify the application works correctly
"""

import socket
import time
import json
import random
import threading
from datetime import datetime

class UdpTestSender:
    def __init__(self, host='127.0.0.1', port=9000):
        self.host = host
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
    def send_message(self, header, mstype, symbol, data):
        """Send a single message in the expected format"""
        message = f"{header}-{mstype}-{symbol}-{data}"
        self.socket.sendto(message.encode(), (self.host, self.port))
        print(f"Sent: {message}")
        
    def send_test_sequence(self):
        """Send a test sequence to verify ordering requirements"""
        print("=== Testing ordering requirements ===")
        print("Sending: A A A B C A")
        print("Expected: A1→A2→A3→A6 (sequential), B4||C5 (parallel)")
        
        messages = [
            ("HDR001", "1", "AAPL", '{"price":150.25,"volume":1000,"seq":1}'),
            ("HDR002", "1", "AAPL", '{"price":150.30,"volume":1500,"seq":2}'),
            ("HDR003", "1", "AAPL", '{"price":150.35,"volume":2000,"seq":3}'),
            ("HDR004", "2", "GOOGL", '{"bid":2800.00,"ask":2801.00,"seq":4}'),
            ("HDR005", "3", "MSFT", '{"last":300.50,"size":500,"seq":5}'),
            ("HDR006", "1", "AAPL", '{"price":150.40,"volume":800,"seq":6}'),
        ]
        
        for header, mstype, symbol, data in messages:
            self.send_message(header, mstype, symbol, data)
            time.sleep(0.1)  # Small delay to see ordering
            
    def send_high_frequency_test(self, duration=10, rate=1000):
        """Send high frequency messages for performance testing"""
        print(f"=== High frequency test: {rate} msg/s for {duration}s ===")
        
        symbols = ["AAPL", "GOOGL", "MSFT", "TSLA", "AMZN"]
        start_time = time.time()
        count = 0
        
        while time.time() - start_time < duration:
            symbol = random.choice(symbols)
            mstype = random.randint(1, 5)
            
            data = {
                "price": round(random.uniform(100, 3000), 2),
                "volume": random.randint(100, 10000),
                "timestamp": int(time.time() * 1000),
                "seq": count
            }
            
            header = f"HDR{count:06d}"
            self.send_message(header, str(mstype), symbol, json.dumps(data))
            
            count += 1
            time.sleep(1.0 / rate)
            
        print(f"Sent {count} messages in {duration} seconds")
        print(f"Average rate: {count/duration:.1f} msg/s")

def main():
    sender = UdpTestSender()
    
    print("CBOE Feed Handler Test Sender")
    print("=============================")
    
    try:
        # Test 1: Ordering requirements
        sender.send_test_sequence()
        time.sleep(2)
        
        # Test 2: Different message types
        print("\n=== Testing different message types ===")
        test_messages = [
            ("HDR100", "10", "BOND001", '{"yield":2.5,"maturity":"2025-01-01"}'),
            ("HDR101", "20", "FUT001", '{"strike":150,"expiry":"2024-12-20"}'),
            ("HDR102", "30", "OPT001", '{"delta":0.5,"gamma":0.1}'),
        ]
        
        for header, mstype, symbol, data in test_messages:
            sender.send_message(header, mstype, symbol, data)
            time.sleep(0.5)
            
        # Test 3: High frequency (optional)
        print("\n=== High frequency test (optional) ===")
        response = input("Run high frequency test? (y/N): ")
        if response.lower() == 'y':
            sender.send_high_frequency_test(duration=5, rate=100)
            
        print("\nTest completed!")
        
    except KeyboardInterrupt:
        print("\nTest interrupted by user")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        sender.socket.close()

if __name__ == "__main__":
    main() 