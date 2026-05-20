

#ifndef ____ETHERNET_H
#define ____ETHERNET_H



//See: https://en.wikipedia.org/wiki/EtherType

/*
EtherType values for some notable protocols[8] EtherType 	Protocol
0x0800 	Internet Protocol version 4 (IPv4)
0x0806 	Address Resolution Protocol (ARP)
0x0842 	Wake-on-LAN[9]
0x22F0 	Audio Video Transport Protocol (AVTP)
0x22F3 	IETF TRILL Protocol
0x22EA 	Stream Reservation Protocol
0x6002 	DEC MOP RC
0x6003 	DECnet Phase IV, DNA Routing
0x6004 	DEC LAT
0x8035 	Reverse Address Resolution Protocol (RARP)
0x809B 	AppleTalk (Ethertalk)
0x80F3 	AppleTalk Address Resolution Protocol (AARP)
0x8100 	VLAN-tagged frame (IEEE 802.1Q) and Shortest Path Bridging IEEE 802.1aq with NNI compatibility[10]
0x8102 	Simple Loop Prevention Protocol (SLPP)
0x8137 	IPX
0x8204 	QNX Qnet
0x86DD 	Internet Protocol Version 6 (IPv6)
0x8808 	Ethernet flow control
0x8809 	Ethernet Slow Protocols[11] such as the Link Aggregation Control Protocol
0x8819 	CobraNet
0x8847 	MPLS unicast
0x8848 	MPLS multicast
0x8863 	PPPoE Discovery Stage
0x8864 	PPPoE Session Stage
0x886D 	Intel Advanced Networking Services [12]
0x8870 	Jumbo Frames (Obsoleted draft-ietf-isis-ext-eth-01)
0x887B 	HomePlug 1.0 MME
0x888E 	EAP over LAN (IEEE 802.1X)
0x8892 	PROFINET Protocol
0x889A 	HyperSCSI (SCSI over Ethernet)
0x88A2 	ATA over Ethernet
0x88A4 	EtherCAT Protocol
0x88A8 	Provider Bridging (IEEE 802.1ad) & Shortest Path Bridging IEEE 802.1aq[10]
0x88AB 	Ethernet Powerlink[citation needed]
0x88B8 	GOOSE (Generic Object Oriented Substation event)
0x88B9 	GSE (Generic Substation Events) Management Services
0x88BA 	SV (Sampled Value Transmission)
0x88CC 	Link Layer Discovery Protocol (LLDP)
0x88CD 	SERCOS III
0x88DC 	WSMP, WAVE Short Message Protocol
0x88E1 	HomePlug AV MME[citation needed]
0x88E3 	Media Redundancy Protocol (IEC62439-2)
0x88E5 	MAC security (IEEE 802.1AE)
0x88E7 	Provider Backbone Bridges (PBB) (IEEE 802.1ah)
0x88F7 	Precision Time Protocol (PTP) over Ethernet (IEEE 1588)
0x88F8 	NC-SI
0x88FB 	Parallel Redundancy Protocol (PRP)
0x8902 	IEEE 802.1ag Connectivity Fault Management (CFM) Protocol / ITU-T Recommendation Y.1731 (OAM)
0x8906 	Fibre Channel over Ethernet (FCoE)
0x8914 	FCoE Initialization Protocol
0x8915 	RDMA over Converged Ethernet (RoCE)
0x891D 	TTEthernet Protocol Control Frame (TTE)
0x892F 	High-availability Seamless Redundancy (HSR)
0x9000 	Ethernet Configuration Testing Protocol[13]
0x9100 	VLAN-tagged (IEEE 802.1Q) frame with double tagging
0xCAFE 	Veritas Technologies Low Latency Transport (LLT) 
*/




// Ethernet header length
#define ETHERNET_HEADER_LENGHT 14      

// ethernet header
struct ether_header {
	
	uint8_t dst[6];
	uint8_t src[6];
	uint16_t type;
	
} __attribute__((packed)); 



#endif    


