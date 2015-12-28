#pragma once
#include <vector>
#include <string>
#include <functional>
using namespace std;
typedef vector<vector<reference_wrapper<int>>> Matrix;
typedef vector<vector<int>>					   ValueMatrix;


class RgbImage
{
	Matrix red;
	Matrix grn;
	Matrix blu;
	int* _data;

public:
	int height;
	int width;

	RgbImage(int height, int width);
	RgbImage* load(string path_to_bmpif);
	RgbImage* load2(string path_to_bmpif);
	RgbImage* blur();
	RgbImage* save(string path);
	~RgbImage();

private:
	int _file_size();
	RgbImage* _convolve(ValueMatrix& kernel);
	static ValueMatrix blur_kernel;

};