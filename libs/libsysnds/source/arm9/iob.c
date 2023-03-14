#include <stdio.h>

#include <nds/interrupts.h>
#include <nds/arm9/keyboard.h>

// Defined in libnds
typedef ssize_t (* fn_write_ptr)(const char *, size_t);
extern fn_write_ptr libnds_stdout_write, libnds_stderr_write;

// Buffers so that we can send libnds full ANSI escape sequences.
#define BUFFER_SIZE 16
static char stdout_buf[BUFFER_SIZE + 1];
static int stdout_buf_len = 0;
static char stderr_buf[BUFFER_SIZE + 1];
static int stderr_buf_len = 0;

static int putc_buffered(char c, char *buf, int *buf_len, fn_write_ptr fn)
{
    if ((c == 0x1B) || (*buf_len > 0))
    {
        buf[*buf_len] = c;
        (*buf_len)++;
        buf[*buf_len] = 0;

        if ((*buf_len == BUFFER_SIZE) || (c == '\n') || (c == '\r') ||
            ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))
        {
            fn(buf, *buf_len);
            *buf_len = 0;
        }
    }
    else
    {
        fn(&c, 1);
    }

    return c;
}

static int stdout_putc_buffered(char c, FILE *file)
{
    (void)file;

    if (libnds_stdout_write == NULL)
        return c;

    return putc_buffered(c, stdout_buf, &stdout_buf_len, libnds_stdout_write);
}

static int stderr_putc_buffered(char c, FILE *file)
{
    (void)file;

    if (libnds_stderr_write == NULL)
        return c;

    return putc_buffered(c, stderr_buf, &stderr_buf_len, libnds_stderr_write);
}


int stdin_getc_keyboard(FILE *file)
{
    (void)file;

    static int shown = 0;

    if (shown == 0)
    {
        keyboardShow();
        shown = 1;
    }

    int c = -1;
    while (c == -1)
    {
        c = keyboardUpdate();
        swiWaitForVBlank();
    }

    if (c == '\n')
    {
        keyboardHide();
        shown = 0;
    }

    return c;
}

static FILE __stdin = FDEV_SETUP_STREAM(NULL, stdin_getc_keyboard, NULL,
                                        _FDEV_SETUP_READ);
static FILE __stdout = FDEV_SETUP_STREAM(stdout_putc_buffered, NULL, NULL,
                                         _FDEV_SETUP_WRITE);
static FILE __stderr = FDEV_SETUP_STREAM(stderr_putc_buffered, NULL, NULL,
                                         _FDEV_SETUP_WRITE);

FILE *const stdin = &__stdin;
FILE *const stdout = &__stdout;
FILE *const stderr = &__stderr;
