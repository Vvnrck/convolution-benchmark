#include <iostream>
#include <fstream>
#include "rbg_image.h"


FloatMatrix RgbImage::blur_kernel;


RgbImage::RgbImage(int height, int width):
	height(height), width(width)
{
	for (int row = 0; row < this->height; row++)
	{
		this->red.push_back(vector<int>(width));
		this->grn.push_back(vector<int>(width));
		this->blu.push_back(vector<int>(width));
	}

	if (this->blur_kernel.size() == 0)
	{
		for (int row = 0; row < 3; row++)
			this->blur_kernel.push_back(vector<float>(3));

		blur_kernel[0][0] = 1.0 / 16; blur_kernel[0][1] = 1.0 / 8; blur_kernel[0][2] = 1.0 / 16;
		blur_kernel[1][0] = 1.0 / 8;  blur_kernel[1][1] = 1.0 / 4; blur_kernel[1][2] = 1.0 / 8;
		blur_kernel[2][0] = 1.0 / 16; blur_kernel[2][1] = 1.0 / 8; blur_kernel[2][2] = 1.0 / 16;
	}
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


RgbImage* RgbImage::save(string path)
{
	ofstream file(path, ios::binary | ios::out);
	for (int row = 0; row < this->height; row++)
	for (int col = 0; col < this->width; col++)
	{
		file.write((char*)(&this->red[row][col]), sizeof(int));
		file.write((char*)(&this->grn[row][col]), sizeof(int));
		file.write((char*)(&this->blu[row][col]), sizeof(int));
	}
	file.close();
	return this;
}


RgbImage* RgbImage::_convolve(FloatMatrix& kernel)
{
	RgbImage* out = new RgbImage(this->height, this->width);
	auto hit = [&](int row, int col) -> bool { 
		return row >= 0 && row < this->height
			&& col >= 0 && col < this->width; 
	};
	auto get_red = [&](int r, int c) -> int { return hit(r, c) ? this->red[r][c] : 0; };
	auto get_grn = [&](int r, int c) -> int { return hit(r, c) ? this->grn[r][c] : 0; };
	auto get_blu = [&](int r, int c) -> int { return hit(r, c) ? this->blu[r][c] : 0; };
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
			sum[0] += get_red(row, col) * kernel[row_offset][col_offset];
			sum[1] += get_grn(row, col) * kernel[row_offset][col_offset];
			sum[2] += get_grn(row, col) * kernel[row_offset][col_offset];
		}

		out->red[row_num][cell_num] = sum[0];
		out->grn[row_num][cell_num] = sum[1];
		out->blu[row_num][cell_num] = sum[2];
	}

	return out;
}


RgbImage* RgbImage::blur()
{
	return this->_convolve(this->blur_kernel);
}

