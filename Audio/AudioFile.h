#pragma once
#ifndef AF_LIBRARY
#define AF_LIBRARY


#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <math.h>
#include <sstream>
#include <vector>

#include <stddef.h>
#include <stdio.h>

#include "config.h"


#ifndef AF_TYPES_DEFINED
#define AF_TYPES_DEFINED
#if defined(_MSC_VER) && _MSC_VER < 1600
typedef   signed char    af_int8;
typedef unsigned char    af_uint8;
typedef   signed short   af_int16;
typedef unsigned short   af_uint16;
typedef   signed int     af_int32;
typedef unsigned int     af_uint32;
typedef   signed __int64 af_int64;
typedef unsigned __int64 af_uint64;
typedef af_int8          af_byte;
#else
#include <stdint.h>
typedef int8_t           af_int8;
typedef uint8_t          af_uint8;
typedef int16_t          af_int16;
typedef uint16_t         af_uint16;
typedef int32_t          af_int32;
typedef uint32_t         af_uint32;
typedef int64_t          af_int64;
typedef uint64_t         af_uint64;
typedef uint8_t          af_byte;
#endif
#endif


#ifndef AF_STRUCT
#define AF_STRUCT
typedef struct {
	af_uint8         RIFF[4];
	af_uint32        ChunkSize;
	af_uint8         WAVE[4];
	af_uint8         fmt[4];
	af_uint32        Subchunk1Size;
	af_uint16        AudioFormat;
	af_uint16        NumOfChan;
	af_uint32        SamplesPerSec;
	af_uint32        bytesPerSec;
	af_uint16        blockAlign;
	af_uint16        bitsPerSample;
} wav_header;

typedef struct {
	af_uint8 name[4];
	af_uint32 size;
} wav_chunk;
#endif // !AF_STRUCT


#ifndef AF_DEFINES
#define AF_DEFINES

#define AF_WAV_MAX_SUBCHUNK 20
#define AF_OGG_BUFFER_SIZE 512
#define AF_MP3_BUFFER_SIZE 4096


#define AF_CODEC_UNDEFINED -1
#define AF_CODEC_WAV 0
#define AF_CODEC_OGG 1
#define AF_CODEC_MP3 2
#define AF_CODEC_FLAC 3


#define AF_MODE_NONE -1
#define AF_MODE_READ 1
#define AF_MODE_WRITE 2


#define AF_OK 1


#endif // !AF_DEFINES

using namespace std;

int    AFGetExtension(string path);
string AFGetExtensionName(int af_codec_id);
string AFGetExtensionName(string path);

class AudioFile{

	int readBytes(ifstream* f, unsigned char* data, unsigned int size);

	int ReadWav(string path);
	int ReadFlac(string path);
	int ReadOgg(string path);
	int ReadMP3(string path);

	/*int WriteWav(string path);
	int WriteFlac(string path);
	int WriteOgg(string path);
	int WriteMP3(string path);*/

	int error = AF_OK;
	int extension = AF_CODEC_UNDEFINED;
	int mode = AF_MODE_NONE;

public:
	AudioFile();
	~AudioFile();

	/*
	Funkcja publiczna.
	Funkcja ³aduje dane z pliku zwraca 1 je¿eli zostanie plik poprawnie za³adowany.
	*/
	int Read(string path);
	/*
	Funkcja publiczna.
	Funkcja ³aduje dane z buffora zwraca 1 je¿eli zostanie plik poprawnie za³adowany.
	*/
	int Read(af_byte* file_context, af_uint32 file_size);
	/*
	Funkcja publiczna.
	Funkcja zapisuje dane z tego obiektu jako plik.
	1 -- pomyœlnie zapisano
	0 -- b³¹d podczas zapisywania
	*/
	int Write(string path, int af_codec);
	/*
	Funkcja publiczna.
	Funkcja zapisuje dane z tego obiektu do buffora.
	1 -- pomyœlnie zapisano
	0 -- b³¹d podczas zapisywania
	*/
	int Encode(af_byte** file_context, af_uint32* file_size, int af_codec);



	/*
	Funkcja publiczna.
	Funkcja zwraca string z errorem.
	*/
	string GetErrorStr();


	/*
	Zmienna publiczna.
	Znajduje siê w niej data.
	*/
	af_int32* data;
	/*
	Zmienna publiczna.
	Znajduje siê w niej wielkoœæ daty.
	*/
	af_uint32 data_size;
	/*
	Zmienna publiczna.
	Znajduje siê w niej liczba kana³ów audio.
	*/
	int nChannels;
	/*
	Zmienna publiczna.
	Znajduje siê w niej BitRate audio.
	*/
	int nBitrate;
	/*
	Zmienna publiczna.
	Znajduje siê w niej SampleRate audio.
	*/
	int nSampleRate;
	

};

#endif // AF_LIBRARY