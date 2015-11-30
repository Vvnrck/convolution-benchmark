import numpy as np
from PIL import Image


def run_tests():
    assert intermediate_format_is_readable()


def intermediate_format_is_readable():
    import main
    bmp = 'tests.bmp'
    bmpif, shape = main.bmp_to_intermediate_format(bmp)
    bmp_array = np.array(Image.open(bmp))
    bmp_array.reshape(-1, bmp_array.shape[2])
    bmp_array2 = np.fromfile(bmpif, dtype=int)
    bmp_array2 = bmp_array2.reshape(shape)
    return np.all(bmp_array == bmp_array2)


if __name__ == '__main__':
    run_tests()
