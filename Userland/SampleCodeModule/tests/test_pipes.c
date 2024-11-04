#include <stdio.h>
#include "../include/lib.h"
#include "../include/usr_sys_calls.h"

#define TEST_BUFFER_SIZE 1024
#define TEST_STRING "Hello through the pipe! Let's make this longer in order to test correctly!"
#define PIPE_NAME "test_pipe"
#define NUM_MESSAGES 5
#define WRITE 1
#define READ 0

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

void test_pipe_creation() {
    print(0xFFFFFF, "Testing pipe creation...\n");

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

    // Test opening pipe in read mode
    int read_fd = call_named_pipe_open(PIPE_NAME, READ);
    if (read_fd < 3) {
        print(0xFFFFFF, "FAIL: Opening pipe in read mode failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Successfully opened pipe in read mode\n");

    // Test opening pipe in write mode
    int write_fd = call_named_pipe_open(PIPE_NAME, WRITE);
    if (write_fd < 3) {
        print(0xFFFFFF, "FAIL: Opening pipe in write mode failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Successfully opened pipe in write mode\n");

    // Test duplicate reader (should fail)
    int duplicate_read_fd = call_named_pipe_open(PIPE_NAME, READ);
    if (duplicate_read_fd != -1) {
        print(0xFFFFFF, "FAIL: Opening duplicate reader should have failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Duplicate reader correctly failed\n");

//    call_named_pipe_close(fd);
    call_named_pipe_close(read_fd);
    call_named_pipe_close(write_fd);
    print(0xFFFFFF, "Pipe creation tests passed!\n\n");
}

void test_pipe_io() {
    print(0xFFFFFF, "Testing pipe I/O...\n");

    // Create the pipe
    int create_fd = call_named_pipe_create(PIPE_NAME);
    if (create_fd < 3) {
        print(0xFFFFFF, "FAIL: Pipe creation failed\n");
        return;
    }

    // Open for reading and writing
    int read_fd = call_named_pipe_open(PIPE_NAME, READ);
    int write_fd = call_named_pipe_open(PIPE_NAME, WRITE);
    if (read_fd < 3 || write_fd < 3) {
        print(0xFFFFFF, "FAIL: Opening pipe failed\n");
        return;
    }

    char write_buffer[TEST_BUFFER_SIZE];
    char read_buffer[TEST_BUFFER_SIZE];
    str_cpy(write_buffer, TEST_STRING);

    // Test writing
    ssize_t bytes_written = call_pipe_write(write_fd, write_buffer, str_len(TEST_STRING) + 1);
    if (bytes_written != str_len(TEST_STRING) + 1) {
        print(0xFFFFFF, "FAIL: Write operation failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Written %d bytes to pipe\n", bytes_written);

    // Test reading
    ssize_t bytes_read = call_pipe_read(read_fd, read_buffer, TEST_BUFFER_SIZE);
    if (bytes_read != str_len(TEST_STRING) + 1) {
        print(0xFFFFFF, "FAIL: Read operation failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Read %d bytes from pipe\n", bytes_read);

    // Verify data
    if (str_cmp(write_buffer, read_buffer) != 0) {
        print(0xFFFFFF, "FAIL: Data mismatch\n");
        return;
    }
    print(0xFFFFFF, "PASS: Data verification passed\n");

    // Test writing to read fd (should fail)
    if (call_pipe_write(read_fd, write_buffer, str_len(TEST_STRING) + 1) != -1) {
        print(0xFFFFFF, "FAIL: Writing to read fd should have failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Writing to read fd correctly failed\n");

    // Test reading from write fd (should fail)
    if (call_pipe_read(write_fd, read_buffer, TEST_BUFFER_SIZE) != -1) {
        print(0xFFFFFF, "FAIL: Reading from write fd should have failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Reading from write fd correctly failed\n");

//    call_named_pipe_close(create_fd);
    call_named_pipe_close(read_fd);
    call_named_pipe_close(write_fd);
    print(0xFFFFFF, "Pipe I/O tests passed!\n\n");
}

void test_pipe_concurrent_processes() {
    print(0xFFFFFF, "Testing pipe with concurrent processes...\n");

    // Create the pipe first
    int create_fd = call_named_pipe_create(PIPE_NAME);
    if (create_fd < 3) {
        print(0xFFFFFF, "FAIL: Failed to create pipe\n");
        return;
    }

    char *empty_args[] = {"name"};

    // Create reader and writer processes
    uint64_t reader_pid = call_create_process(reader_process, 1, 1, empty_args, 0);
    uint64_t writer_pid = call_create_process(writer_process, 1, 1, empty_args, 0);

    if (writer_pid == -1 || reader_pid == -1) {
        print(0xFFFFFF, "FAIL: Failed to create processes\n");
        call_named_pipe_close(create_fd);
        return;
    }

    // Wait for both processes to complete
    call_waitpid(reader_pid);
    call_waitpid(writer_pid);

//    call_named_pipe_close(create_fd);
    print(0xFFFFFF, "PASS: Concurrent processes test completed\n\n");
}

int main_test_pipes() {
    print(0xFFFFFF, "Starting named pipe tests...\n\n");

    test_pipe_creation();
    test_pipe_io();
    test_pipe_concurrent_processes();

    print(0xFFFFFF, "All tests completed!\n");
    return 0;
}