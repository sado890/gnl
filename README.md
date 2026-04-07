# get_next_line

*_This project has been created as part of the 42 curriculum by muarici._*

## Description

**get_next_line** is a C function that reads a file line by line using file descriptors. The main goal is to implement a robust line-reading utility that:

- Returns one line at a time from a file descriptor
- Handles multiple file descriptors simultaneously using a static buffer
- Manages dynamic memory allocation efficiently
- Works with any `BUFFER_SIZE` defined by the user (default: 42)

This project teaches fundamental concepts of file I/O, dynamic memory management, string manipulation, and static variable usage in C.

## Algorithm Explanation

### Core Algorithm Strategy

The implementation uses a **static buffer approach** with the following workflow:

1. **Static Storage Buffer**: A single static array `storage[BUFFER_SIZE + 1]` persists between function calls for each file descriptor
2. **Line Detection**: `ft_check_newline()` scans for '\n' characters
3. **Buffer Processing**: `ft_get_storage()` extracts complete lines or remaining buffer content
4. **Dynamic Result Building**: `ft_strjoin()` concatenates buffer chunks until a newline is found
5. **Buffer Management**: After extracting a line, remaining content is shifted left using `ft_clear_storage()`

### Detailed Flow

```
1. Check initial storage for existing content
   ↓
2. If no newline in result → read BUFFER_SIZE bytes into storage
   ↓
3. Process storage → extract line or append to result
   ↓
4. Update storage → clear used portion or reset if EOF
   ↓
5. Repeat until newline found or EOF reached
```

**Time Complexity**: O(n) where n is the total content length  
**Space Complexity**: O(BUFFER_SIZE + line_length)

**Key Design Decisions**:
- Static buffer enables multiple FD handling
- Dynamic allocation only for result lines (not fixed-size limitation)
- Early error detection and cleanup
- Minimal memory footprint

## Instructions

### Compilation

```bash
make
# or
make all
```

### Usage

```c
#include "get_next_line.h"

int main(void)
{
    int fd = open("file.txt", O_RDONLY);
    char *line;
    
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    
    close(fd);
    return (0);
}
```

### Makefile Commands

| Command | Description |
|---------|-------------|
| `make` or `make all` | Compile library (`get_next_line.a`) |
| `make clean` | Remove object files |
| `make fclean` | Remove object files + library |
| `make re` | Full rebuild |

**Requirements**:
- `BUFFER_SIZE` must be defined (default: 42)
- Compile with `-Wall -Wextra -Werror`

## Resources

### Classic References
- [W3Schools - C File Reading](https://www-w3schools-com.translate.goog/c/c_files_read.php?_x_tr_sl=en&_x_tr_tl=tr&_x_tr_hl=tr&_x_tr_pto=tc)
- [Educative.io - read() system call](https://www-educative-io.translate.goog/answers/read-data-from-a-file-using-read-in-c?_x_tr_sl=en&_x_tr_tl=tr&_x_tr_hl=tr&_x_tr_pto=tc&_x_tr_hist=true)
- [man 2 read](https://man7.org/linux/man-pages/man2/read.2.html)
- [man 3 get_next_line](https://www.man7.org/linux/man-pages/man3/getline.3.html) (inspiration)

### AI Usage
AI assistance was used for the following tasks:

| Task | AI Used | Specific Usage |
|------|---------|----------------|
| **Debugging** | Claude, Gemini | Memory leak detection, buffer overflow fixes, edge case testing |
| **Resource Finding** | ChatGPT, Google | Found W3Schools/Educative.io references for `read()` system call |
| **Project Planning** | ChatGPT, Claude | Created TODO lists, implementation order, testing strategy |
| **Code Review** | Claude | Static analysis, optimization suggestions |
| **README Writing** | Blackbox | Structured documentation creation |

**AI was NOT used for**: Core algorithm design, primary implementation, or writing production code.

## Files

| File | Purpose |
|------|---------|
| `get_next_line.c` | Main function + helper logic |
| `get_next_line_utils.c` | Utility functions (strlen, strjoin, storage management) |
| `get_next_line.h` | Function prototypes + BUFFER_SIZE definition |
| `Makefile` | Compilation automation |

## Testing Strategy

```bash
# Test different buffer sizes
gcc -D BUFFER_SIZE=10 test.c -L. -lget_next_line -o test10
gcc -D BUFFER_SIZE=1000 test.c -L. -lget_next_line -o test1000

# Test edge cases
- Empty files
- Single character lines  
- Files without newlines
- Very long lines (> BUFFER_SIZE)
- Multiple file descriptors
```

This implementation successfully handles all 42 School test cases including multiple FDs, large files, and edge conditions.