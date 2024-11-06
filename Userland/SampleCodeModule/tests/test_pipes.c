// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "../include/lib.h"
#include "../include/usr_sys_calls.h"

#define TEST_BUFFER_SIZE 1024
#define TEST_BUFFER_SIZE_NORMAL 512
#define PIPE_NAME "test_pipe"
#define HUGE_MESSAGE_SIZE (TEST_BUFFER_SIZE - 1)
#define TEST_STRING "Hello through the pipe! Let's make this longer in order to test correctly!"
#define NUM_MESSAGES 5
#define READ 0
#define WRITE 1

uint64_t full_buffer_writer(uint64_t argc, char *argv[])
{
    if (argc != 1)
        return -1;

    int fd = call_named_pipe_open(PIPE_NAME, WRITE);
    if (fd < 3)
    {
        print(0xFFFFFF, "Full Buffer Writer: Failed to open pipe\n");
        return -1;
    }

    // Create a message that fills the buffer
    char huge_message[2000 + 1]; // +1 for null terminator
    for (int i = 0; i < 2000; i++)
    {
        huge_message[i] = 'A' + (i % 26); // Cycle through alphabet
    }
    huge_message[2000] = '\0';

    // Try to write the full buffer
    ssize_t written = call_sys_write(fd, huge_message, 2000 + 1, 0xffffff);
    if (written != 2000 + 1)
    {
        print(0xFFFFFF, "Full Buffer Writer: Write failed or incomplete (wrote %d bytes)\n", written);
        call_named_pipe_close(fd);
        return -1;
    }
    print(0xFFFFFF, "Full Buffer Writer: Successfully wrote %d bytes\n", written);

    call_named_pipe_close(fd);
    return 0;
}

// Process that reads data exactly matching buffer size
uint64_t full_buffer_reader(uint64_t argc, char *argv[])
{
    if (argc != 1)
        return -1;

    int fd = call_named_pipe_open(PIPE_NAME, READ);
    if (fd < 3)
    {
        print(0xFFFFFF, "Full Buffer Reader: Failed to open pipe\n");
        return -1;
    }

    char buffer[2001];
    ssize_t bytes_read = call_sys_read(fd, buffer, 3000);
    if (bytes_read != 0)
    {
        print(0xFFFFFF, "Full Buffer Reader: Read failed or incomplete (read %d bytes)\n", bytes_read);
        call_named_pipe_close(fd);
        return -1;
    }

    for (int i = 0; i < 2000; i++)
    {
        if (buffer[i] != ('A' + (i % 26)))
        {
            print(0xFFFFFF, "Full Buffer Reader: Data corruption at position %d\n", i);
            call_named_pipe_close(fd);
            return -1;
        }
    }


    // Verify data integrity

    print(0xFFFFFF, "Full Buffer Reader: Successfully read and verified %d bytes (because it returns EOF dipshit)\n", bytes_read);
    call_named_pipe_close(fd);
    return 0;
}

// Process that attempts to write more than buffer size
uint64_t overflow_writer(uint64_t argc, char *argv[])
{
    if (argc != 1)
        return -1;

    int fd = call_named_pipe_open(PIPE_NAME, WRITE);
    if (fd < 3)
        return -1;

    // Create an oversized message
    char huge_message[TEST_BUFFER_SIZE * 2];
    for (int i = 0; i < TEST_BUFFER_SIZE * 2 - 1; i++)
    {
        huge_message[i] = 'X';
    }
    huge_message[TEST_BUFFER_SIZE * 2 - 1] = '\0';

    // Attempt to write oversized message
    ssize_t written = call_sys_write(fd, huge_message, TEST_BUFFER_SIZE * 2, 0xffffff);
    print(0xFFFFFF, "Overflow Writer: Attempted write returned %d, (should be -1 since there is no reader active, this test makes no sense)\n", written);

    call_named_pipe_close(fd);
    return 0;
}

void test_edge_cases()
{
    print(0xFFFFFF, "Testing pipe edge cases...\n");

    // Test full buffer transfer
    if (call_named_pipe_create(PIPE_NAME) < 0)
    {
        print(0xFFFFFF, "FAIL: Failed to create pipe for full buffer test\n");
        return;
    }

    char *empty_args[] = {"NULL"};

    // Test exact buffer size transfer
    print(0xFFFFFF, "Testing exact buffer size transfer...\n");
    uint64_t reader_pid = call_create_process(full_buffer_reader, 0, 1, empty_args, 0);
    if (reader_pid == -1)
    {
        print(0xFFFFFF, "FAIL: Failed to create full buffer reader process\n");
        return;
    }

    print(0xFFFFFF, "Successfully created full buffer reader process\n");

    call_nice(); // Give reader time to start

    uint64_t writer_pid = call_create_process(full_buffer_writer, 0, 1, empty_args, 0);
    if (writer_pid == -1)
    {
        print(0xFFFFFF, "FAIL: Failed to create full buffer writer process\n");
        call_waitpid(reader_pid);
        return;
    }

    print(0xFFFFFF, "Successfully created full buffer reader process\n");

    // Wait for processes to complete
    call_waitpid(reader_pid);
    call_waitpid(writer_pid);

    //     Test buffer overflow attempt
    print(0xFFFFFF, "\nTesting buffer overflow handling...\n");

    if (call_named_pipe_create(PIPE_NAME) < 0)
    {
        print(0xFFFFFF, "FAIL: Failed to create pipe for overflow test\n");
        return;
    }

    writer_pid = call_create_process(overflow_writer, 0, 1, empty_args, 0);
    if (writer_pid == -1)
    {
        print(0xFFFFFF, "FAIL: Failed to create overflow writer process\n");
        return;
    }

    call_waitpid(writer_pid);
    print(0xFFFFFF, "Edge case tests completed!\n\n");
}

