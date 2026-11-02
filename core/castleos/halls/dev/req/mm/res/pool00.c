// Some worker that potencially can be used in mmpool.c and mm.c
// Created by Grok/X

// =====================================================
//  Page Object Creation (Recommended new helper)
// =====================================================

static struct page_d* page_create(int slot)
{
    struct page_d *page;

    if (slot < 0 || slot >= PAGE_COUNT_MAX)
        return NULL;

    page = (struct page_d *) kmalloc(sizeof(struct page_d));
    if (page == NULL)
        return NULL;

    memset(page, 0, sizeof(struct page_d));

    page->id                = slot;
    page->used              = TRUE;
    page->free              = FALSE;
    page->locked            = FALSE;
    page->magic             = 1234;
    page->ref_count         = 1;
    page->next              = NULL;
    page->absolute_frame_number = 0;

    return page;
}

// =====================================================
//  Improved __pageObject (replace your old one)
// =====================================================

static struct page_d* __pageObject(void)
{
    for (int slot = 0; slot < PAGE_COUNT_MAX; slot++)
    {
        if (pageAllocList[slot] == 0)
        {
            struct page_d *page = page_create(slot);
            if (page == NULL)
                return NULL;

            pageAllocList[slot] = (unsigned long) page;
            return page;
        }
    }

    return NULL;  // Pool exhausted
}

// ============================================

// =====================================================
//  Free / Release functions
// =====================================================

void mmFreePages(void *ptr, size_t size)
{
    unsigned long base = (unsigned long)g_pagedpool_va;
    unsigned long va = (unsigned long)ptr;
    int start_slot;

    if (ptr == NULL || size == 0 || base == 0)
        return;

    if (va < base)
        return;

    start_slot = (int)((va - base) / PAGE_SIZE);

    if (start_slot < 0 || (start_slot + size) > PAGE_COUNT_MAX)
        return;

    for (int i = 0; i < size; i++)
    {
        int slot = start_slot + i;
        struct page_d *page = (struct page_d *) pageAllocList[slot];

        if (page && page->magic == 1234)
        {
            page->ref_count--;

            if (page->ref_count <= 0)
            {
                pageAllocList[slot] = 0;
                page->used = FALSE;
                page->free = TRUE;
                page->locked = FALSE;
                // TODO: kfree(page) when you have safe object lifetime management
            }
        }
    }
}

// Release one page (convenience)
void mmFreePage(void *ptr)
{
    mmFreePages(ptr, 1);
}

/////////////


// =====================================================
//  Statistics and Debug
// =====================================================

int mmPoolGetUsedPages(void)
{
    int used = 0;
    for (int i = 0; i < PAGE_COUNT_MAX; i++)
    {
        if (pageAllocList[i] != 0)
            used++;
    }
    return used;
}

int mmPoolGetFreePages(void)
{
    return PAGE_COUNT_MAX - mmPoolGetUsedPages();
}

// Returns size of largest contiguous free block
int mmPoolGetLargestFreeBlock(void)
{
    int max_block = 0;
    int current = 0;

    for (int i = 0; i < PAGE_COUNT_MAX; i++)
    {
        if (pageAllocList[i] == 0)
        {
            current++;
            if (current > max_block)
                max_block = current;
        }
        else
        {
            current = 0;
        }
    }
    return max_block;
}

void mmPoolPrintStatus(void)
{
    int used = mmPoolGetUsedPages();
    int free = PAGE_COUNT_MAX - used;
    int largest = mmPoolGetLargestFreeBlock();

    printk("Paged Pool Status:\n");
    printk("  Total pages : %d\n", PAGE_COUNT_MAX);
    printk("  Used pages  : %d\n", used);
    printk("  Free pages  : %d\n", free);
    printk("  Largest free: %d pages\n", largest);
    printk("  Usage       : %d%%\n", (used * 100) / PAGE_COUNT_MAX);
}


////////////////////////////////

// Better initialization
void initializeFramesAlloc(void)
{
    for (int i = 0; i < PAGE_COUNT_MAX; i++)
        pageAllocList[i] = 0;

    // Optional: pre-allocate a few pages for critical early use
    // mmAllocPages(4); etc.
}

// Integrity check (useful for debugging)
bool mmPoolIntegrityCheck(void)
{
    for (int i = 0; i < PAGE_COUNT_MAX; i++)
    {
        struct page_d *p = (struct page_d *) pageAllocList[i];

        if (p != NULL)
        {
            if (p->magic != 1234 || p->id != i)
                return false;
        }
    }
    return true;
}

///////////////////////////

