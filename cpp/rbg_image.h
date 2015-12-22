#pragma once
#include <vector>
#include <string>
using namespace std;
typedef vector<vector<int>>   Matrix;
typedef vector<vector<int>>   FloatMatrix;


class RgbImage
{
	Matrix red;
	Matrix grn;
	Matrix blu;

public:
	int height;
	int width;

	RgbImage(int height, int width);
	RgbImage* load(string path_to_bmpif);
	RgbImage* blur();
	RgbImage* save(string path);

private:
	RgbImage* _convolve(FloatMatrix& kernel);
	static FloatMatrix blur_kernel;

};