#include "Renderer.h"
#include "Walnut/Random.h"
#include <chrono>

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g* 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

}

Renderer::Renderer() {
	this->SetLightDirectionInput(glm::vec3(-1, -1, 1));
	this->SetColor(glm::vec3(0.8f, 0.2f, 1));
}

void Renderer::OnResize(uint32_t width, uint32_t height) {
	if (finalImage) {
		bool noResizeNecessary = finalImage->GetWidth() == width && finalImage->GetHeight() == height;
		if (noResizeNecessary) return;
		finalImage->Resize(width, height);
	}
	else {
		finalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] imageData;
	imageData = new uint32_t[height * width];
}


void Renderer::Render(const Camera& camera) {
	// render every pixel
	const glm::vec3& rayOrigin = camera.GetPosition();;
	for (uint32_t y = 0; y < finalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < finalImage->GetWidth(); x++) {		
			uint32_t idx = x + y * finalImage->GetWidth();
			const glm::vec3& rayDirection = camera.GetRayDirections()[idx];
			auto color = TraceRay(coord);

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			imageData[idx] = Utils::ConvertToRGBA(color);
		}
	}
	
	finalImage->SetData(imageData);
}

void Renderer::SetColor(glm::vec3 color) {
	sphereColor = color;
}

void Renderer::SetLightDirectionInput(glm::vec3 lightDirection)
{
	this->lightDirectionInput = lightDirection;
	this->lightDirection = glm::normalize(lightDirectionInput);
	this->lightDirection.z = -(this->lightDirection.z);
}

glm::vec4 Renderer::TraceRay(glm::vec2 uv) {
	float radius = 0.5f;
	float distance = 0.0f;
	float aspectRatio = finalImage->GetWidth() / (float)finalImage->GetHeight();
	uv.x *= aspectRatio;

	glm::vec3 sphereCenter    (0.0f, 0.0f, distance);

	glm::vec3 rayOrigin       (0.0f, 0.0f, 1.0f);
	glm::vec3 rayDirection    (uv.x, uv.y, -1);


	float a = glm::dot        (rayDirection, rayDirection);
	float b = 2.0f * glm::dot (rayOrigin - sphereCenter, rayDirection);
	float c = glm::dot        (rayOrigin - sphereCenter, rayOrigin - sphereCenter) - radius * radius;

	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) return glm::vec4(0, 0, 0, 1);

	float t = (-b - glm::sqrt(discriminant)) / (2 * a);

	glm::vec3 intersectionPoint(rayOrigin + t * rayDirection);
	glm::vec3 normal = intersectionPoint - sphereCenter;
	normal = glm::normalize(normal);

	float lightingFactor = glm::dot(normal, -lightDirection);
	float brightness = lightingFactor < 0 ? 0 : lightingFactor;

	return glm::vec4(sphereColor * brightness, 1);
}
