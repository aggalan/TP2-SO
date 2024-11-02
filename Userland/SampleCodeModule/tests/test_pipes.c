#include <stdio.h>
#include "../include/lib.h"
#include "../include/usr_sys_calls.h"

#define TEST_BUFFER_SIZE 1024
#define TEST_STRING "Hello through the pipe! Let's make this longer in order to test correctly!"
#define PIPE_NAME "test_pipe"
#define NUM_MESSAGES 5

// Process that writes to the pipe
uint64_t writer_process(uint64_t argc, char *argv[]) {
    if (argc != 1) return -1;
    
    // Open the pipe
    int fd = call_named_pipe_open(PIPE_NAME);
    if (fd < 3) {
        print(0xFFFFFF, "Writer: Failed to open pipe\n");
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
    

    int fd = call_named_pipe_open(PIPE_NAME);
    if (fd < 3) {
        print(0xFFFFFF, "Reader: Failed to open pipe\n");
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
    print(0xFFFFFF,"Testing pipe creation...\n");

    int fd = call_named_pipe_create(PIPE_NAME);
    if (fd < 3) {
        print(0xffffff, "PIPE CREATION FAILED\n");
    }
    print(0xffffff, "Created pipe with fd: %d\n", fd);

    int fd2 = call_named_pipe_create(PIPE_NAME);
    if (fd2 != -1) {
        print(0xffffff, "DUPLICATE PIPE CREATION SHOULD FAIL\n");
    }
    print(0xffffff, "Duplicate pipe creation correctly failed\n");

    int fd3 = call_named_pipe_open(PIPE_NAME);
    if (fd3 != fd) {
        print(0xffffff, "OPNENING EXISTING PIPE FAILED\n");
    }
    print(0xffffff, "Successfully opened existing pipe\n");

    call_named_pipe_close(fd);
    call_named_pipe_close(fd3);
    print(0xffffff, "Pipe creation tests passed!\n\n");
}

void test_pipe_io() {
    print(0xffffff, "Testing pipe I/O...\n");

    int fd = call_named_pipe_create(PIPE_NAME);
    if (fd < 3) {
        print(0xffffff, "PIPE CREATION FAILED\n");
    }

    char write_buffer[TEST_BUFFER_SIZE];
    char read_buffer[TEST_BUFFER_SIZE];
    str_cpy(write_buffer, TEST_STRING);

    ssize_t bytes_written = call_pipe_write(fd, write_buffer, str_len(TEST_STRING) + 1);
    if (bytes_written != str_len(TEST_STRING) + 1){
        print(0xffffff, "WRITE FAILED\n");
    }
    print(0xffffff, "Written %d bytes to pipe\n", bytes_written);

    ssize_t bytes_read = call_pipe_read(fd, read_buffer, TEST_BUFFER_SIZE);
    if (bytes_read != str_len(TEST_STRING) + 1){
        print(0xffffff, "READ FAILED\n");
    }
    print(0xffffff, "Read %d bytes from pipe\n", bytes_read);

    if (str_cmp(write_buffer, read_buffer) != 0) {
        print(0xffffff, "DATA MISMATCH");
    }
    print(0xffffff, "Data verification passed\n");

    call_named_pipe_close(fd);
    print(0xffffff, "Pipe I/O tests passed!\n\n");
}

void test_pipe_multiple_processes() {
    print(0xffffff, "Testing pipe with multiple processes...\n");

    int fd1 = call_named_pipe_create(PIPE_NAME);
    print(0xffffff, "fd1 is: %d\n", fd1);
    if (fd1 < 3) {
        print(0xffffff, "PIPE CREATION FAILED\n");
    }

    int fd2 = call_named_pipe_open(PIPE_NAME);
    if (fd2 < 3) {
        print(0xffffff, "PIPE OPEN FAILED\n");
    }

    char write_buf[] = "Test Message 1";
    char read_buf[TEST_BUFFER_SIZE];

    ssize_t written = call_pipe_write(fd1, write_buf, str_len(write_buf) + 1);
    if (written != str_len(write_buf) + 1){
        print(0xffffff, "WRITE FAILED\n");
    }

    ssize_t read = call_pipe_read(fd2, read_buf, TEST_BUFFER_SIZE);
    if (read != str_len(write_buf) + 1){
        print(0xffffff, "READ FAILED\n");
    }
    if (str_cmp(write_buf, read_buf) != 0){
        print(0xffffff, "DATA MISMATCH\n");
    }

    call_named_pipe_close(fd1);
    call_named_pipe_close(fd2);
    print(0xffffff, "Multiple process tests passed!\n\n");
}

void test_pipe_concurrent_processes() {
    print(0xffffff, "Testing pipe with concurrent processes...\n");
    
    int pipe_fd = call_named_pipe_create(PIPE_NAME);
    if (pipe_fd < 3) {
        print(0xFFFFFF, "Failed to create pipe\n");
        return;
    }
    
    char *empty_args[] = {NULL};
    uint64_t writer_pid = call_create_process(writer_process, 1, 1, empty_args, 0);
    uint64_t reader_pid = call_create_process(reader_process, 1, 1, empty_args, 0);
    
    if (writer_pid == -1 || reader_pid == -1) {
        print(0xFFFFFF, "Failed to create processes\n");
        call_named_pipe_close(pipe_fd);
        return;
    }
    
    call_waitpid(writer_pid);
    call_waitpid(reader_pid);
    
    call_named_pipe_close(pipe_fd);
    print(0xffffff, "Concurrent processes test passed!\n\n");
}

int main_test_pipes() {
    print(0xffffff, "Starting named pipe tests...\n\n");

    test_pipe_creation();
    test_pipe_io();
    test_pipe_multiple_processes();
    test_pipe_concurrent_processes();

    print(0xffffff, "All tests completed successfully!\n");
    return 0;
}