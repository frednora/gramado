#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>  // For sleep() on POSIX systems.
#endif

/* Definitions that should come from your header files. */
#define ARP_TABLE_COUNT_MAX   32
#define ARP_ENTRY_TIMEOUT     60  /* Timeout in seconds. */
#define TRUE                  1
#define FALSE                 0

/* Define your ARP cache item structure, augmented with a last_updated field
 * for aging. */
typedef struct arp_cache_item_d {
    int      used;
    int      magic;
    int      id;  // Table index.
    uint8_t  ipv4_address[4];
    uint8_t  mac_address[6];
    time_t   last_updated;  /* Timestamp of the last update. */
} arp_cache_item_t;

typedef struct arp_table_d {
    int               initialized;
    arp_cache_item_t  arpTable[ARP_TABLE_COUNT_MAX];
} arp_table_t;

/* Assuming ARP_Table is the global ARP cache table instance. */
arp_table_t ARP_Table;


/* ---------------- ARP Cache Worker Functions ---------------- */

/* Lookup an entry by IPv4 address. Returns a pointer to the cache item or NULL. */
arp_cache_item_t* arp_cache_lookup_by_ip(const uint8_t ipv4_address[4]) {
    if (!ARP_Table.initialized)
        return NULL;
    for (int i = 0; i < ARP_TABLE_COUNT_MAX; i++) {
        if (ARP_Table.arpTable[i].used) {
            if (memcmp(ARP_Table.arpTable[i].ipv4_address, ipv4_address,
                       sizeof(ARP_Table.arpTable[i].ipv4_address)) == 0)
                return &ARP_Table.arpTable[i];
        }
    }
    return NULL;
}

/* Lookup an entry by MAC address. Returns a pointer to the cache item or NULL. */
arp_cache_item_t* arp_cache_lookup_by_mac(const uint8_t mac_address[6]) {
    if (!ARP_Table.initialized)
        return NULL;
    for (int i = 0; i < ARP_TABLE_COUNT_MAX; i++) {
        if (ARP_Table.arpTable[i].used) {
            if (memcmp(ARP_Table.arpTable[i].mac_address, mac_address,
                       sizeof(ARP_Table.arpTable[i].mac_address)) == 0)
                return &ARP_Table.arpTable[i];
        }
    }
    return NULL;
}

/* Insert or update an ARP cache entry.
 * If an entry for the given IP exists, update its MAC and timestamp.
 * If not, insert the new entry into an empty slot.
 * If the table is full, replace the oldest entry.
 * Returns the table index on success, or -1 on failure. */
int arp_cache_insert(const uint8_t ipv4_address[4], const uint8_t mac_address[6]) {
    if (!ARP_Table.initialized)
        return -1;

    time_t now = time(NULL);
    arp_cache_item_t *entry = arp_cache_lookup_by_ip(ipv4_address);
    if (entry != NULL) {
        /* Update existing entry. */
        memcpy(entry->mac_address, mac_address, sizeof(entry->mac_address));
        entry->last_updated = now;
        return entry->id;
    }

    /* Look for an unused slot. */
    for (int i = 0; i < ARP_TABLE_COUNT_MAX; i++) {
        if (!ARP_Table.arpTable[i].used) {
            ARP_Table.arpTable[i].id = i;
            memcpy(ARP_Table.arpTable[i].ipv4_address, ipv4_address,
                   sizeof(ARP_Table.arpTable[i].ipv4_address));
            memcpy(ARP_Table.arpTable[i].mac_address, mac_address,
                   sizeof(ARP_Table.arpTable[i].mac_address));
            ARP_Table.arpTable[i].used = TRUE;
            ARP_Table.arpTable[i].magic = 1234;
            ARP_Table.arpTable[i].last_updated = now;
            return i;
        }
    }

    /* If no free slot is available, replace the oldest entry. */
    int oldest_index = 0;
    time_t oldest_time = ARP_Table.arpTable[0].last_updated;
    for (int i = 1; i < ARP_TABLE_COUNT_MAX; i++) {
        if (ARP_Table.arpTable[i].last_updated < oldest_time) {
            oldest_time = ARP_Table.arpTable[i].last_updated;
            oldest_index = i;
        }
    }
    ARP_Table.arpTable[oldest_index].id = oldest_index;
    memcpy(ARP_Table.arpTable[oldest_index].ipv4_address, ipv4_address,
           sizeof(ARP_Table.arpTable[oldest_index].ipv4_address));
    memcpy(ARP_Table.arpTable[oldest_index].mac_address, mac_address,
           sizeof(ARP_Table.arpTable[oldest_index].mac_address));
    ARP_Table.arpTable[oldest_index].last_updated = now;
    return oldest_index;
}

/* Remove an entry at the given index by marking it as unused. */
void arp_cache_remove(int index) {
    if (!ARP_Table.initialized)
        return;
    if (index < 0 || index >= ARP_TABLE_COUNT_MAX)
        return;
    ARP_Table.arpTable[index].used = FALSE;
}

/* Age out (remove) stale ARP cache entries.
 * This function checks each used entry, and if its age exceeds a defined timeout,
 * it marks the entry as unused. */
void arp_cache_age_entries(void) {
    if (!ARP_Table.initialized)
        return;
    time_t now = time(NULL);
    for (int i = 0; i < ARP_TABLE_COUNT_MAX; i++) {
        if (ARP_Table.arpTable[i].used) {
            if ((now - ARP_Table.arpTable[i].last_updated) > ARP_ENTRY_TIMEOUT) {
                printf("ARP: Removing aged entry at index %d\n", i);
                arp_cache_remove(i);
            }
        }
    }
}

/* A simple worker function that can be run
 * in its own thread or called periodically from a main loop.
 * This worker periodically checks and ages out stale ARP cache entries.
 */
void arp_cache_worker(void) {
    while (1) {
        arp_cache_age_entries();
#ifdef _WIN32
        Sleep(1000);  // Sleep for 1000 milliseconds on Windows.
#else
        sleep(1);     // Sleep for 1 second on POSIX systems.
#endif
    }
}

/* Optional: A helper function to print the ARP cache table for debugging. */
void arp_cache_print(void) {
    if (!ARP_Table.initialized)
        return;
    printf("\nARP Cache Table:\n");
    for (int i = 0; i < ARP_TABLE_COUNT_MAX; i++) {
        if (ARP_Table.arpTable[i].used) {
            printf("Index: %d, IP: %d.%d.%d.%d, MAC: %02x:%02x:%02x:%02x:%02x:%02x, Last Updated: %ld\n",
                ARP_Table.arpTable[i].id,
                ARP_Table.arpTable[i].ipv4_address[0],
                ARP_Table.arpTable[i].ipv4_address[1],
                ARP_Table.arpTable[i].ipv4_address[2],
                ARP_Table.arpTable[i].ipv4_address[3],
                ARP_Table.arpTable[i].mac_address[0],
                ARP_Table.arpTable[i].mac_address[1],
                ARP_Table.arpTable[i].mac_address[2],
                ARP_Table.arpTable[i].mac_address[3],
                ARP_Table.arpTable[i].mac_address[4],
                ARP_Table.arpTable[i].mac_address[5],
                ARP_Table.arpTable[i].last_updated);
        }
    }
}
