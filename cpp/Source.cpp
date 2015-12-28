#include <iostream>
#include <fstream>
#include <time.h>
#include "rbg_image.h"

void main(int argc, char** argv)
{
	clock_t start, end;
	double run_time;

	// bmpif, str(shape[0]), str(shape[1]), '1.bmp.cs.bmpif', benchmark
	if (argc != 6)
	{
		std::cout << "bmpif, str(shape[0]), str(shape[1]), '1.bmp.cs.bmpif', benchmark";
		return;
	}

	string in_file(argv[1]), out_file(argv[4]), bench(argv[5]);
	int heigth = std::stoi(argv[2]), width = std::stoi(argv[3]);
	auto image = (new RgbImage(heigth, width))->load2(in_file);

	start = clock();
	for (int _ = 0; _ < 100; _++)
	{
		auto blurred = image->blur()->save(out_file);
		delete blurred;
	}
	delete image;
	end = clock();

	run_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	ofstream file(bench, ios::app);
	file << "C++: " << run_time / 100 << "s" << std::endl;
	file.close();
}