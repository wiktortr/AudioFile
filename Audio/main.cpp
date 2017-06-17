#include <Windows.h>
#include "AudioFile.h"

int main() {

	AudioFile af_file;
	if (!af_file.Read("data.mp3")) {
		cout << "audio load error: " << af_file.GetErrorStr() << endl;
		system("pause");
	}

	/*ifstream f("data.flac", ios::binary);
	std::streampos fsize = 0;

	fsize = f.tellg();
	f.seekg(0, std::ios::end);
	fsize = f.tellg() - fsize;
	f.seekg(0, std::ios::beg);

	af_byte* d = new af_byte[fsize];
	f.read((char*)d, fsize);

	f.close();*/

	/*AudioFile af_file;
	if (!af_file.Read(d, fsize)) {
		cout << "audio load error: " << af_file.GetErrorStr() << endl;
		system("pause");
	}*/

	//af_file.~AudioFile();
	//Sleep(1000);
	return 0;
}
