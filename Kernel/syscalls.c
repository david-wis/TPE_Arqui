#include <stdint.h>
#include <video.h>
#include <keyboard.h>
#include <lib.h>

static char syscall_0(uint32_t fd);
static uint64_t syscall_1(uint32_t fd, const char *buff , uint64_t count);
static void syscall_2();
static uint32_t syscall_3();
uint64_t * syscall_4();
void syscall_5(uint8_t size);

uint64_t syscallDispatcher(uint64_t nr, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	switch (nr) {
        case 0:
            return syscall_0((uint32_t)arg0);
		case 1:
			return syscall_1((uint32_t)arg0, (char *)arg1, (uint64_t)arg2);
        case 2:
            syscall_2();
            return 0;
        case 3:
            return syscall_3();
        case 4:
            return syscall_4();
        case 5:
            syscall_5((uint8_t)arg0);
            return 0;
	}
	return -1;
}

// Read char
char syscall_0(uint32_t fd){
    switch (fd){
        case 0:
            return getAscii();
    }
    return 0;
}

// Write
uint64_t syscall_1(uint32_t fd, const char *buff , uint64_t count){
    Color color;
    switch (fd)
    {
    case 1:
        color = (Color){200, 200, 200};
        break;
    case 2:
        color = (Color){255, 0, 0};
        break;
    }
    setFontColor(color);
    print(buff);
    return count;
}

// Clear
void syscall_2(){
    videoClear();
}

// GetSeconds
uint32_t syscall_3(){
    char h, m, s;
    getTime(&h, &m, &s);
    return s + m * 60 + ((h - 3) % 24) * 3600;
}

// Inforeg
uint64_t * syscall_4() {
    return getRegisterArray();
}

// FontSize
void syscall_5(uint8_t size){
    if (size - 1 >= FONT_12 && size - 1 <= FONT_24)
        setFontSize(size - 1);
}
