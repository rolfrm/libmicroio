// requires headers:
// #include <stdint.h>
// 
// binary reader / writer
typedef struct{
  void * data;
  union{
    size_t offset;
    void * user_data;
  };
  size_t size;

  // if its a read op, this reads
  // if its a write op, this writes
  void (* f)(void * data, size_t count, void * user_data);
}binary_io;

typedef binary_io io_writer;
typedef binary_io io_reader;

void io_advance(io_reader * rd, size_t bytes);
void io_rewind(io_reader * rd, size_t bytes);
uint8_t io_read_u8(io_reader * rd);
uint8_t io_peek_u8(io_reader * rd);
void io_read(io_reader * rd, void * buffer, size_t len);
uint64_t io_read_u64_leb(io_reader * rd);
uint32_t io_read_u32_leb(io_reader * rd);
int32_t io_read_i32_leb(io_reader * rd);
int64_t io_read_i64_leb(io_reader * rd);
uint64_t io_read_u64(io_reader * rd);
uint32_t io_read_u32(io_reader * rd);

uint16_t io_read_u16(io_reader * rd);
float io_read_f32(io_reader * rd);
double io_read_f64(io_reader * rd);
int64_t io_read_i64(io_reader * rd);
int32_t io_read_i32(io_reader * rd);

// caller deallocates the returned string.
char * io_read_str(binary_io * io);


void io_write(io_writer * writer, const void * data, size_t count);
void io_write_u8(io_writer * wd, uint8_t value);
void io_write_u32(io_writer * wd, uint32_t value);
void io_write_u64(io_writer * wd, uint64_t value);
void io_write_i32(io_writer * wd, int32_t value);
void io_write_str(io_writer * wd, const char * str);
void io_write_u64_leb(io_writer * wd, uint64_t value);
void io_write_i64_leb(io_writer * wd, int64_t value);
void io_write_f64(io_writer * wd, double value);
void io_write_f32(io_writer * wd, float value);

