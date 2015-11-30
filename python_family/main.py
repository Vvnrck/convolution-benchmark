import numpy as np
import image_filter as cython_filter
import image_filter_py as python_filter
import image_filter_pypy
import timeit
from subprocess import call
from PIL import Image

IRON_PYTHON_PATH = "D:\Programs\IronPython2.7\ipy.exe"


def bmp_to_intermediate_format(path_to_bmp):
    path_to_bmpif = path_to_bmp + '.bmpif'
    image = Image.open(path_to_bmp)
    array = np.array(image, dtype=int)
    array.reshape(-1, array.shape[2])
    array.tofile(path_to_bmpif)
    return path_to_bmpif, array.shape


def get_benchmark_file_path(path):
    bmark = '{}{}'.format(path, '.speed.txt')
    with open(bmark, 'w') as f:
        f.write('Results:\n')
    return bmark


def main():
    bmpif, shape = bmp_to_intermediate_format('1.bmp')
    benchmark = get_benchmark_file_path(bmpif)
    run_python_code = lambda: run_python(bmpif, '1.bmp.py.bmpif', shape)
    run_cython_code = lambda: cython_filter.blur(b'1.bmp.bmpif', shape[0], shape[1])
    run_pypy_code = lambda: run_pypy(bmpif, '1.bmp.pypy.bmpif', shape)

    with open(benchmark, 'a') as f:
        f.write('python: {}s\n'.format(timeit.timeit(stmt=run_pypy_code, number=1)))
        f.write('python+numpy: {}s\n'.format(timeit.timeit(stmt=run_python_code, number=1)))
        f.write('cython: {}s\n'.format(timeit.timeit(stmt=run_cython_code, number=100) / 100))
    call(['pypy', 'image_filter_pypy.py', bmpif, str(shape[0]), str(shape[1]), '1.bmp.pypy.bmpif', benchmark])
    call([IRON_PYTHON_PATH, 'image_filter_ironpy.py', bmpif, str(shape[0]), str(shape[1]), '1.bmp.ironpy.bmpif', benchmark])
    call(['image_filter_cpp_project.exe', bmpif, str(shape[0]), str(shape[1]), '1.bmp.cpp.bmpif', benchmark])
    call(['image_filter_cs_project.exe', bmpif, str(shape[0]), str(shape[1]), '1.bmp.cs.bmpif', benchmark])


def run_python(bmpif, result_path, shape):
    image = python_filter.bmpif_to_array(bmpif, shape)
    result = python_filter.convolve(image, python_filter.filters['blur 3x3'])
    result.tofile(result_path)


def run_pypy(bmpif, result_path, shape):
    image_filter_pypy.RgbImage(shape[0], shape[1], bmpif).blur().save(result_path)


if __name__ == '__main__':
    import tests
    tests.run_tests()
    main()
