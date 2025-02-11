#include <stdexcept>

#include "png.h"
#include "PngExporter.hpp"

namespace Magia
{
	void PngExporter::Export(const std::string& path, int width, int height, uint32_t* pixels)
	{
        FILE* fp = fopen(path.c_str(), "wb");

        if (!fp)
            throw std::runtime_error("Failed to open file");

        png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

        if (!pngPtr)
            throw std::runtime_error("Failed to init");

        png_infop infoPtr = png_create_info_struct(pngPtr);
        if (!infoPtr)
        {
            png_destroy_write_struct(&pngPtr, nullptr);
            throw std::runtime_error("Failed to init");
        }

        if (setjmp(png_jmpbuf(pngPtr))) // TODO: See how to use png_abort instead?
        {
            png_destroy_write_struct(&pngPtr, &infoPtr);
            fclose(fp);
            throw std::runtime_error("Error");
        }

        png_init_io(pngPtr, fp);

        // TODO: png_set_write_status_fn(png_ptr, write_row_callback);

        png_set_IHDR(pngPtr, infoPtr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        // TODO: Fill text with software name

        png_write_info(pngPtr, infoPtr);

        //png_write_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);

        png_byte** rowPointers;
        rowPointers = reinterpret_cast<png_byte**>(malloc(sizeof(png_byte*) * height));
        for (int y = 0; y < height; y++)
        {
            rowPointers[y] = reinterpret_cast<png_byte*>(malloc(sizeof(png_byte) * width * 4));
            for (int x = 0; x < width; x++)
            {
                rowPointers[y][x * 4] = (pixels[y * width + x] >> 24) & 0xFF;
                rowPointers[y][x * 4 + 1] = (pixels[y * width + x] >> 16) & 0xFF;
                rowPointers[y][x * 4 + 2] = (pixels[y * width + x] >> 8) & 0xFF;
                rowPointers[y][x * 4 + 3] = pixels[y * width + x] & 0xFF;
            }
        }

        png_write_image(pngPtr, rowPointers);
        png_write_end(pngPtr, infoPtr);

        png_destroy_write_struct(&pngPtr, &infoPtr);
        fclose(fp);
	}
}