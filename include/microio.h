// requires headers:
// #include <stdint.h>
// 
// binary reader / writer

typedef enum {
		  IO_MODE_NONE = 0,
		  IO_MODE_STACK = 1,
		  

}io_mode;
typedef struct{
  void * data;
  union{
    // don't set offset directly!! use io_advance/io_rewind/io_reset instead.
    size_t offset;
    void * user_data;
  };
  size_t size;
  // if its a read op, this reads
  // if its a write op, this writes
  void (* f)(void * data, size_t count, void * user_data);

  // in stack mode, reads will decrease the offset and write will increase it.
  io_mode mode;
}binary_io;

typedef binary_io io_base;
typedef binary_io io_writer;
typedef binary_io io_reader;

void io_advance(io_base * rd, size_t bytes);
void io_rewind(io_base * rd, size_t bytes);
void io_reset(io_base * io);
uint8_t io_read_u8(io_reader * rd);

binary_io io_stack_from_bytes(uint8_t * data, size_t count);
binary_io io_reader_from_bytes(uint8_t * data, size_t count);

// [obsolete] use io_offset instead.
size_t io_getloc(binary_io * io);
size_t io_offset(io_base * io);

uint8_t io_peek_u8(io_reader * rd);
uint64_t io_peek_u64(io_reader * rd);
void io_read(io_reader * rd, void * buffer, size_t len);

uint64_t io_read_u64_leb(io_reader * rd);
int64_t io_read_i64_leb(io_reader * rd);
int32_t io_read_i32_leb(io_reader * rd);
uint32_t io_read_u32_leb(io_reader * rd);
uint64_t io_read_u64(io_reader * rd);
uint32_t io_read_u32(io_reader * rd);
uint16_t io_read_u16(io_reader * rd);
uint8_t io_read_u8(io_reader * rd);
int64_t io_read_i64(io_reader * rd);
int32_t io_read_i32(io_reader * rd);
int16_t io_read_i16(io_reader * rd);
int8_t io_read_i8(io_reader * rd);
float io_read_f32(io_reader * rd);
double io_read_f64(io_reader * rd);
// caller deallocates the returned string.
// it expects a 0-terminated string.
char * io_read_str0(binary_io * io);
// caller deallocates the returned string.
// reads a unsigned LEB128 number and reads the following number of bytes.
char * io_read_strn(binary_io * io, uint32_t * out_len);

// reads a utf8 codepoint.
int32_t io_read_utf8_codepoint(io_reader * rd);


// write
void io_write(io_writer * writer, const void * data, size_t count);
void io_write_u64_leb(io_writer * wd, uint64_t value);
void io_write_i64_leb(io_writer * wd, int64_t value);
void io_write_u32_leb(io_writer * wd, uint32_t value);
void io_write_i32_leb(io_writer * wd, int32_t value);
void io_write_u8(io_writer * wd, uint8_t value);
void io_write_u16(io_writer * wd, uint16_t value);
void io_write_u32(io_writer * wd, uint32_t value);
void io_write_u64(io_writer * wd, uint64_t value);
void io_write_i8(io_writer * wd, int8_t value);
void io_write_i16(io_writer * wd, int16_t value);
void io_write_i32(io_writer * wd, int32_t value);
void io_write_i64(io_writer * wd, int64_t value);
void io_write_f64(io_writer * wd, double value);
void io_write_f32(io_writer * wd, float value);
// writes the string in zero-terminated format.
void io_write_str0(io_writer * wd, const char * str);
// writes the length of the string in unsigned LEB128 format and prepends the string itself.
void io_write_strn(io_writer * io, const char * str);
// writes a string to the buffer.
void io_write_str(io_writer * io, const char * str);
  

// writes a string to the writer.
//void io_write_str(io_writer * wd, const char * str);
// writes a formatted string. Note, this does 
void io_write_fmt(io_writer * io, const char * fmt, ...);
void io_writer_clear(io_writer * wd);
