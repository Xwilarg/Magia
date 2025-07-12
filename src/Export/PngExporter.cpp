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
            throw std::runtime_error("Fatal error");
        }

        png_init_io(pngPtr, fp);

        png_set_IHDR(pngPtr, infoPtr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        char key[] = "Software";
        char value[] = "Magia";
        png_text metadata[] = {
            { .compression = PNG_TEXT_COMPRESSION_NONE, .key = key, .text = value }
        };
        png_set_text(pngPtr, infoPtr, metadata, 1);

        png_write_info(pngPtr, infoPtr);

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

    void PngExporter::Import(const std::string& path, std::vector<std::shared_ptr<DrawLayer>>& layers)
    {
        FILE* fp = fopen(path.c_str(), "rb");

        if (!fp)
            throw std::runtime_error("Failed to open file");

        unsigned char sig[8];

        fread(sig, 1, 8, fp);
        if (!png_check_sig(sig, 8))
            throw std::runtime_error("Bad signature");

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) throw std::runtime_error("Out of memory");
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_read_struct(&png_ptr, NULL, NULL);
            fclose(fp);
            return throw std::runtime_error("Out of memory");
        }

        /*if (setjmp(png_ptr->jmpbuf)) {
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
            throw std::runtime_error("Fatal error");
        }*/

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);

        png_uint_32 width, height;
        int bit_depth, color_type;
        png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

        layers.clear();
        std::shared_ptr<DrawLayer> layer = std::make_shared<DrawLayer>(width, height);

        png_bytepp row_pointers;

        for (int y = 0; y < height; y++)
        {
            row_pointers = png_get_rows(png_ptr, info_ptr);
            for (int x = 0; x < width; x++)
            {
                auto b = *(row_pointers + x);
                layer->Set(y * width + x, *b);
            }
        }
        layers.push_back(std::move(layer));

        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
    }
}