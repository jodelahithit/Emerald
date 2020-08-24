#pragma once

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef unsigned int uint;
typedef unsigned char byte;
typedef string String;
typedef wstring WString;
typedef const char* String_t;
typedef const wchar_t* WString_t;

typedef glm::vec3 WorldPos;
//typedef glm::ivec3 BlockPos;
typedef glm::ivec3 ChunkPos;

#define BIT(x) (1 << x)

constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;