// ================================================
//  Global Memory Status
// ================================================

void mmShowMemoryInfo(void)
{
    printk("\n=== Memory Manager Status ===\n");
    printk("Kernel Heap:\n");
    printk("  Start:     0x%X\n", kernel_heap_start);
    printk("  End:       0x%X\n", kernel_heap_end);
    printk("  Pointer:   0x%X\n", g_heap_pointer);
    printk("  Available: %d KB\n", g_available_heap / 1024);
    printk("  mmblocks:  %d / %d\n", mmblockCount, MMBLOCK_COUNT_MAX);

    printk("Paged Pool:\n");
    printk("  Base VA:   0x%X\n", g_pagedpool_va);
    printk("  Total:     %d pages (%d KB)\n", 
           PAGE_COUNT_MAX, (PAGE_COUNT_MAX * PAGE_SIZE)/1024);

    int used = mmPoolGetUsedPages();  // from mmpool.c
    printk("  Used:      %d pages\n", used);
    printk("  Free:      %d pages\n", PAGE_COUNT_MAX - used);
    printk("  Largest free block: %d pages\n", mmPoolGetLargestFreeBlock());

    printk("Kernel Stack:\n");
    printk("  Start: 0x%X\n", kernel_stack_start);
    printk("  End:   0x%X\n", kernel_stack_end);

    // TODO: Physical memory, zones, etc.
    printk("=============================\n");
}

///////////////////////////////

// Allocate from paged pool with zeroing option
void *mmAllocSharedPages(size_t npages, int zero)
{
    if (npages == 0)
        return NULL;

    void *ptr = mmAllocPages(npages);   // your current function

    if (ptr && zero)
        memset(ptr, 0, npages * PAGE_SIZE);

    return ptr;
}

// Allocate one page from paged pool (convenience)
void *mmAllocSharedPage(void)
{
    return mmAllocPages(1);
}

// Allocate from kernel heap with zeroing
void *kmallocz(size_t size)
{
    void *ptr = kmalloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

//////////////////////////


// Free pages from paged pool
void mmFreeSharedPages(void *ptr, size_t npages)
{
    if (ptr == NULL || npages == 0)
        return;
    mmFreePages(ptr, npages);   // the function we created earlier
}

// Try to reuse from heap, fallback to normal allocation
void *kmalloc_reuse(size_t size)
{
    void *ptr = (void*) heapReuseMemory(size, TRUE);
    if (ptr)
        return ptr;

    return kmalloc(size);   // fallback
}

// Soft free (mark as reusable) + optional zero
void kfree_ex(void *ptr, int zero)
{
    if (ptr == NULL)
        return;

    // Check if it's from paged pool (optional heuristic)
    if ((unsigned long)ptr >= g_pagedpool_va && 
        (unsigned long)ptr < g_pagedpool_va + (PAGE_COUNT_MAX*PAGE_SIZE))
    {
        // For now we need size to free from pool
        // TODO: Add size tracking in page_d if needed
        return;
    }

    heapFreeMemory(ptr);

    if (zero)
        memset(ptr, 0, 64);  // optional small clear
}

///////////////////////////////


void mmIntegrityCheck(void)
{
    if (!mmPoolIntegrityCheck())
        printk("WARNING: Paged Pool integrity check failed!\n");

    // TODO: Check heap blocks, stack canary, etc.
}

void mmDumpPagedPool(void)
{
    printk("Paged Pool dump (first 32 slots):\n");
    for (int i = 0; i < 32 && i < PAGE_COUNT_MAX; i++)
    {
        struct page_d *p = (struct page_d *) pageAllocList[i];
        if (p)
            printk(" [%d] id=%d ref=%d used=%d\n", 
                   i, p->id, p->ref_count, p->used);
        else
            printk(" [%d] FREE\n", i);
    }
}


///////////////////////////////////////


static void mmInitializePhase0(void)
{
    wink_initialize_video();
    __init_kernel_heap();
    __init_kernel_stack();
    mmsize_initialize();

    // Clear lists
    for (int i = 0; i < MMBLOCK_COUNT_MAX; i++)
        mmblockList[i] = 0;
}

static void mmInitializePhase1(void)
{
    initializeFramesAlloc();           // paged pool
    pagesInitializePaging();           // your paging init
}

// Main entry point
int mmInitialize(int phase)
{
    if (phase == 0)
    {
        mmInitializePhase0();
        return TRUE;
    }
    else if (phase == 1)
    {
        mmInitializePhase1();
        mmShowMemoryInfo();            // optional
        return TRUE;
    }

    return FALSE;
}

/////////////////////////////

