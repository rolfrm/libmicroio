#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "microio.h"

typedef int8_t i8;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;


void io_advance(binary_io * rd, size_t bytes){
  if(rd->f){
    rd->f(NULL, bytes, rd->user_data);
    return;
  }
  assert(rd->offset + bytes <= rd->size);
  rd->offset += bytes;
}

void io_rewind(io_reader * rd, size_t bytes){
  if(rd->f){
    perror("Rewind not supported by reader\n");
    return;
  }
  assert(((int)rd->offset - bytes) > 0);
  rd->offset -= bytes;
}


u8 io_read_u8(io_reader * rd){
  if(rd->f){
    u8 r;
    rd->f(&r, 1, rd->user_data);
    return r;
  }
  u8 b = ((u8 *)(rd->data + rd->offset))[0];
  io_advance(rd, 1);
  return b;
}

u8 io_peek_u8(io_reader * rd){
  if(rd->f){
    perror("Rewind not supported by reader\n");
    return 0;
  }

  u8 b = ((u8 *)(rd->data + rd->offset))[0];
  return b;
}

void io_read(io_reader * rd, void * buffer, size_t len){
  if(rd->f){
    rd->f(buffer, len, rd->user_data);
    return;
  }
  assert(rd->offset + len <= rd->size);
  memcpy(buffer, rd->data + rd->offset, len);
  io_advance(rd, len);
}

u64 io_read_u64_leb(io_reader * rd){
  // read LEB128
  u8 chunk = 0;
  u64 value = 0;
  u32 offset = 0;
  while((chunk = io_read_u8(rd)) > 0){
    value |= (0b01111111L & chunk) << offset;
    offset += 7;
    if((0b10000000L & chunk) == false)
      break;
  }
  return value;
}


u32 io_read_u32_leb(io_reader * rd){
  return io_read_u64_leb(rd);
}

u64 io_read_u64(io_reader * rd){
  u64 value;
  io_read(rd, &value, sizeof(value));
  return value;
}

i32 io_read_i32(io_reader * rd){
  i32 value;
  io_read(rd, &value, sizeof(value));
  return value;
}

i64 io_read_i64(io_reader * rd){
  i64 value;
  io_read(rd, &value, sizeof(value));
  return value;
}

u32 io_read_u32(io_reader * rd){
  u32 value;
  io_read(rd, &value, sizeof(value));
  return value;
}

u16 io_read_u16(io_reader * rd){
  u16 value;
  io_read(rd, &value, sizeof(value));
  return value;
}


f32 io_read_f32(io_reader * rd){
  f32 v = 0;
  memcpy(&v, rd->data + rd->offset, sizeof(v));
  io_advance(rd, sizeof(v));
  return v;
}

f64 io_read_f64(io_reader * rd){
  f64 v = 0;
  io_read(rd, &v, sizeof(v));
  return v;
}

i64 io_read_i64_leb(io_reader * rd) {
    // read LEB128
  i64 value = 0;
  u32 shift = 0;
  u8 chunk;
  do {
    chunk = io_read_u8(rd);
    value |= (((u64)(chunk & 0x7f)) << shift);
    shift += 7;
  } while (chunk >= 128);
  if (shift < 64 && (chunk & 0x40))
    value |= (-1ULL) << shift;
  return value;
}

i32 io_read_i32_leb(io_reader * rd){
  return (i32)io_read_i64_leb(rd);
}

size_t reader_getloc(io_reader * rd){
  assert(rd->f == NULL);
  return rd->offset;
}
  
char * io_readname(io_reader * rd){
  u32 len = io_read_u32_leb(rd);
  char * buffer = malloc(len + 1);
  io_read(rd, buffer, len);
  buffer[len] = 0;
  return buffer;
}

char * io_read_str(binary_io * io){
  char * buf = NULL;
  char c = 0;
  size_t s = 0;
  do{
    c = io_read_u8(io);
    buf = realloc(buf, (s = s + 1));
    buf[s- 1] = c;
  }while(c != 0);
  return buf;
}


void io_write(io_writer * writer, const void * data, size_t count){
  if(writer->f){
    writer->f((void *) data, count, writer->user_data);
    return;
  }
  if(writer->offset + count > writer->size){
    size_t newsize = (writer->size + count) * 1.2;
    writer->data = realloc(writer->data, newsize);
    writer->size = newsize;
  }
  memcpy(writer->data + writer->offset, data, count);
  writer->offset += count;
}
void io_write_u8(io_writer * wd, u8 value){ io_write(wd, &value, sizeof(value));} 
void io_write_u32(io_writer * wd, u32 value){ io_write(wd, &value, sizeof(value));}
void io_write_u64(io_writer * wd, u64 value){ io_write(wd, &value, sizeof(value));}
void io_write_i32(io_writer * wd, i32 value){ io_write(wd, &value, sizeof(value));}
void io_write_i64(io_writer * wd, i64 value){ io_write(wd, &value, sizeof(value));}
void io_write_f32(io_writer * wd, f32 value){ io_write(wd, &value, sizeof(value));}
void io_write_f64(io_writer * wd, f64 value){ io_write(wd, &value, sizeof(value));}



void io_write_str(binary_io * io, const char * str){
  int len = strlen(str);
  io_write(io, str, len + 1);
}

void io_write_u64_leb(io_writer * wd, u64 value){
  while(true){
    u8 to_write = value & 0b01111111L;
    value >>= 7;
    if(value){
      to_write |= 0b10000000L;
      io_write_u8(wd, to_write);
    }
    else{
      io_write_u8(wd, to_write);
      break;
    }
  }
}

void io_write_i64_leb(io_writer * wd, i64 value){
  while(true){
    u8 bits = value & 0b01111111;
    u8 sign = value & 0b01000000;
    i64 next = value >> 7;
    if((next == 0 && sign == 0) || (sign > 0 && next == -1)){
      io_write_u8(wd, bits);
      break;
    }else{
      io_write_u8(wd, bits | 0b10000000);
      value = next;
    }
  }
}

