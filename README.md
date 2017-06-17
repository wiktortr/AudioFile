# AudioFile

<p align="center">
<img src="http://i.imgur.com/dNweeQN.png" />
</p>

Ta biblioteka służąca do kodowania i dekodowania plików audio.

# Obsługuje formaty takie jak:
* Flac
* MP3
* Ogg
* Wav

# Jest bardzo prosta w użyciu
Przykład jak odczytywać dane:
``` c++
#include <AudioFile.h>

int main() {

	AudioFile af_file;
	if (af_file.Read("filename")) {
		cout << "Audio error: " << af_file.GetErrorStr() << endl;
	}
	else {
		cout << "Num of Channel: " << af_file.nChannels << endl;
		cout << "Bitrate: " << af_file.nBitRate << endl;
		cout << "SampleRate: " << af_file.nSampleRate << endl;
	}

	return 0;
}
```
Przykład jak zapisywać dane:
