#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

START_TEST(test_input_security_boundary)
{
    // Invariant: The program must not crash or exhibit undefined behavior when processing adversarial input
    const char *payloads[] = {
        "\0",                     // Exact exploit case - null byte injection
        "A" * 1024,              // Boundary case - large input
        "normal_input",          // Valid input
        "\xFF\xFE\xFD",          // Binary data
        "'; DROP TABLE users;--" // SQL injection attempt
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process: execute the actual program with adversarial input
            close(STDIN_FILENO);
            dup2(STDOUT_FILENO, STDERR_FILENO); // Redirect stderr to stdout
            
            // Write payload to stdin via pipe
            int pipefd[2];
            pipe(pipefd);
            dup2(pipefd[0], STDIN_FILENO);
            write(pipefd[1], payloads[i], strlen(payloads[i]));
            close(pipefd[1]);
            
            // Execute the actual vulnerable program
            execl("./np/heavy/games3d/demo00/main", "main", NULL);
            _exit(EXIT_FAILURE); // Should not reach here
        } else {
            // Parent process: wait with timeout
            int status;
            waitpid(pid, &status, 0);
            
            // Property: Must not crash (no segmentation fault, bus error, etc.)
            ck_assert_msg(!WIFSIGNALED(status), 
                         "Program crashed with signal %d on payload %d", 
                         WTERMSIG(status), i);
            
            // Property: Must exit cleanly (even if with error code)
            ck_assert_msg(WIFEXITED(status), 
                         "Program did not exit normally on payload %d", i);
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_input_security_boundary);
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