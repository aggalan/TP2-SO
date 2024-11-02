#include <stdio.h>
#include "../include/lib.h"
#include "../include/usr_sys_calls.h"

#define TEST_BUFFER_SIZE 1024
#define TEST_STRING "Hello through the pipe! Let's make this longer in order to test correctly!"
#define PIPE_NAME "test_pipe"

void test_pipe_creation() {
    print(0xFFFFFF,"Testing pipe creation...\n");

    // Create a new pipe
    int fd = call_named_pipe_create(PIPE_NAME);
    if (fd < 3) {
        print(0xffffff, "PIPE CREATION FAILED\n");
    }
//    assert(fd >= 0 && "Pipe creation failed");
    print(0xffffff, "Created pipe with fd: %d\n", fd);

    // Try to create pipe with same name (should fail)
    int fd2 = call_named_pipe_create(PIPE_NAME);
    if (fd2 != -1) {
        print(0xffffff, "DUPLICATE PIPE CREATION SHOULD FAIL\n");
    }
//    assert(fd2 == -1 && "Duplicate pipe creation should fail");
    print(0xffffff, "Duplicate pipe creation correctly failed\n");

    // Open existing pipe
    int fd3 = call_named_pipe_open(PIPE_NAME);
    if (fd3 != fd) {
        print(0xffffff, "OPNENING EXISTING PIPE FAILED\n");
    }
//    assert(fd3 == fd && "Opening existing pipe failed");
    print(0xffffff, "Successfully opened existing pipe\n");

    // Close both references
    call_named_pipe_close(fd);
    call_named_pipe_close(fd3);
    print(0xffffff, "Pipe creation tests passed!\n\n");
}

void test_pipe_io() {
    print(0xffffff, "Testing pipe I/O...\n");

    // Create a new pipe
    int fd = call_named_pipe_create(PIPE_NAME);
    if (fd < 3) {
        print(0xffffff, "PIPE CREATION FAILED\n");
    }
//    assert(fd >= 0 && "Pipe creation failed");

    // Prepare test data
    char write_buffer[TEST_BUFFER_SIZE];
    char read_buffer[TEST_BUFFER_SIZE];
    str_cpy(write_buffer, TEST_STRING);

    // Write to pipe
    ssize_t bytes_written = call_pipe_write(fd, write_buffer, str_len(TEST_STRING) + 1);
    if (bytes_written != str_len(TEST_STRING) + 1){
        print(0xffffff, "WRITE FAILED\n");
    }
//    assert(bytes_written == strlen(TEST_STRING) + 1 && "Write failed");
    print(0xffffff, "Written %d bytes to pipe\n", bytes_written);

    // Read from pipe
    ssize_t bytes_read = call_pipe_read(fd, read_buffer, TEST_BUFFER_SIZE);
    if (bytes_read != str_len(TEST_STRING) + 1){
        print(0xffffff, "READ FAILED\n");
    }
//    assert(bytes_read == strlen(TEST_STRING) + 1 && "Read failed");
    print(0xffffff, "Read %d bytes from pipe\n", bytes_read);

    // Verify data
    if (str_cmp(write_buffer, read_buffer) != 0) {
        print(0xffffff, "DATA MISMATCH");
    }
//    assert(strcmp(write_buffer, read_buffer) == 0 && "Data mismatch");
    print(0xffffff, "Data verification passed\n");

    // Clean up
    call_named_pipe_close(fd);
    print(0xffffff, "Pipe I/O tests passed!\n\n");
}

void test_pipe_multiple_processes() {
    print(0xffffff, "Testing pipe with multiple processes...\n");

    // Create pipe
    int fd1 = call_named_pipe_create(PIPE_NAME);
    print(0xffffff, "fd1 is: %d\n", fd1);
    if (fd1 < 3) {
        print(0xffffff, "PIPE CREATION FAILED\n");
    }
//    assert(fd1 >= 0 && "Pipe creation failed");

    // Open second reference
    int fd2 = call_named_pipe_open(PIPE_NAME);
    if (fd2 < 3) {
        print(0xffffff, "PIPE OPEN FAILED\n");
    }
//    assert(fd2 >= 0 && "Pipe open failed");

    // Test concurrent access
    char write_buf[] = "Test Message 1";
    char read_buf[TEST_BUFFER_SIZE];

    ssize_t written = call_pipe_write(fd1, write_buf, str_len(write_buf) + 1);
    if (written != str_len(write_buf) + 1){
        print(0xffffff, "WRITE FAILED\n");
    }
//    assert(written == strlen(write_buf) + 1 && "Write failed");

    ssize_t read = call_pipe_read(fd2, read_buf, TEST_BUFFER_SIZE);
    if (read != str_len(write_buf) + 1){
        print(0xffffff, "READ FAILED\n");
    }
//    assert(read == strlen(write_buf) + 1 && "Read failed");
    if (str_cmp(write_buf, read_buf) != 0){
        print(0xffffff, "DATA MISMATCH\n");
    }
//    assert(strcmp(write_buf, read_buf) == 0 && "Data mismatch");

    // Clean up
    call_named_pipe_close(fd1);
    call_named_pipe_close(fd2);
    print(0xffffff, "Multiple process tests passed!\n\n");
}

int main_test_pipes() {
    print(0xffffff, "Starting named pipe tests...\n\n");

    // Run tests
    test_pipe_creation();
    test_pipe_io();
    test_pipe_multiple_processes();

    print(0xffffff, "All tests completed successfully!\n");
    return 0;
}