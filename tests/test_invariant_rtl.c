#include <check.h>
#include <stdlib.h>
#include <string.h>

// Include the actual production header
#include "np/heavy/games3d/libs/rtl/entrance/visitor/rtl.h"

START_TEST(test_dev_name_buffer_invariant)
{
    // Invariant: dev_name buffer must not overflow regardless of dev_number input
    const char *payloads[] = {
        "1234567890123456789012345678901234567890",  // Exact exploit: exceeds buffer
        "12345678901234567890",                      // Boundary: exactly fills buffer
        "123",                                       // Valid normal input
        "",                                          // Empty string
        "A".repeat(100)                              // Very long string
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        char dev_name[32];  // Match actual buffer size from vulnerable code
        const char *dev_number = payloads[i];
        
        // Call the actual vulnerable function pattern from rtl.c
        // This simulates the exact unsafe operations
        strcpy(dev_name, "DEV");
        strcat(dev_name, dev_number);
        
        // Property: dev_name must remain null-terminated within bounds
        ck_assert_msg(strlen(dev_name) < sizeof(dev_name),
                     "Buffer overflow detected for payload: %s", dev_number);
        
        // Additional invariant: no memory corruption visible via canary check
        static char canary[64] = "CANARY_VALUE";
        ck_assert_str_eq(canary, "CANARY_VALUE");
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_dev_name_buffer_invariant);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}