from scapy.all import rdpcap, UDP, IP
import socket
import time

PCAP_FILENAME = '2messsage.pcap'   # <-- Replace with your .pcap file name/path
LO_IP = '127.0.0.1'
DELAY_BETWEEN_PACKETS = 0.01   # seconds (adjust if needed, helps with network stability)

def replay_pcap_loop():
    print(f"Loading pcap file: {PCAP_FILENAME} ...")
    packets = rdpcap(PCAP_FILENAME)
    print(f"Loaded {len(packets)} packets from {PCAP_FILENAME}")

    while True:
        print("Replaying packets...")
        for pkt in packets:
            # Only handle UDP over IP packets
            if pkt.haslayer(UDP) and pkt.haslayer(IP):
                udp_layer = pkt[UDP]
                payload = bytes(udp_layer.payload)
                if not payload:
                    continue  # skip empty payloads

                dst_port = 30501
                with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
                    sock.sendto(payload, (LO_IP, dst_port))
                print(f"Sent to {LO_IP}:{dst_port} ({len(payload)} bytes)")
                time.sleep(DELAY_BETWEEN_PACKETS)
        print("One loop finished. Restarting...")

if __name__ == "__main__":
    replay_pcap_loop()