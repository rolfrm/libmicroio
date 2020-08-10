
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <microio.h>
#include <string.h>
#include <stdio.h>
#define UNUSED(x) (void)(x)


typedef int8_t i8;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

static void read_some(void * data, u64 count, void * ptr){
  FILE * f = ptr;
  fread(data, count, 1, f);
}


static void write_some(void * data, u64 count, void * ptr){
  FILE * f = ptr;
  fwrite(data, count, 1, f);
}


int main(int argc, char ** argv){
  UNUSED(argc);UNUSED(argv);

  for(int i = 0; i < 2; i++){

    binary_io _io = {0};
    binary_io * io = &_io;

    if(i == 1){
      io->user_data = fopen("./test.bin", "a");
      io->f = write_some;
    }
    
    u8 e1 = 42;
    i64 e2 = -32196491321;
    const char * e3 =  "Hello World";
    double e4 = 1.53213;
    io_write_u8(io, e1);
    io_write_i64_leb(io, e2);
    io_write_str(io, "Hello World");
    io_write_f64(io, e4);

    if(i == 1){
      fclose(io->user_data);
      io->user_data = fopen("./test.bin", "r");
      io->f = read_some;
      assert(io->user_data);
    }else{
      io_reset(io);
    }
 
    u8 a = io_read_u8(io);
    i64 b = io_read_i64_leb(io);
    char * c = io_read_str(io);
    double d = io_read_f64(io);
    if(i == 1){
      fclose(io->user_data);
      remove("test.bin");
    }
    assert(a == e1);
    assert(b == e2);
    assert(strcmp(e3, c) == 0);
    assert(d == e4);
  }
  
  printf("tests passed!\n");
  return 0;
  
}
