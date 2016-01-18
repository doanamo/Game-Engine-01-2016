#include "Precompiled.hpp"
#include "Texture.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogLoadError(filename) "Failed to load a texture from \"" << filename << "\" file! "
    #define LogInitializeError() "Failed to initialize a texture! "
    
    // Invalid types.
    const GLuint InvalidHandle = 0;
    const GLenum InvalidEnum = 0;
}

Texture::Texture(System::ResourceManager* resourceManager) :
    Resource(resourceManager),
    m_handle(InvalidHandle),
    m_width(0),
    m_height(0),
    m_format(InvalidEnum),
    m_initialized(false)
{
}

Texture::~Texture()
{
    if(m_initialized)
        this->Cleanup();
}

void Texture::Cleanup()
{
    // Destroy the texture handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteTextures(1, &m_handle);
        m_handle = InvalidHandle;
    }

    // Reset texture parameters.
    m_width = 0;
    m_height = 0;
    m_format = InvalidEnum;

    // Reset initialization state.
    m_initialized = false;
}

bool Texture::Load(std::string filename)
{
    // Validate arguments.
    if(filename.empty())
    {
        Log() << LogLoadError(filename) << "Invalid argument - \"filename\" is empty.";
        return false;
    }

    // Check the file extenstion.
    if(Utility::GetFileExtension(filename) != "png")
    {
        Log() << LogLoadError(filename) << "Unsupported file extension.";
        return false;
    }

    // Open the file stream.
    std::ifstream file(Build::GetWorkingDir() + filename, std::ios::binary);

    if(!file.is_open())
    {
        Log() << LogLoadError(filename) << "Couldn't open the file.";
        return false;
    }

    // Validate the file header.
    const size_t png_sig_size = 8;
    png_byte png_sig[png_sig_size];

    file.read((char*)png_sig, png_sig_size);

    if(png_sig_cmp(png_sig, 0, png_sig_size) != 0)
    {
        Log() << LogLoadError(filename) << "Not a valid PNG file.";
        return false;
    }
    
    // Create format decoder structures.
    png_structp png_read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if(png_read_ptr == nullptr)
    {
        Log() << LogLoadError(filename) << "Couldn't create PNG read structure.";
        return false;
    }

    png_infop png_info_ptr = png_create_info_struct(png_read_ptr);

    if(png_info_ptr == nullptr)
    {
        Log() << LogLoadError(filename) << "Couldn't create PNG info structure.";
        return false;
    }

    SCOPE_GUARD
    (
        png_destroy_read_struct(&png_read_ptr, &png_info_ptr, nullptr);
    );

    // Declare file read function.
    auto png_read_function = [](png_structp png_ptr, png_bytep data, png_size_t length) -> void
    {
        std::ifstream* stream = (std::ifstream*)png_get_io_ptr(png_ptr);
        stream->read((char*)data, length);
    };

    // Declare image buffers.
    png_bytep* png_row_ptrs = nullptr;
    png_byte* png_data_ptr = nullptr;

    SCOPE_GUARD
    (
        delete [] png_row_ptrs;
        delete [] png_data_ptr;
    );

    // Setup the error handling routine.
    // This is apparently a standard way to handle errors with libpng and some
    // embedded C code. Be aware of how dangerous it is to do this in C++.
    // For ex. objects created past this if() won't have their destructors
    // called if one of libpng functions jumps back here on an error!!!
    // This is the reason why scope guards and other objects that require
    // destruction are declared before this line.
    if(setjmp(png_jmpbuf(png_read_ptr)))
    {
        Log() << LogLoadError(filename) << "An error occurred while reading the file.";
        return false;
    }

    // Setup the file read function.
    png_set_read_fn(png_read_ptr, (png_voidp)&file, png_read_function);

    // Set the amount of already read signature bytes.
    png_set_sig_bytes(png_read_ptr, png_sig_size);

    // Read image info.
    png_read_info(png_read_ptr, png_info_ptr);

    png_uint_32 width = png_get_image_width(png_read_ptr, png_info_ptr);
    png_uint_32 height = png_get_image_height(png_read_ptr, png_info_ptr);
    png_uint_32 depth = png_get_bit_depth(png_read_ptr, png_info_ptr);
    png_uint_32 channels = png_get_channels(png_read_ptr, png_info_ptr);
    png_uint_32 format = png_get_color_type(png_read_ptr, png_info_ptr);

    // Process different format types.
    switch(format)
    {
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        if(depth < 8)
        {
            // Convert gray scale image to single 8bit channel.
            png_set_expand_gray_1_2_4_to_8(png_read_ptr);
            depth = 8;
        }
        break;

    case PNG_COLOR_TYPE_PALETTE:
        {
            // Convert indexed palette to RGB.
            png_set_palette_to_rgb(png_read_ptr);
            channels = 3;

            // Create alpha channel if pallete has transparency.
            if(png_get_valid(png_read_ptr, png_info_ptr, PNG_INFO_tRNS))
            {
                png_set_tRNS_to_alpha(png_read_ptr);
                channels += 1;
            }
        }
        break;

    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGBA:
        break;

    default:
        Log() << LogLoadError(filename) << "Unsupported image format.";
        return false;
    }
    
    // Make sure we only get 8bits per channel.
    if(depth == 16)
    {
        png_set_strip_16(png_read_ptr);
    }

    if(depth != 8)
    {
        Log() << LogLoadError(filename) << "Unsupported image depth size.";
        return false;
    }

    // Allocate image buffers.
    png_row_ptrs = new png_bytep[height];
    png_data_ptr = new png_byte[width * height * channels];

    // Setup an array of row pointers to the actual data buffer.
    png_uint_32 png_stride = width * channels;

    for(png_uint_32 i = 0; i < height; ++i)
    {
        png_uint_32 png_offset = i * png_stride;
        png_row_ptrs[i] = png_data_ptr + png_offset;
    }

    // Read image data.
    png_read_image(png_read_ptr, png_row_ptrs);

    // Determine the texture format.
    GLenum textureFormat = GL_NONE;

    switch(channels)
    {
    case 1:
        textureFormat = GL_R;
        break;

    case 2:
        textureFormat = GL_RG;
        break;

    case 3:
        textureFormat = GL_RGB;
        break;

    case 4:
        textureFormat = GL_RGBA;
        break;

    default:
        Log() << LogLoadError(filename) << "Unsupported number of image channels.";
        return false;
    }

    // Call the initialization method.
    if(!this->Initialize(width, height, textureFormat, png_data_ptr))
    {
        Log() << LogLoadError(filename) << "Initialization failed.";
        return false;
    }

    // Success!
    Log() << "Loaded a texture from \"" << filename << "\" file.";

    return true;
}

bool Texture::Initialize(int width, int height, GLenum format, const void* data)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Validate arguments.
    if(width <= 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"width\" is invalid.";
        return false;
    }

    if(height <= 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"height\" is invalid.";
        return false;
    }

    m_width = width;
    m_height = height;
    m_format = format;

    // Create a texture handle.
    glGenTextures(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        Log() << LogInitializeError() << "Couldn't create a texture.";
        return false;
    }

    // Bind the texture.
    glBindTexture(GL_TEXTURE_2D, m_handle);

    // Set packing aligment for provided data.
    /*
    if(format == GL_R || format == GL_RED)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    */

    // Allocated a texture surface on the hardware.
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);

    // Generate texture mipmap.
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture.
    glBindTexture(GL_TEXTURE_2D, 0);

    // Success!
    return m_initialized = true;
}

void Texture::Update(const void* data)
{
    if(!m_initialized)
        return;

    // Upload new texture data.
    if(data != nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, m_handle);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
