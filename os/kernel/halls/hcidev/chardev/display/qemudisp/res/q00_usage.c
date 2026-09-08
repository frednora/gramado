
qemudisp_initialize();

if (qemudisp_set_resolution(1024, 768, 32) == 0) {
    printk("Resolution changed!\n");
    // optionally force a full refresh
    bldisp_flush(0);
}

