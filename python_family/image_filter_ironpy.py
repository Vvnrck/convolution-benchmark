import sys
import struct
from itertools import product


filters = {
    'emboss': [
        [-2, -1, 0],
        [-1,  1, 1],
        [0,   1, 2]
    ],
    'blur 3x3': (
        (1/16, 1/8, 1/16),
        (1/8,  1/4, 1/8),
        (1/16, 1/8, 1/16)
    ),
    'identity': [
        [0, 0, 0],
        [0, 1, 0],
        [0, 0, 0]
    ]
}


class RgbImage:
	def __init__(self, height, width, path=None):
		self.height = int(height)
		self.width = int(width)
		self.red = [[0 for _ in range(self.width)] for _ in range(self.height)]
		self.grn = [[0 for _ in range(self.width)] for _ in range(self.height)]
		self.blu = [[0 for _ in range(self.width)] for _ in range(self.height)]
		self.image = [self.red, self.grn, self.blu]
		if path is not None:
			self.load(path)

	def load(self, path):
		with open(path, 'rb') as f:
			read_int = lambda: struct.unpack('i', f.read(4))[0]
			for row, col, channel in self.index_iter():
				self.image[channel][row][col] = read_int()
			return self

	def save(self, path):
		with open(path, 'wb') as f:
			write_int = lambda value: f.write(struct.pack('i', value))
			for row, col, channel in self.index_iter():
				write_int(self.image[channel][row][col])
		return self

	def get_pixel(self, row, col, channel):
		if row in range(self.height) and col in range(self.width) and channel in range(3):
			return self.image[channel][row][col]
		else:
			return 0

	def index_iter(self):
		return product(range(self.height), range(self.width), range(3), repeat=1)

	def convolve(self, kernel):
		image_out = RgbImage(self.height, self.width)
		kernel_width = len(kernel) / 2
		for row_num, cell_num, channel_num in self.index_iter():
			base_row = row_num - kernel_width
			base_col = cell_num - kernel_width
			sum_ = 0
			for row_offset, col_offset in product(range(len(kernel)), repeat=2):
				row = base_row + row_offset
				col = base_col + col_offset
				pix = self.get_pixel(row, col, channel_num)
				sum_ += int(pix * kernel[row_offset][col_offset])
			image_out.image[channel_num][row_num][cell_num] = sum_
		return image_out

	def blur(self):
		return self.convolve(filters['blur 3x3'])


if __name__ == '__main__':
	import timeit
	_, bmpif, shape0, shape1, result_path, benchmark = sys.argv
	run = lambda: RgbImage(shape0, shape1, bmpif).blur().save(result_path)
	with open(benchmark, 'a') as f:
		f.write('ironpy: {}s\n'.format(timeit.timeit(stmt=run, number=1)))

