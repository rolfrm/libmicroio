
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <microio.h>
#include <string.h>
#include <stdio.h>
#define UNUSED(x) (void)(x)


typedef int8_t i8;
typedef int16_t i16;
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

void test_stack_mode(){
  binary_io _io = {0};
  _io.mode = IO_MODE_STACK;
  binary_io * io = &_io;
  io_write_f32(io, 1.0f);
  io_write_f32(io, 5.0f);
  io_write_i32(io, 123123123);
  io_write_u8(io, 5);
  assert(io_read_u8(io) == 5);
  assert(io_read_i32(io) == 123123123);
  assert(io_read_f32(io) == 5.0f);
  assert(io_read_f32(io) == 1.0f);  
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
    float e5 = 1.0f / 3.0f;
    u64 e6 = 0xFF0012321111FABCL;
    i16 e7 = -3127;
    io_write_u8(io, e1);
    io_write_i64_leb(io, e2);
    io_write_str0(io, "Hello World");
    io_write_strn(io, "Hello World");
    io_write_f64(io, e4);
    io_write_f32(io, e5);
    io_write_u64_leb(io, e6);
    io_write_i16(io, e7);
    if(i == 1){
      fclose(io->user_data);
      io->user_data = fopen("./test.bin", "r");
      io->f = read_some;
      assert(io->user_data);
    }else{
      io_reset(io);
    }
 
    u8 r1 = io_read_u8(io);
    assert(r1 == e1);
    i64 r2 = io_read_i64_leb(io);
    assert(r2 == e2);
    char * r3 = io_read_str0(io);
    assert(strcmp(e3, r3) == 0);
    u32 r3_len;
    char * r3_2 = io_read_strn(io, &r3_len);
    assert(r3_len == strlen(r3));
    assert(strncmp(e3, r3_2, r3_len) == 0);
    
    double r4 = io_read_f64(io);
    assert(r4 == e4);
    float r5 = io_read_f32(io);
    assert(r5 == e5);
    u64 r6 = io_read_u64_leb(io);
    assert(r6 == e6);
    i16 r7 = io_read_i16(io);
    assert(r7 == e7);
    if(i == 1){
      fclose(io->user_data);
      remove("test.bin");
    }
  }
  
  printf("tests passed!\n");
  printf("Testing stack mode:\n");
  test_stack_mode();
  printf("End of test 2\n");
  return 0;
  
}
