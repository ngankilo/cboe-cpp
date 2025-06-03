import dpkt
import socket
import time


def forward_pcap_to_udp(pcap_file, target_ip="127.0.0.1", target_port=9000, interval=0.1):
    """
    Reads a pcap file using dpkt and forwards its packets as UDP datagrams to a local port.

    Args:
        pcap_file (str): Path to the .pcap file.
        target_ip (str): Target IP address (default: 127.0.0.1).
        target_port (int): Target UDP port (default: 9000).
        interval (float): Delay between sending packets in seconds (default: 0.1).
    """
    # Open and read the pcap file
    with open(pcap_file, 'rb') as f:
        pcap = dpkt.pcap.Reader(f)

        # Create a UDP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        try:
            print(f"Forwarding packets from {pcap_file} to {target_ip}:{target_port}...")
            for timestamp, buf in pcap:
                # Parse the packet
                eth = dpkt.ethernet.Ethernet(buf)
                if isinstance(eth.data, dpkt.ip.IP) and isinstance(eth.data.data, dpkt.udp.UDP):
                    ip = eth.data
                    udp = ip.data
                    payload = bytes(udp.data)
                    if payload:  # Only send if there’s a payload
                        print(f"Sending {len(payload)} bytes at {timestamp}: {payload.hex()}")
                        sock.sendto(payload, (target_ip, target_port))
                        time.sleep(interval)  # Control the sending rate
                    else:
                        print(f"Skipping packet with no payload at {timestamp}")
                else:
                    print(f"Skipping non-UDP packet at {timestamp}: {eth}")

            print("Finished forwarding packets.")
        except Exception as e:
            print(f"Error: {e}")
        finally:
            sock.close()


if __name__ == "__main__":
    # Example usage
    pcap_file = "100kpacket.pcap"  # Replace with your .pcap file path
    forward_pcap_to_udp(pcap_file, target_ip="127.0.0.1", target_port=9000, interval=0.1)
