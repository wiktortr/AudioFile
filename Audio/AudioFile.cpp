#include "AudioFile.h"

#include <vorbis\codec.h>
#include <vorbis\vorbisfile.h>

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

#include <lame.h>

//#include "Drawer.h"

/* ========================================================================================================================================================================= */

int AudioFile::ReadWav(string path){
	//otwieranie is prawdzanie pliku
	ifstream f(path.c_str(), ios::binary);
	if (!f.good())
		return -1;

	//£adowanie nag³ównów
	wav_header header;
	f.read((char*)(&header), sizeof(wav_header));

	//Szukanie chunk'a z data
	bool loop_end_data = true;
	this->data_size = 0;
	int i = 0;
	while (loop_end_data){
		//oczytanie chunk'a
		wav_chunk chunk;
		f.read((char*)(&chunk), sizeof(wav_chunk));
		//sprawdzenie czy petla dosz³a do chunk'a z data
		if (chunk.name[0] == 'd' && chunk.name[1] == 'a' && chunk.name[2] == 't' && chunk.name[3] == 'a') {
			loop_end_data = false;
			data_size = chunk.size;
		}
		else {
			//odczytanie i skasowanie niepotrzebnego chunk'a
			af_int8* tmp = new af_int8[chunk.size];
			f.read((char*)tmp, chunk.size);
			delete tmp;
		}
		i++;
		//sprawdzanie czy pêtla nie przekroczy³a max chunk'ów
		if (i >= AF_WAV_MAX_SUBCHUNK)
			return -2;
	}

	//Odczywywanie danych
	af_int8* tmp_data = (af_int8*)malloc(data_size);
	f.read((char*)tmp_data, data_size);
	this->data = (af_int32*)tmp_data;

	//odczytywanie metadaty
	this->nChannels = header.NumOfChan;
	this->nBitrate = header.bitsPerSample;
	this->nSampleRate = header.SamplesPerSec;

	return 1;
}

/* ========================================================================================================================================================================= */

int AudioFile::ReadFlac(string path){
	//Otwarcie pliku i sprawdzenie
	drflac* pFlac = drflac_open_file("data.flac");
	if (pFlac == NULL)
		return -1;

	
	//Odczytywanie danych PCM z pliku
	data = (af_int32*)malloc((size_t)pFlac->totalSampleCount * sizeof(af_int32));
	data_size = (size_t)pFlac->totalSampleCount * sizeof(int32_t);
	drflac_read_s32(pFlac, pFlac->totalSampleCount, data);
	
	//Odczytywanie metadany
	this->nChannels = pFlac->channels;
	this->nBitrate = pFlac->bitsPerSample;
	this->nSampleRate = pFlac->sampleRate;

	//Usuwanie danych tymczasowych
	drflac_close(pFlac);

	//Draw((int16_t*)data, data_size, true);

	return 1;
}

/* ========================================================================================================================================================================= */

int AudioFile::ReadOgg(string path){
	//otwieranie i sprawdzanie pliku
	OggVorbis_File vf;
	if (ov_fopen(path.c_str(), &vf) != 0)
		return -1;

	vector<int8_t> data0;
	
	//Odczytywaie danych
	int eof = 0;
	int current_section;
	char pcmout[AF_OGG_BUFFER_SIZE];
	while (!eof) {
		long ret = ov_read(&vf, pcmout, AF_OGG_BUFFER_SIZE, 0, 2, 1, &current_section);
		if (ret == 0)
			eof = 1;
		else if (ret < 0) {
			if (ret == OV_EBADLINK)
				return -2;
		}
		else
			data0.insert(data0.end(), (int8_t*)pcmout, (int8_t*)pcmout + ret);
	}

	//Zapisywanie danych
	data = (int32_t*)&data0[0];
	data_size = data0.size();

	//Zapisywanie metadaty
	this->nChannels = vf.vi->channels;
	this->nBitrate = vf.vi->bitrate_nominal;
	this->nSampleRate = vf.vi->rate;

	//Draw((int16_t*)data, data_size, true);

	return 1;
}