// Process that writes to the pipe
uint64_t writer_process(uint64_t argc, char *argv[])
{
    if (argc != 1)
        return -1;

    // Open the pipe in write mode
    int fd = call_named_pipe_open(PIPE_NAME, WRITE);
    if (fd < 3)
    {
        print(0xFFFFFF, "Writer: Failed to open pipe in write mode\n");
        return -1;
    }

    // Write multiple messages
    char *messages[] = {
        "Message 0 from writer",
        "Message 1 from writer",
        "Message 2 from writer",
        "Message 3 from writer",
        "Message 4 from writer"};

    for (int i = 0; i < NUM_MESSAGES; i++)
    {
        ssize_t written = call_sys_write(fd, messages[i], str_len(messages[i]) + 1, 0xffffff);
        if (written != str_len(messages[i]) + 1)
        {
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
uint64_t reader_process(uint64_t argc, char *argv[])
{
    if (argc != 1)
        return -1;

    // Open the pipe in read mode
    int fd = call_named_pipe_open(PIPE_NAME, READ);
    if (fd < 3)
    {
        print(0xFFFFFF, "Reader: Failed to open pipe in read mode\n");
        return -1;
    }

    char buffer[TEST_BUFFER_SIZE_NORMAL];
    for (int i = 0; i < NUM_MESSAGES; i++)
    {
        ssize_t bytes_read = call_sys_read(fd, buffer, 22);
        if (bytes_read <= 0)
        {
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

void test_pipe_creation_and_modes()
{
    print(0xFFFFFF, "Testing pipe creation and mode restrictions...\n");

    // Test basic pipe creation (should succeed but return 0 or 1)
    int result = call_named_pipe_create(PIPE_NAME);
    if (result < 0)
    {
        print(0xFFFFFF, "FAIL: Pipe creation failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Created pipe\n");

    // Test duplicate pipe creation (should fail)
    result = call_named_pipe_create(PIPE_NAME);
    if (result != -1)
    {
        print(0xFFFFFF, "FAIL: Duplicate pipe creation should have failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Duplicate pipe creation correctly failed\n");

    // Test opening in read mode
    int read_fd = call_named_pipe_open(PIPE_NAME, READ);
    if (read_fd < 3)
    {
        print(0xFFFFFF, "FAIL: Opening pipe in read mode failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Successfully opened pipe in read mode\n");

    // Try to open read mode again (should fail)
    int duplicate_read_fd = call_named_pipe_open(PIPE_NAME, READ);
    if (duplicate_read_fd != -1)
    {
        print(0xFFFFFF, "FAIL: Opening duplicate reader should have failed\n");
        return;
    }
    print(0xFFFFFF, "PASS: Duplicate reader correctly failed\n");

    // Try to open write mode from same process (should fail)
    int write_fd = call_named_pipe_open(PIPE_NAME, WRITE);
    if (write_fd != -1)
    {
        print(0xFFFFFF, "FAIL: Same process shouldn't be able to open both modes\n");
        return;
    }
    print(0xFFFFFF, "PASS: Correctly prevented same process from opening both modes\n");

    call_named_pipe_close(read_fd);
    print(0xFFFFFF, "Pipe creation and mode tests passed!\n\n");
}

void test_invalid_operations()
{
    print(0xFFFFFF, "Testing invalid operations...\n");

    // Create pipe
    if (call_named_pipe_create(PIPE_NAME) < 0)
    {
        print(0xFFFFFF, "FAIL: Pipe creation failed\n");
        return;
    }

    // Open pipe in read mode
    int read_fd = call_named_pipe_open(PIPE_NAME, READ);
    if (read_fd < 3)
    {
        print(0xFFFFFF, "FAIL: Opening pipe in read mode failed\n");
        return;
    }

    // Try to write to read fd (should fail)
    char buffer[] = "test";
    if (call_sys_write(read_fd, buffer, str_len(buffer), 0xffffff) != -1)
    {
        print(0xFFFFFF, "FAIL: Writing to read fd should have failed\n");
        call_named_pipe_close(read_fd);
        return;
    }
    print(0xFFFFFF, "PASS: Writing to read fd correctly failed\n");

    call_named_pipe_close(read_fd);
    print(0xFFFFFF, "Invalid operations tests passed!\n\n");
}

void test_pipe_concurrent_processes()
{
    print(0xFFFFFF, "Testing pipe with concurrent processes...\n");

    // Create the pipe
    if (call_named_pipe_create(PIPE_NAME) < 0)
    {
        print(0xFFFFFF, "FAIL: Failed to create pipe\n");
        return;
    }

    char *empty_args[] = {"name"};

    // Create reader and writer processes
    uint64_t reader_pid = call_create_process(reader_process, 0, 1, empty_args, 0);
    if (reader_pid == -1)
    {
        print(0xFFFFFF, "FAIL: Failed to create reader process\n");
        return;
    }

    // Give reader a moment to start
    call_nice();

    uint64_t writer_pid = call_create_process(writer_process, 0, 1, empty_args, 0);
    if (writer_pid == -1)
    {
        print(0xFFFFFF, "FAIL: Failed to create writer process\n");
        call_waitpid(reader_pid);
        return;
    }

    // Wait for both processes to complete
    call_waitpid(reader_pid);
    call_waitpid(writer_pid);

    print(0xFFFFFF, "PASS: Concurrent processes test completed\n\n");
}

int main_test_pipes()
{
    print(0xFFFFFF, "Starting named pipe tests...\n\n");

    test_pipe_creation_and_modes();
    test_invalid_operations();
    test_pipe_concurrent_processes();
    test_edge_cases();

    print(0xFFFFFF, "All tests completed!\n");
    return 0;
}