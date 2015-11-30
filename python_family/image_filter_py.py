import numpy as np
from PIL import Image
import webbrowser
import functools
import sys
import timeit


def image_to_array(image_path : str):
    image = Image.open(image_path)
    array = np.array(image)
    array.reshape(-1, array.shape[2])
    return array


def bmpif_to_array(path, shape):
    bmp_array = np.fromfile(path, dtype=int)
    bmp_array = bmp_array.reshape(shape)
    return bmp_array
        

def convolve(matrix : np.ndarray, kernel : np.ndarray):
    def get_matrix_item(x, y, z):
        xm, ym, zm = matrix.shape
        if (0 <= x < xm) and (0 <= y < ym) and (0 <= z < zm):
            return matrix[x, y, z]
        return 0
    
    matrix_out = np.zeros(matrix.shape, dtype=matrix.dtype)
    kernel_width = len(kernel) // 2
    kernel_sum = kernel.sum()
    for (row_num, cell_num, chanel_num), element in np.ndenumerate(matrix):
        base_row = row_num - kernel_width
        base_col = cell_num - kernel_width
        sum_ = 0
        for (row_offset, col_offset), _ in np.ndenumerate(kernel):
            row = base_row + row_offset
            column = base_col + col_offset
            matrix_elem = get_matrix_item(row, column, chanel_num)
            sum_ += int(matrix_elem * kernel[row_offset, col_offset])
        matrix_out[row_num, cell_num, chanel_num] = int(sum_ / kernel_sum)
    return matrix_out


filters = {
    'emboss': np.array([
        [-2, -1, 0],
        [-1,  1, 1],
        [0,   1, 2]
    ]),
    'blur 3x3': np.array([
        [1/16, 1/8, 1/16],
        [1/8,  1/4, 1/8],
        [1/16, 1/8, 1/16]
    ]),
    'identity': np.array([
        [0, 0, 0],
        [0, 1, 0],
        [0, 0, 0]
    ])
}

# a = image_to_array('1.bmp')
# b = convolve(a, filters['blur 3x3'])
# Image.fromarray(b, mode='RGB').save('2.bmp')
# webbrowser.open('2.bmp')

if __name__ == '__main__':
    # _, bmpif, result_path, bench_path, *shape = sys.argv
    # image = bmpif_to_array(bmpif, [int(i) for i in shape])
    # result = None
    # def _convolve():
    #     global result
    #     result = convolve(image, filters['blur 3x3'])
    # time = timeit.timeit(stmt=_convolve, number=1)
    # result.tofile(result_path)
    # with open(bench_path, 'a') as f:
    #     f.write('python+numpy: {}s\n'.format(time))
    a = image_to_array('1.bmp') 

    import cProfile, pstats, io
    pr = cProfile.Profile()
    pr.enable()
    b = convolve(a, filters['blur 3x3'])
    pr.disable()
    s = io.StringIO()
    sortby = 'cumulative'
    ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
    ps.print_stats()
    print(s.getvalue())

    Image.fromarray(b, mode='RGB').save('2.bmp')
    webbrowser.open('2.bmp')
