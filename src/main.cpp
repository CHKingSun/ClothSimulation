//
// Created by KingSun on 2018/05/14
//

#include "Header.h"
#include "./render/ClothRenderer.h"

int main() {
	auto renderer = new KRenderer::ClothRenderer();

	renderer->exec();

	delete renderer;
}