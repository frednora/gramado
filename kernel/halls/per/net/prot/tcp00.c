#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Assume these macros and types are defined elsewhere.
#define TCP_HEADER_LENGTH 20  // Adjust if you support TCP options.
#define PAYLOAD_BUFFER_SIZE 1022
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20

// Dummy definitions for network <-> host order conversions.
uint16_t FromNetByteOrder16(uint16_t net16) {
    // Replace with actual conversion if needed.
    return ((net16 >> 8) & 0xff) | ((net16 & 0xff) << 8);
}
uint32_t FromNetByteOrder32(uint32_t net32) {
    // Replace with actual conversion if needed.
    return ((net32 >> 24) & 0xff)       |
           ((net32 >> 8)  & 0xff00)     |
           ((net32 << 8)  & 0xff0000)   |
           ((net32 << 24) & 0xff000000);
}

// Dummy types; define according to your project.
typedef unsigned int tcp_seq;
typedef unsigned int tcp_ack;

struct tcp_d {
    uint16_t th_sport;
    uint16_t th_dport;
    uint32_t th_seq;
    uint32_t th_ack;
    uint16_t do_res_flags; // Data offset + reserved bits + flags.
    // ... more fields if needed.
};

// Global buffer for the TCP payload.
char tcp_payload[PAYLOAD_BUFFER_SIZE];

//
// When receiving a TCP frame from the NIC device.
//
void network_handle_tcp(const unsigned char *buffer, ssize_t size)
{
    // Ensure non-null buffer and that we have at least the TCP header.
    if (buffer == NULL) {
        printk("network_handle_tcp: NULL buffer\n");
        return;
    }
    if (size < TCP_HEADER_LENGTH) {
        printk("network_handle_tcp: size (%zd) less than TCP header length\n", size);
        return;
    }

    // Cast the buffer to the TCP header structure.
    struct tcp_d *tcp = (struct tcp_d *) buffer;

    // Convert source and destination ports from network to host order.
    uint16_t sport = FromNetByteOrder16(tcp->th_sport);
    uint16_t dport = FromNetByteOrder16(tcp->th_dport);

    // Convert sequence and acknowledgment numbers.
    tcp_seq seq_number = FromNetByteOrder32(tcp->th_seq);
    tcp_ack ack_number = FromNetByteOrder32(tcp->th_ack);

    // Clear our payload buffer.
    memset(tcp_payload, 0, sizeof(tcp_payload));

    // Assume a fixed header length.
    // Note: In a real implementation, the data offset field within the TCP header
    // should be used to determine the true header length.
    size_t header_len = TCP_HEADER_LENGTH;

    // Determine available payload length.
    size_t payload_length = (size > header_len) ? (size - header_len) : 0;
    size_t copy_length = (payload_length < (PAYLOAD_BUFFER_SIZE - 1)) ?
                           payload_length : (PAYLOAD_BUFFER_SIZE - 1);

    // Copy the TCP payload; use memcpy for binary data.
    memcpy(tcp_payload, buffer + header_len, copy_length);
    tcp_payload[copy_length] = '\0'; // Ensure NUL-termination

    // Get and convert TCP flags.
    uint16_t flags = FromNetByteOrder16(tcp->do_res_flags);

    // Extract individual flags.
    int fFIN  = (flags & TH_FIN)  ? 1 : 0;
    int fSYN  = (flags & TH_SYN)  ? 1 : 0;
    int fRST  = (flags & TH_RST)  ? 1 : 0;
    int fPUSH = (flags & TH_PUSH) ? 1 : 0;
    int fACK  = (flags & TH_ACK)  ? 1 : 0;
    int fURG  = (flags & TH_URG)  ? 1 : 0;

    // Debug printing for known ports.
    if (dport == 80 || dport == 443) {
        printk("TCP: dport{%d}   #debug\n", dport);
    }

    /* SPECIAL HANDLING FOR PORT 11888 */
    if (dport == 11888) {
        printk("------------------------\n");
        printk("---- [11888] << TCP ----\n");
        printk("SYN=%d ACK=%d\n", fSYN, fACK);

        // 1. SYN: Client initiates connection.
        if (fSYN && !fACK) {
            printk("\n<<<< [TCP] SYN (1)\n");
            printk("SEQ=%u | ACK=%u\n", seq_number, ack_number);
            // TODO: Handshake initiation handling.
            return;
        }
        // 2. SYN/ACK: Server accepted the connection.
        else if (fSYN && fACK) {
            printk("\n<<<< [TCP] SYN/ACK (2)\n");
            printk("SEQ=%u | ACK=%u\n", seq_number, ack_number);
            // TODO: Process response of our SYN.
            return;
        }
        // 3. ACK: Client acknowledges connection acceptance.
        else if (!fSYN && fACK) {
            printk("\n<<<< [TCP] ACK (3)\n");
            printk("SEQ=%u | ACK=%u\n", seq_number, ack_number);
            // TODO: Process ACK response.
            return;
        }
    }

    // Optionally, for other ports, you might process or print the payload.
    // Uncomment the next lines to show the message:
    // printk("TCP: MESSAGE: {%s}\n", tcp_payload);
    // memset(tcp_payload, 0, sizeof(tcp_payload));

    // Drop any packets that do not match explicit processing rules.
    return;
}
