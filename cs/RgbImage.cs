using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace image_filter_cs_project
{
    class RgbImage
    {
        int height;
        int width;
        public int[,,] image;

        public RgbImage(int height, int width)
        {
            this.height = height;
            this.width = width;
            this.image = new int[height, width, 3];
        }

        public RgbImage(string pathToFile, int height, int width)
        {
            this.height = height;
            this.width = width;
            this.image = new int[height, width, 3];

            using (BinaryReader reader = new BinaryReader(File.Open(pathToFile, FileMode.Open)))
            {
                for (int i = 0; i < height; i++)
                    for (int j = 0; j < width; j++)
                        for (int k = 0; k < 3; k++)
                            image[i, j, k] = reader.ReadInt32();
            }
        }


        public void SaveToFile(string path)
        {
            int errors = 0;
            while (errors < 100)
            {
                try
                {
                    using (BinaryWriter writer = new BinaryWriter(File.Open(path, FileMode.OpenOrCreate)))
                    {
                        for (int i = 0; i < height; i++)
                            for (int j = 0; j < width; j++)
                                for (int k = 0; k < 3; k++)
                                    writer.Write(image[i, j, k]);
                        return;
                    }
                }
                catch (IOException) 
                {
                    errors++;
                    Console.Write(errors);
                    Console.Write("\n");
                }
                Thread.Sleep(100);
            }
        }

        public int GetElement(int row, int col, int channel)
        {
            if (0 <= row && row < this.height && 0 <= col && col < this.width)
                return this.image[row, col, channel];
            return 0;
        }

        private RgbImage Convolve(double[][] kernel)
        {
            RgbImage image = new RgbImage(this.height, this.width);
            int kernel_width = kernel.Length / 2;
            int kernel_sum = 1;

            for (int row_num = 0; row_num < height; row_num++)
                    for (int cell_num = 0; cell_num < width; cell_num++)
                        for (int chanel_num = 0; chanel_num < 3; chanel_num++)
                        {
                            int base_row = row_num - kernel_width,
                                base_col = cell_num - kernel_width,
                                sum = 0;

                            for (int row_offset = 0; row_offset < kernel.Length; row_offset++)
                                for (int col_offset = 0; col_offset < kernel[0].Length; col_offset++)
                                {
                                    int row = base_row + row_offset,
                                        col = base_col + col_offset,
                                        element = this.GetElement(row, col, chanel_num);
                                    sum += (int)(element * kernel[row_offset][col_offset]);
                                }

                            image.image[row_num, cell_num, chanel_num] = sum / kernel_sum;
                        }

            return image;
        }

        public RgbImage Blur()
        {
            double[][] blur_kernel = new double[3][];
            blur_kernel[0] = new double[3];
            blur_kernel[1] = new double[3];
            blur_kernel[2] = new double[3];
            blur_kernel[0][0] = 1.0/16; blur_kernel[0][1] = 1.0/8; blur_kernel[0][2] = 1.0/16;
            blur_kernel[1][0] = 1.0/8;  blur_kernel[1][1] = 1.0/4; blur_kernel[1][2] = 1.0/8;
            blur_kernel[2][0] = 1.0/16; blur_kernel[2][1] = 1.0/8; blur_kernel[2][2] = 1.0/16;
            return this.Convolve(blur_kernel);
        }
    }
}