/* ========================================================================================================================================================================= */

int AudioFile::ReadMP3(string path){
	//Tworzenie klasy lame
	lame_t lame = lame_init();
	lame_set_decode_only(lame, 1);
	if (lame_init_params(lame) == -1)
		return -1;


	//Tworzenie dekodera
	hip_t hip = hip_decode_init();


	//tworzenie struktóry mp3 data
	mp3data_struct mp3data;
	memset(&mp3data, 0, sizeof(mp3data_struct));


	//tworzenie tymczasowych zmiennych
	int read, i, samples;
	long cumulative_read = 0;
	int mp3_len;

	short pcm_l[AF_MP3_BUFFER_SIZE];
	short pcm_r[AF_MP3_BUFFER_SIZE];
	unsigned char mp3_buffer[AF_MP3_BUFFER_SIZE];

	//Otworzenie pliku i sprawdzenie go
	ifstream mp3(path.c_str(), ios::binary);
	if (!mp3.good())
		return -2;

	vector<af_int16> _data;

	//Pêtla, któa odczytuje kawa³ki pliku mp3
	while ((read = readBytes(&mp3, mp3_buffer, AF_MP3_BUFFER_SIZE)) > 0){
		//Zapisywanie danych tymczasowych
		mp3_len = read;
		cumulative_read += read * sizeof(char);

		//Pêtla, która dekoduje kawa³ki pliku mp3
		do{
			//Dekodowanie i sprawdzanie wielkoœci sampla
			samples = hip_decode1_headers(hip, mp3_buffer, mp3_len, pcm_l, pcm_r, &mp3data);

			//odczytywanie metadaty
			if (mp3data.header_parsed == 1) {
				this->nChannels = mp3data.stereo;
				this->nBitrate = mp3data.bitrate;
				this->nSampleRate = mp3data.samplerate;
			}

			//Sprawdzanie czy proces dekodowanie siê nie wywali³
			if (samples > 0 && mp3data.header_parsed != 1)
				return -3;

			//Odczytywanie danych
			if (samples > 0){
				//Sprawdzenie czy plik jest stereo
				if (nChannels == 2) {
					//tworzenie zmiennych tymczasowych
					af_uint32 j = 0;
					af_int16* new_data = new af_int16[samples * 2];
					//Pêtla do przekszta³cenia dwóch tablic w jedn¹
					for (uint32_t i = 0; i < samples; i++){
						new_data[j + 0] = (af_int16)pcm_l[i];
						new_data[j + 1] = (af_int16)pcm_r[i];
						j += 2;
					}
					//Wpisanie daych do tymczasowego vektora
					_data.insert(_data.end(), new_data, new_data + samples * 2);
				}
				else /* Je¿eli dŸwiêk jest mono to zapisanie tylko lewego kana³u */
					_data.insert(_data.end(), (af_int16*)pcm_l, (af_int16*)pcm_l + samples);
			}

			mp3_len = 0;
		} while (samples>0);
	}

	//Sprawdzenie czy s¹ dane
	if (_data.size() == 0)
		return -4;

	//Zapisanie danych
	data = (af_int32*)&_data[0];
	data_size = _data.size();

	//Draw((int16_t*)data, data_size, true);

	return 1;
}


/* ========================================================================================================================================================================= */

AudioFile::AudioFile(){ }

/* ========================================================================================================================================================================= */

int AudioFile::Read(string path){
	mode = AF_MODE_READ;
	extension = AFGetExtension(path);
	switch (extension){
	case AF_CODEC_FLAC:
		error = ReadFlac(path);
		break;
	case AF_CODEC_MP3:
		error = ReadMP3(path);
		break;
	case AF_CODEC_OGG:
		error = ReadOgg(path);
		break;
	case AF_CODEC_WAV:
		error = ReadWav(path);
		break;
	default:
		error = -10;
		break;
	}
	return error;
}

/* ========================================================================================================================================================================= */

