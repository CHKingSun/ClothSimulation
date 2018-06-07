//
// Created by KingSun on 2018/05/14
//

#include "Header.h"
#include "./render/BackBuffer.h"
#include "./render/ClothRenderer.h"
#include "./render/EulerClothRenderer.h"
#include "./render/VerletClothRenderer.h"

int main() {
	auto renderer = new KRenderer::VerletClothRenderer();

	renderer->exec();

	delete renderer;
}