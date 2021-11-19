#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
#include "files.h"
};
#define FILENUM sizeof(file_table) / sizeof(Finfo) 

void init_fs() {
  // TODO: initialize the size of /dev/fb
}



int fs_open(const char *pathname, int flags, int mode){
  for(int i = 0; i < FILENUM; i++){
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len){
  Finfo *file = &file_table[fd];
  if (file->read == NULL) file->read = ramdisk_read;

  size_t read_len = 0, read_offset = file->disk_offset + file->open_offset;
  if (file->open_offset + len > file->size) {
    read_len = file->size - file->open_offset;
  } else read_len = len; 
  
  if (read_len <= 0) return 0;

  Log("read start in 0x%x(disk), 0x%x(file) len = 0x%x.", read_offset, file->open_offset, read_len);
  int rd = file->read(buf, read_offset, read_len);
  file->open_offset += rd;
  return rd;
}

size_t fs_write(int fd, const void *buf, size_t len){
  Finfo *file = &file_table[fd];
  if (file->write == NULL) file->write = ramdisk_write;

  size_t write_len = 0, write_offset = file->disk_offset + file->open_offset;
  if (file->open_offset + len > file->size) {
    write_len = file->size - file->open_offset;
  } else write_len = len;

  if (write_len <= 0) return 0; 

  Log("write start in 0x%x(disk), 0x%x(file) len = 0x%x.", write_offset, file->open_offset, write_len);
  int wd = file->write(buf, write_offset, write_len);
  file->open_offset += wd;
  return wd;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  Finfo *file = &file_table[fd];
  int ret = 0;
  switch (whence) {
    case SEEK_SET: file->open_offset = offset; break; 
    case SEEK_CUR: file->open_offset += offset; break;
    case SEEK_END: file->open_offset = file->size + offset; break;
    default: ret = -1;
  }
  if (ret == -1) return ret;
  else return file->open_offset;
}

int fs_close(int fd){
  file_table[fd].open_offset = 0;
  return 0;
}