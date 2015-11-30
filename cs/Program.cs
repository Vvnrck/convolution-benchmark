using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;

namespace image_filter_cs_project
{
    class Program
    {
        const string docString = "Blur a .bmpif image.\n" +
                                 "Application arguments:\n" +
                                 "inputFile, height, width, outputFile, benchmarkOutputFile";

        static void Main(string[] args)
        {
            try
            {
                string inputFile = args[0],
                       height = args[1],
                       width = args[2],
                       outputFile = args[3],
                       benchmarkFile = args[4];

                Stopwatch sw = Stopwatch.StartNew();
                for (var _ = 0; _ < 100; _++)
                {
                    RgbImage image = new RgbImage(inputFile, Int32.Parse(height), Int32.Parse(width));
                    image.Blur().SaveToFile(outputFile);
                }
                sw.Stop();
                File.AppendAllText(benchmarkFile, "C#: " + (sw.Elapsed.TotalSeconds / 100.0).ToString() + "s." + Environment.NewLine);
            }
            catch (IndexOutOfRangeException)
            {
                Console.Write(Program.docString);
            }            
        }
    }
}
