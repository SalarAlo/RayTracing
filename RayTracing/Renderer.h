#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"

class Renderer
{
public:
	Renderer();

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Camera& camera);

	// Setters
	void SetColor(glm::vec3 color);
	void SetLightDirectionInput(glm::vec3 lightDirection);

	// Getters
	glm::vec3 GetColor() const { return sphereColor; };
	glm::vec3 GetLightDirectionInput() const { return lightDirectionInput; };
	std::shared_ptr<Walnut::Image> GetFinalImage() const { return finalImage; };

private:
	glm::vec4 TraceRay(glm::vec2 uv);

private:
	std::shared_ptr<Walnut::Image> finalImage;
	glm::vec3 sphereColor;
	glm::vec3 lightDirectionInput;
	glm::vec3 lightDirection;
	uint32_t* imageData = nullptr;
};

