#include "MouseSelector.h"

MouseSelector::MouseSelector(unsigned int windowWidth, unsigned int windowHeight) {
	this->pixelInfo.init(windowWidth, windowHeight);
}

void MouseSelector::pickingPhase(std::vector<Mesh*>& meshes, GLuint framebuffer) 
{
	pixelInfo.enableWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.useShader();

	for (size_t i = 0; i < meshes.size(); i++) {
		shader.setUint("objectIndex", meshes[i]->getObjectID() + 1);
		shader.setUint("drawIndex", static_cast<uint>(i));
		shader.setMat4("model", meshes[i]->getModelMatrix());

		meshes[i]->drawMesh(GL_TRIANGLES);
	}

	pixelInfo.disableWriting(framebuffer);

	shader.endShader();
}

int MouseSelector::mouseSelectionResult(int windowHeight, int x, int y) {
	int clickedObjectID = -1;
	
	SelectionTexture::PixelInfo pixel = pixelInfo.readPixel(x, windowHeight - y - 1);

	if (pixel.objectID != 0) {
		clickedObjectID = pixel.objectID - 1;
		
		return clickedObjectID;
	}

	return -1;
}