int AudioFile::Read(af_byte * file_context, af_uint32 file_size){
	string path = to_string(file_size) + ".tmp";

	ofstream f(path.c_str(), ios::binary);
	f.write((char*)file_context, file_size);
	f.close();

	mode = AF_MODE_READ;
	extension = AFGetExtension(path);
	switch (extension) {
	case AF_CODEC_FLAC:
		error = ReadFlac(path);
		break;
	case AF_CODEC_MP3:
		error = ReadMP3(path);
		break;
	case AF_CODEC_OGG:
		error = ReadOgg(path);
		break;
	case AF_CODEC_WAV:
		error = ReadWav(path);
		break;
	default:
		error = -10;
		break;
	}
	remove(path.c_str());
	return error;
}

/* ========================================================================================================================================================================= */

string AudioFile::GetErrorStr(){
	string err;
	if (mode == AF_MODE_READ) {
		switch (extension) {
		case AF_CODEC_FLAC:
			switch (error) {
			case -1:
				err = "Error004: Error init lame decoder";
				break;
			default:
				err = "Error404: Undefined error";
				break;
			}
			break;
		case AF_CODEC_MP3:
			switch (error) {
			case -1:
				err = "Error004: Error init lame decoder";
				break;
			case -2:
				err = "Error001: File not exist";
				break;
			case -3:
				err = "Error005: Lame decode error";
				break;
			case -4:
				err = "Error006: Output buffer is empty";
				break;
			default:
				err = "Error404: Undefined error";
				break;
			}
			break;
		case AF_CODEC_OGG:
			switch (error) {
			case -1:
				err = "Error001: File not exist";
				break;
			case -2:
				err = "Error003: Ogg error load";
				break;
			default:
				err = "Error404: Undefined error";
				break;
			}
			break;
		case AF_CODEC_WAV:
			switch (error) {
			case -1:
				err = "Error001: File not exist";
				break;
			case -2:
				err = "Error002: Wav subchunk overflow";
				break;
			default:
				err = "Error404: Undefined error";
				break;
			}
			break;
		default:
			err = "Error000: Undefined audio codec";
			break;
		}
	}
	else if (mode == AF_MODE_WRITE) {

	}
	else {
		err = "Error: Undefined action";
	}
	return err;
}

/* ========================================================================================================================================================================= */

AudioFile::~AudioFile(){ }

/* ========================================================================================================================================================================= */

int AudioFile::readBytes(ifstream * f, unsigned char * data, unsigned int size) {
	f->read((char*)data, size);
	return f->gcount();
}

/* ========================================================================================================================================================================= */

int AFGetExtension(string path){
	char tmp[5];
	ifstream f(path.c_str(), ios::binary);
	f.read((char*)(tmp), 5);
	f.close();

	if (tmp[0] == 'R' && tmp[1] == 'I' && tmp[2] == 'F' && tmp[3] == 'F')
		return AF_CODEC_WAV;
	else if (tmp[0] == 'O' && tmp[1] == 'g' && tmp[2] == 'g')
		return AF_CODEC_OGG;
	else if (tmp[0] == 'I' && tmp[1] == 'D' && tmp[2] == '3')
		return AF_CODEC_MP3;
	else if (tmp[0] == 'f' && tmp[1] == 'L' && tmp[2] == 'a' && tmp[3] == 'C')
		return AF_CODEC_FLAC;
	else
		return AF_CODEC_UNDEFINED;
}

/* ========================================================================================================================================================================= */

string AFGetExtensionName(string path){
	int ext = AFGetExtension(path);
	return AFGetExtensionName(ext);
}

/* ========================================================================================================================================================================= */

string AFGetExtensionName(int af_codec_id){
	int ext = af_codec_id;
	if (ext == AF_CODEC_FLAC)
		return "Flac";
	if (ext == AF_CODEC_MP3)
		return "MP3";
	if (ext == AF_CODEC_OGG)
		return "Ogg";
	if (ext == AF_CODEC_WAV)
		return "Wav";
	else
		return "";
	return string();
}

/* ========================================================================================================================================================================= */
