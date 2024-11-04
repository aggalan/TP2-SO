#include <stdio.h>
#include "../include/lib.h"
#include "../include/usr_sys_calls.h"

#define TEST_BUFFER_SIZE 1024
#define TEST_STRING "Hello through the pipe! Let's make this longer in order to test correctly!"
#define PIPE_NAME "test_pipe"
#define NUM_MESSAGES 5
#define READ 0
#define WRITE 1

// Process that writes to the pipe
uint64_t writer_process(uint64_t argc, char *argv[]) {
    if (argc != 1) return -1;

    // Open the pipe in write mode
    int fd = call_named_pipe_open(PIPE_NAME, WRITE);
    if (fd < 3) {
        print(0xFFFFFF, "Writer: Failed to open pipe in write mode\n");
        return -1;
    }

    // Write multiple messages
    char *messages[] = {
            "Message 0 from writer",
            "Message 1 from writer",
            "Message 2 from writer",
            "Message 3 from writer",
            "Message 4 from writer"
    };

    for (int i = 0; i < NUM_MESSAGES; i++) {
        ssize_t written = call_pipe_write(fd, messages[i], str_len(messages[i]) + 1);
        if (written != str_len(messages[i]) + 1) {
            print(0xFFFFFF, "Writer: Write failed\n");
            call_named_pipe_close(fd);
            return -1;
        }
        print(0xFFFFFF, "Writer: Sent: %s\n", messages[i]);
        call_nice();
    }

    call_named_pipe_close(fd);
    return 0;
}

// Process that reads from the pipe
uint64_t reader_process(uint64_t argc, char *argv[]) {
    if (argc != 1) return -1;

    // Open the pipe in read mode
    int fd = call_named_pipe_open(PIPE_NAME, READ);
    if (fd < 3) {
        print(0xFFFFFF, "Reader: Failed to open pipe in read mode\n");
        return -1;
    }

    char buffer[TEST_BUFFER_SIZE];
    for (int i = 0; i < NUM_MESSAGES; i++) {
        ssize_t bytes_read = call_pipe_read(fd, buffer, TEST_BUFFER_SIZE);
        if (bytes_read <= 0) {
            print(0xFFFFFF, "Reader: Read failed\n");
            call_named_pipe_close(fd);
            return -1;
        }
        print(0xFFFFFF, "Reader: Received: %s\n", buffer);
        call_nice();
    }

    call_named_pipe_close(fd);
    return 0;
}

void test_pipe_creation_and_modes() {
    print(0xFFFFFF, "Testing pipe creation and mode restrictions...\n");

    // Test basic pipe creation
    int fd = call_named_pipe_create(PIPE_NAME);
    if (fd < 3) {
        print(0xFFFFFF, "FAIL: Pipe creation failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Created pipe with fd: %d\n", fd);

    // Test duplicate pipe creation (should fail)
    int fd2 = call_named_pipe_create(PIPE_NAME);
    if (fd2 != -1) {
        print(0xFFFFFF, "FAIL: Duplicate pipe creation should have failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Duplicate pipe creation correctly failed\n");

    // Test single process trying to open both modes (should fail)
    int read_fd = call_named_pipe_open(PIPE_NAME, READ);
    if (read_fd < 3) {
        print(0xFFFFFF, "FAIL: Opening pipe in read mode failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Successfully opened pipe in read mode\n");

    // Try to open write mode from same process (should fail)
    int write_fd = call_named_pipe_open(PIPE_NAME, WRITE);
    if (write_fd != -1) {
        print(0xFFFFFF, "FAIL: Same process shouldn't be able to open both read and write modes\n");
        return;
    }
    print(0xFFFFFF, "PASS: Correctly prevented same process from opening both modes\n");

    // Test duplicate reader (should fail)
    int duplicate_read_fd = call_named_pipe_open(PIPE_NAME, READ);
    if (duplicate_read_fd != -1) {
        print(0xFFFFFF, "FAIL: Opening duplicate reader should have failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Duplicate reader correctly failed\n");

//    call_named_pipe_close(fd);
    call_named_pipe_close(read_fd);
    print(0xFFFFFF, "Pipe creation and mode tests passed!\n\n");
}

void test_pipe_concurrent_processes() {
    print(0xFFFFFF, "Testing pipe with concurrent processes...\n");

    // Create the pipe first
    int create_fd = call_named_pipe_create(PIPE_NAME);
    if (create_fd < 3) {
        print(0xFFFFFF, "FAIL: Failed to create pipe\n");
        return;
    }

    char *empty_args[] = {NULL};

    // Create reader and writer processes
    uint64_t reader_pid = call_create_process(reader_process, 1, 1, empty_args, 0);
    if (reader_pid == -1) {
        print(0xFFFFFF, "FAIL: Failed to create reader process\n");
        call_named_pipe_close(create_fd);
        return;
    }

    // Give reader a moment to start
    call_nice();

    uint64_t writer_pid = call_create_process(writer_process, 1, 1, empty_args, 0);
    if (writer_pid == -1) {
        print(0xFFFFFF, "FAIL: Failed to create writer process\n");
        call_named_pipe_close(create_fd);
        call_waitpid(reader_pid);
        return;
    }

    // Wait for both processes to complete
    call_waitpid(reader_pid);
    call_waitpid(writer_pid);

//    call_named_pipe_close(create_fd);
    print(0xFFFFFF, "PASS: Concurrent processes test completed\n\n");
}

void test_invalid_operations() {
    print(0xFFFFFF, "Testing invalid operations...\n");

    int fd = call_named_pipe_create(PIPE_NAME);
    if (fd < 3) {
        print(0xFFFFFF, "FAIL: Pipe creation failed\n");
        return;
    }

    // Open pipe in read mode
    int read_fd = call_named_pipe_open(PIPE_NAME, READ);
    if (read_fd < 3) {
        print(0xFFFFFF, "FAIL: Opening pipe in read mode failed\n");
        call_named_pipe_close(fd);
        return;
    }

    // Try to write to read fd (should fail)
    char buffer[] = "test";
    if (call_pipe_write(read_fd, buffer, str_len(buffer)) != -1) {
        print(0xFFFFFF, "FAIL: Writing to read fd should have failed\n");
        call_named_pipe_close(fd);
        call_named_pipe_close(read_fd);
        return;
    }
    print(0xFFFFFF, "PASS: Writing to read fd correctly failed\n");

//    call_named_pipe_close(fd);
    call_named_pipe_close(read_fd);
    print(0xFFFFFF, "Invalid operations tests passed!\n\n");
}

int main_test_pipes() {
    print(0xFFFFFF, "Starting named pipe tests...\n\n");

    test_pipe_creation_and_modes();
    test_invalid_operations();
    test_pipe_concurrent_processes();

    print(0xFFFFFF, "All tests completed!\n");
    return 0;
}