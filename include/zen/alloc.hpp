
#pragma once

#include <cstddef>
#include <stdlib.h>
#include <string.h>

#include <vector>

ZEN_NAMESPACE_START

#define ZEN_BLOCK_SIZE_NEXT sizeof(char**)
#define ZEN_BLOCK_SIZE_SIZE sizeof(std::size_t)

#define ZEN_BLOCK_OFFSET_NEXT 0
#define ZEN_BLOCK_OFFSET_SIZE ZEN_BLOCK_OFFSET_NEXT + ZEN_BLOCK_SIZE_NEXT
#define ZEN_BLOCK_OFFSET_DATA ZEN_BLOCK_OFFSET_SIZE + ZEN_BLOCK_SIZE_SIZE
#define ZEN_BLOCK_HEADER_SIZE ZEN_BLOCK_OFFSET_DATA

class block {

  friend class pool_alloc;

  char* data;

public:

  inline block(char* data):
    data(data) {
      if (data != nullptr) {
        set_size(0);
      }
    }

  operator bool() const noexcept {
    return data;
  }

  block next() const noexcept {
    return block(*reinterpret_cast<char**>(data + ZEN_BLOCK_OFFSET_NEXT));
  }

  void set_next(block new_next) const noexcept {
    memcpy(data + ZEN_BLOCK_OFFSET_NEXT, reinterpret_cast<char**>(&new_next.data), ZEN_BLOCK_SIZE_NEXT);
  }

  std::size_t size() const noexcept {
    return *reinterpret_cast<std::size_t*>(data + ZEN_BLOCK_OFFSET_SIZE);
  }

  void set_size(std::size_t new_size) const noexcept {
    memcpy(data, reinterpret_cast<char*>(&new_size), ZEN_BLOCK_SIZE_SIZE);
  }

};

class pool_alloc {

  block head = nullptr;
  block tail = nullptr;

  std::size_t block_size;

  block create_block() {
    auto raw = malloc(block_size + ZEN_BLOCK_HEADER_SIZE);
    if (!raw) {
      return nullptr;
    }
    block blk(static_cast<char*>(raw));
    return blk;
  }

  char* allocate_from_block(block& blk, std::size_t amount) {
    auto sz = blk.size();
    if (block_size < amount + sz) {
      return nullptr;
    }
    blk.set_size(sz + amount);
    return blk.data + ZEN_BLOCK_OFFSET_DATA + sz;
  }

public:

  inline pool_alloc(
    std::size_t block_size = 16 * 1024
  ): block_size(block_size) {}

  std::size_t max_alloc_size() const noexcept {
    return block_size;
  }

  void* allocate(std::size_t byte_count) {
    if (!tail) {
      head = tail = create_block();
      if (!tail) {
        return nullptr;
      }
      tail.set_next(nullptr);
    }
    auto ptr = allocate_from_block(tail, byte_count);
    if (ptr) {
      return ptr;
    }
    auto next = create_block();
    if (!next) {
      return nullptr;
    }
    tail.set_next(next);
    next.set_next(nullptr);
    tail = next;
    return allocate_from_block(tail, byte_count);
  }

};

ZEN_NAMESPACE_END
