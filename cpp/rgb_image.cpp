#include <iostream>
#include <fstream>
#include "rbg_image.h"


ValueMatrix RgbImage::blur_kernel;


RgbImage::RgbImage(int height, int width):
height(height), width(width)
{
	this->_data = new int[_file_size()];
	int i = 0;
	for (int row = 0; row < this->height; row++)
	{
		vector<reference_wrapper<int>> red_row, grn_row, blu_row;
		for (int col = 0; col < this->width; col++)
		{
			red_row.push_back(this->_data[i++]);
			grn_row.push_back(this->_data[i++]);
			blu_row.push_back(this->_data[i++]);
		}
		this->red.push_back(red_row);
		this->grn.push_back(grn_row);
		this->blu.push_back(blu_row);
	}

	if (this->blur_kernel.size() == 0)
	{
		for (int row = 0; row < 3; row++)
			this->blur_kernel.push_back(vector<int>(3));

		blur_kernel[0][0] = 4; blur_kernel[0][1] = 3; blur_kernel[0][2] = 4;
		blur_kernel[1][0] = 3; blur_kernel[1][1] = 2; blur_kernel[1][2] = 3;
		blur_kernel[2][0] = 4; blur_kernel[2][1] = 3; blur_kernel[2][2] = 4;
	}
}

RgbImage::~RgbImage()
{
	delete this->_data;
}

int RgbImage::_file_size()
{
	return this->width * this->height * 3;
}


RgbImage* RgbImage::load(string path_to_bmpif)
{
	int pixel;
	ifstream file(path_to_bmpif, ios::binary | ios::in);
	for (int row = 0; row < this->height; row++)
	for (int col = 0; col < this->width; col++)
	{
		file.read((char*)(&pixel), sizeof(int));
		this->red[row][col] = pixel;

		file.read((char*)(&pixel), sizeof(int));
		this->grn[row][col] = pixel;

		file.read((char*)(&pixel), sizeof(int));
		this->blu[row][col] = pixel;
	}
	file.close();
	return this;
}


RgbImage* RgbImage::load2(string path_to_bmpif)
{
	ifstream file(path_to_bmpif, ios::binary | ios::in);
	int file_size = sizeof(int) * _file_size(), i = 0;
	file.read((char*)this->_data, file_size);
	file.close();
	return this;
}


RgbImage* RgbImage::save(string path)
{
	ofstream file(path, ios::binary | ios::out);
	file.write((char*)this->_data, sizeof(int)*_file_size());
	file.close();
	return this;
}


RgbImage* RgbImage::_convolve(ValueMatrix& kernel)
{
	RgbImage* out = new RgbImage(this->height, this->width);
	int  kernel_width = kernel.size() / 2;
	int  sum[3];

	for (int row_num = 0; row_num < this->height; row_num++)
	for (int cell_num = 0; cell_num < this->width; cell_num++)
	{
		int base_row = row_num - kernel_width;
		int base_col = cell_num - kernel_width;
		sum[0] = sum[1] = sum[2] = 0;

		for (int row_offset = 0; row_offset < 3; row_offset++)
		for (int col_offset = 0; col_offset < 3; col_offset++)
		{
			int row = base_row + row_offset;
			int col = base_col + col_offset;
			if (row >= 0 && row < this->height && col >= 0 && col < this->width) {
				sum[0] += this->red[row][col].get() >> kernel[row_offset][col_offset];
				sum[1] += this->grn[row][col].get() >> kernel[row_offset][col_offset];
				sum[2] += this->blu[row][col].get() >> kernel[row_offset][col_offset];
			}
		}
		out->red[row_num][cell_num].get() = sum[0];
		out->grn[row_num][cell_num].get() = sum[1];
		out->blu[row_num][cell_num].get() = sum[2];
	}
	return out;
}


RgbImage* RgbImage::blur()
{
	return this->_convolve(this->blur_kernel);
}

