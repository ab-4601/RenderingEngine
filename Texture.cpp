#include "Texture.h"

Texture::Texture(std::string fileLocation)
	: fileLocation{ fileLocation } {

}

Texture::Texture(const Texture& source) noexcept
	: Texture(source.fileLocation) {

}

Texture::Texture(Texture&& source) noexcept
	: Texture(source.fileLocation) {

	source.~Texture();
}

Texture& Texture::operator=(const Texture& rhs) {
	if (this == &rhs)
		return *this;

	this->fileLocation = rhs.fileLocation;

	return *this;
}

Texture& Texture::operator=(Texture&& rhs) noexcept {
	if (this == &rhs)
		return *this;

	this->fileLocation = rhs.fileLocation;

	rhs.~Texture();

	return *this;
}

bool Texture::loadDDSTexture() {
	if (!glewIsSupported("GL_EXT_texture_compression_s3tc")) return false;

	std::ifstream file(this->fileLocation, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Invalid file location. Error opening file" << std::endl;
		return false;
	}

	DDSHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(header));

	if (header.magic != 0x20534444) {
		std::cerr << "Invalid DDS file" << std::endl;
		file.close();
		return false;
	}

	uint32_t width = header.width;
	uint32_t height = header.height;
	uint32_t mipMapCount = header.mipMapCount ? header.mipMapCount : 1;
	uint32_t fourCC = header.pixelFormat.fourCC;

	GLenum format;
	this->isCompressed = true;

	switch (fourCC) {
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;

		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;

		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;

		case FOURCC_ATI1N:
			format = GL_COMPRESSED_RED_RGTC1;
			break;

		case FOURCC_ATI2N:
			format = GL_COMPRESSED_RG_RGTC2;
			break;

		case FOURCC_BC6H:
			format = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
			break;

		case FOURCC_BC7:
			format = GL_COMPRESSED_RGBA_BPTC_UNORM;
			break;

		default:
			std::cerr << "Unsupported DDS format (invalid fourCC): " << this->fileLocation << std::endl;
			file.close();
			return false;
	}

	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(sizeof(header), std::ios::beg);

	size_t dataSize = fileSize - sizeof(header);
	std::vector<char> buffer(dataSize);
	file.read(buffer.data(), dataSize);
	file.close();

	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	uint32_t blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || format == GL_COMPRESSED_RED_RGTC1) ? 8 : 16;
	uint32_t offset = 0;

	for (uint32_t level = 0; level < mipMapCount && (width || height); ++level) {
		uint32_t size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer.data() + offset);
		offset += size;
		width = std::max(1u, width / 2);
		height = std::max(1u, height / 2);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	buffer.clear();

	return true;
}

bool Texture::loadTexture() {
	unsigned char* texData = stbi_load(this->fileLocation.data(), &this->width, &this->height, &this->bitDepth, 0);

	if (!texData) {
		std::cerr << "Invalid file location. Error loading file" << std::endl;
		return false;
	}

	GLenum format;
	switch (bitDepth) {
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			std::cerr << "Invalid format" << std::endl;
			return false;
	}

	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, this->width, this->height, 0, format, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLfloat maxAnisotropy = 0.f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

	if (maxAnisotropy > 0.f) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min(8.f, maxAnisotropy));
	}

	stbi_image_free(texData);

	return true;
}

bool Texture::makeBindless() {
	if (!glewIsSupported("GL_ARB_bindless_texture")) {
		std::cerr << "Bindless textures not supported" << std::endl;
		return false;
	}

	this->textureHandle = glGetTextureHandleARB(this->textureID);
	glMakeTextureHandleResidentARB(this->textureHandle);

	return true;
}

void Texture::clearTexture() {
	if (this->textureHandle != 0) {
		glMakeTextureHandleNonResidentARB(this->textureHandle);
		this->textureHandle = 0;
	}

	glDeleteTextures(1, &this->textureID);

	this->textureID = 0;
	this->width = 0;
	this->height = 0;
	this->bitDepth = 0;
	this->fileLocation = "";
}

Texture::~Texture() {
	this->clearTexture();
}