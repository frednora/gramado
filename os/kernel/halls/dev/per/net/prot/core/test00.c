// This function handles the ipv4 protocol
// It was created by Copilot.

void network_handle_ipv4(const unsigned char *buffer, ssize_t size) {
    if (buffer == NULL) {
        printk("network_handle_ipv4: Invalid buffer\n");
        return;
    }
    
    if (size < IP_HEADER_LENGHT) {
        printk("network_handle_ipv4: Invalid size\n");
        return;
    }

    struct ip_d *ip = (struct ip_d *) buffer;
    if (ip == NULL) {
        printk("network_handle_ipv4: IP header is NULL\n");
        return;
    }

    // Extract IP version and header length
    uint8_t v_hl = ip->v_hl;
    uint8_t version = (v_hl >> 4) & 0x0F;
    uint8_t header_length = (v_hl & 0x0F) * 4;  // Header length in bytes

    if (version != 4) {
        printk("network_handle_ipv4: Not IPv4\n");
        return;
    }

    uint16_t total_length = FromNetByteOrder16(ip->ip_len);
    if (total_length < 20 || total_length > 65535) {
        printk("network_handle_ipv4: Invalid total length\n");
        return;
    }

    unsigned char *payload_pointer = (unsigned char *)(buffer + header_length);
    ssize_t payload_size = total_length - header_length;

    uint8_t protocol = ip->ip_p;
    switch (protocol) {
        case PROTOCOL_IP_UDP:
            network_handle_udp(payload_pointer, payload_size);
            break;
        
        case PROTOCOL_IP_ICMP:
            // network_handle_icmp(payload_pointer, payload_size);
            printk("network_handle_ipv4: ICMP Protocol not implemented\n");
            break;
        
        case PROTOCOL_IP_TCP:
            network_handle_tcp(payload_pointer, payload_size);
            break;
        
        default:
            printk("network_handle_ipv4: Unsupported protocol\n");
            break;
    }
}

