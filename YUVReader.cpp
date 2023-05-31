#include <fstream>
#include <iostream>
using namespace std;
const char* inPath = "Video/dolby-vision-slider-R3.yuv";
const char* outPath = "miss_out.cls";

int main()
{
	ifstream in(inPath, ios::binary);
	in.seekg(0, ios::end);
	int size = in.tellg();
	// cout<<size<<endl;
	in.seekg(0, ios::beg);

	char* inBuffer = new char[size];
	in.read(inBuffer, size);
	in.close();

	char* outBuffer = new char[size];
	int width = 1920, height = 1080, inFrameSize = width * height * 3 / 2;
	int frameNum = size / inFrameSize;
	// int frameNum = 1;

	for (int i = 0; i < frameNum; ++i)
		for (int j = 0; j < inFrameSize; ++ j)
			outBuffer[i * inFrameSize + j] = j < width * height ? inBuffer[i * inFrameSize + j] : 128; // 只读取Y通道，UV置128

	ofstream out(outPath, ios::binary);
	out.write(outBuffer, size);
	out.close();
}
