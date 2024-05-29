#include "queue.h"
#include "lib.h"
#include "protocols.h"
#include <string.h>
#include <arpa/inet.h>


/* Routing table */
struct route_table_entry *rtable;
int rtable_len;

/* Mac table */
struct arp_table_entry *arp_table;
int arp_table_len;

int compare_entries(const void* a, const void* b) {
	// Sort by increasing masked prefix
    if (((struct route_table_entry *)a)->mask != ((struct route_table_entry *)b)->mask) {
        return ((struct route_table_entry *)b)->mask - ((struct route_table_entry *)a)->mask;
    }
    // If masks are equal, sort by increasing prefix to ensure correct binary search
    return ((struct route_table_entry *)a)->prefix - ((struct route_table_entry *)b)->prefix;
}

struct route_table_entry *get_best_route(uint32_t ip_dest) {

    int left = 0;
    int right = rtable_len - 1;

	/* Binary Search */
    while (left <= right) {
        int mid = left + (right - left) / 2;
        uint32_t masked_ip = ip_dest & rtable[mid].mask;
        uint32_t masked_prefix = rtable[mid].prefix & rtable[mid].mask;

        if (masked_ip == masked_prefix) {
			return &rtable[mid];
        } else if (masked_ip > masked_prefix) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return NULL;
}

struct arp_table_entry *get_mac_entry(uint32_t ip_dest) {

	/* Find MAC in arp table */
	for(int i = 0; i < arp_table_len; i++) {
		if(arp_table[i].ip == ip_dest)
			return &arp_table[i];
	}

	return NULL;
}

void send_icmp_error(char *buf, int interface, uint8_t type, uint8_t code) {

	struct ether_header *eth_old = (struct ether_header *)buf;
	struct ether_header *eth_new = (struct ether_header *)malloc(sizeof(struct ether_header));

	/* Build a new packet with destination and source switched */
	memcpy(eth_new->ether_dhost, eth_old->ether_shost, sizeof(uint8_t) * 6);
	memcpy(eth_new->ether_shost, eth_old->ether_dhost, sizeof(uint8_t) * 6);
	eth_new->ether_type = eth_old->ether_type;

	struct iphdr *ip_old = (struct iphdr *)(buf + sizeof(struct ether_header));
	struct iphdr *ip_new = (struct iphdr *)malloc(sizeof(struct iphdr));
	struct icmphdr *icmp_old = (struct icmphdr *)(buf + sizeof(struct ether_header) + sizeof(struct iphdr));

	ip_new->saddr = ip_old->daddr;
	ip_new->daddr = ip_old->saddr;
	ip_new->tos = 0;
	ip_new->frag_off = 0;
	ip_new->version = 4;
	ip_new->ihl = 5;
	ip_new->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
	ip_new->id = htons(1);
	ip_new->protocol = 0x01;
	ip_new->check = 0;
	ip_new->check = htons(checksum((uint16_t *)ip_new, sizeof(struct iphdr)));

	struct icmphdr *icmp_hdr = (struct icmphdr *)malloc(sizeof(struct icmphdr));

	icmp_hdr->type = type;
	icmp_hdr->code = code;
	icmp_hdr->checksum = 0;
	icmp_hdr->checksum = htons(checksum((uint16_t *)icmp_hdr, sizeof(struct icmphdr)));

	char packet[MAX_PACKET_LEN];

	memcpy(packet, eth_new, sizeof(struct ether_header));
	memcpy(packet + sizeof(struct ether_header), ip_new, sizeof(struct iphdr));
	memcpy(packet + sizeof(struct ether_header) + sizeof(struct iphdr), 
						icmp_hdr, sizeof(struct icmphdr));
	memcpy(packet + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct icmphdr), 
						ip_old, sizeof(struct iphdr));
	memcpy(packet + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct iphdr) + sizeof(struct icmphdr), 
						icmp_old, sizeof(struct icmphdr));

	/* Send packet */
	send_to_link(interface, packet, sizeof(struct ether_header) + 2 * sizeof(struct iphdr) + 2 * sizeof(struct icmphdr));
}


int main(int argc, char *argv[])
{
	char buf[MAX_PACKET_LEN];

	// Do not modify this line
	init(argc - 2, argv + 2);

	rtable = malloc(sizeof(struct route_table_entry) * 65000);
	DIE(rtable == NULL, "error routing table");

	arp_table = malloc(sizeof(struct  arp_table_entry) * 10);
	DIE(arp_table == NULL, "error arp table");

	rtable_len = read_rtable(argv[1], rtable);
	arp_table_len = parse_arp_table("arp_table.txt", arp_table);

	qsort(rtable, rtable_len, sizeof(struct route_table_entry), compare_entries);


	while (1) {

		int interface;
		size_t len;

		interface = recv_from_any_link(buf, &len);
		DIE(interface < 0, "recv_from_any_links");

		struct ether_header *eth_hdr = (struct ether_header *) buf;
		/* Note that packets received are in network order,
		any header field which has more than 1 byte will need to be conerted to
		host order. For example, ntohs(eth_hdr->ether_type). The oposite is needed when
		sending a packet on the link, */

		struct iphdr *ip_hdr = (struct iphdr *)(buf + sizeof(struct ether_header));

		if(checksum((uint16_t *)ip_hdr, sizeof(struct iphdr)) != 0) {
			/* Checksum error */
			continue;
		}

		if(ip_hdr->daddr == inet_addr(get_interface_ip(interface))) {
			/* Packet is for me */
			if(ip_hdr->protocol == 0x01) {
				/* ICMP packet */
				struct icmphdr *icmp_current = (struct icmphdr *)(buf + sizeof(struct ether_header) + sizeof(struct iphdr));

				if(icmp_current->type == 8) {
					/* ICMP echo request */
					/* Send ICMP echo reply */

					send_icmp_error(buf, interface, 0, 0);

					continue;
				}
			}
		}
		/* Call get_best_route to find the most specific route, continue; (drop) if null */
		struct route_table_entry *best_router = get_best_route(ip_hdr->daddr);

		if(best_router == NULL) {
			/* No route found */
			/* Send ICMP Destination Unreachable */
			fprintf(stderr, "No route found\n");

			send_icmp_error(buf, interface, 3, 0);

			continue;
		}


		/* Check TTL >= 1. Update TLL. Update checksum using the incremental forumla  */
		if(ip_hdr->ttl <= 1) {
			/* TTL expired */

			send_icmp_error(buf, interface, 11, 0);

			continue;
		}

		ip_hdr->ttl--;
		ip_hdr->check = 0;
		ip_hdr->check = htons(checksum((uint16_t *)ip_hdr, sizeof(struct iphdr)));

		/* Call get_mac_entry to find the MAC address of the next hop, continue; (drop) if null */
		struct arp_table_entry *nexthop_mac = get_mac_entry(best_router->next_hop);
		if (nexthop_mac == NULL)
		{
			/* Mac not found */
			continue;
		}

		memcpy(eth_hdr->ether_dhost, nexthop_mac->mac, sizeof(eth_hdr->ether_dhost));
		get_interface_mac(best_router->interface, eth_hdr->ether_shost);
		  
		/* Send packet */
		send_to_link(best_router->interface, buf, len);

	}
}

