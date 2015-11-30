from libc.stdio cimport *
from libc.stdlib cimport malloc, free


cdef extern from "stdio.h":
    #FILE * fopen (const char * filename, const char * mode)
    FILE* fopen(const char *, const char *)

    #int fclose (FILE * stream)
    int fclose(FILE *)  

    # size_t fread ( void * ptr, size_t size, size_t count, FILE * stream )
    size_t fread (void*, size_t, size_t, FILE*)  

    # size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream )
    size_t fwrite(const void*, size_t, size_t, FILE*)


cdef:
    struct RgbImage:
        int height
        int width
        int** red
        int** grn
        int** blu


    RgbImage* rgb_image_create(int height, int width):
        cdef RgbImage* obj = <RgbImage*>malloc(sizeof(RgbImage))
        obj.height = height
        obj.width = width
        obj.red = <int**>malloc(height * sizeof(int*))
        obj.grn = <int**>malloc(height * sizeof(int*))
        obj.blu = <int**>malloc(height * sizeof(int*))
        cdef int i
        for i in range(height):
            obj.red[i] = <int*>malloc(width * sizeof(int))
            obj.grn[i] = <int*>malloc(width * sizeof(int))
            obj.blu[i] = <int*>malloc(width * sizeof(int)) 
        return obj


    void rgb_image_populate_from_file(RgbImage* image, char* path):
        cdef FILE* file = fopen(path, 'rb')
        cdef int row, col, *buf
        buf = <int*>malloc(sizeof(int))
        for row in range(image.height):
            for col in range(image.width):
                image.red[row][col] = read_int(file, buf)
                image.grn[row][col] = read_int(file, buf)
                image.blu[row][col] = read_int(file, buf)
        fclose(file)
        free(buf)


    void rgb_image_store_to_file(RgbImage* image, char* path):
        cdef FILE* file = fopen(path, 'wb')
        cdef int row, col
        for row in range(image.height):
            for col in range(image.width):
                fwrite(&image.red[row][col], sizeof(int), 1, file)
                fwrite(&image.grn[row][col], sizeof(int), 1, file)
                fwrite(&image.blu[row][col], sizeof(int), 1, file)
        fclose(file)


    void rgb_image_free(RgbImage* image):
        cdef int row
        for row in range(image.height):
            free(image.red[row])
            free(image.grn[row])
            free(image.blu[row])
        free(image.red)
        free(image.grn)
        free(image.blu)
        free(image)


    void rgb_image_get_pixel(RgbImage* image, int row, int col, int buf[3]):
        buf[0] = 0; buf[1] = 0; buf[2] = 0
        if row >= 0 and row < image.height:
            if col >=0 and col < image.width:
                buf[0] = image.red[row][col]
                buf[1] = image.grn[row][col]
                buf[2] = image.blu[row][col]


    int read_int(FILE* file, int* buf):
        fread(buf, sizeof(int), 1, file)
        return buf[0]

    
    RgbImage* rgb_image_blur_3x3(RgbImage* image):
        cdef double blur_kernel[3][3]
        blur_kernel[0][0] = 1.0/16; blur_kernel[0][1] = 1.0/8; blur_kernel[0][2] = 1.0/16
        blur_kernel[1][0] = 1.0/8;  blur_kernel[1][1] = 1.0/4; blur_kernel[1][2] = 1.0/8 
        blur_kernel[2][0] = 1.0/16; blur_kernel[2][1] = 1.0/8; blur_kernel[2][2] = 1.0/16 

        cdef RgbImage* image_out = rgb_image_create(image.height, image.width)
        cdef int kernel_width = 1, kernel_sum = 1
        cdef int row_num, cell_num, base_row, base_col
        cdef int sum_[3], row_offset, col_offset, matrix_elem
        cdef int row, column
        cdef int pixel[3]

        for row_num in range(image.height):
            for cell_num in range(image.width):
                base_row = row_num - kernel_width
                base_col = cell_num - kernel_width
                sum_[0] = 0; sum_[1] = 0; sum_[2] = 0
                for row_offset in range(3):
                    for col_offset in range(3):
                        row = base_row + row_offset
                        column = base_col + col_offset
                        rgb_image_get_pixel(image, row, column, pixel)
                        sum_[0] += <int>(pixel[0] * blur_kernel[row_offset][col_offset])
                        sum_[1] += <int>(pixel[1] * blur_kernel[row_offset][col_offset])
                        sum_[2] += <int>(pixel[2] * blur_kernel[row_offset][col_offset])
                image_out.red[row_num][cell_num] = sum_[0]
                image_out.grn[row_num][cell_num] = sum_[1]
                image_out.blu[row_num][cell_num] = sum_[2]

        return image_out


def blur(image_path, int image_height, int image_width):
    cdef RgbImage* image = rgb_image_create(image_height, image_width)
    cdef RgbImage* blurred

    rgb_image_populate_from_file(image, image_path)
    blurred = rgb_image_blur_3x3(image)
    rgb_image_store_to_file(blurred, '1.bmp.pyx.bmpif')

    rgb_image_free(image)
    rgb_image_free(blurred)
