#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "../Renderer.h"
#include "../Camera.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : camera(45.0f, 0.1f, 100.0f) {};

	virtual void OnUpdate(float ts) override {
		camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override {
		glm::vec3 sphColRend = renderer.GetColor();
		ImVec4 sphereColor(sphColRend.r, sphColRend.g, sphColRend.b, 1);
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", lastRenderedTime);
		if (ImGui::Button("Render")) {
			Render();
		}
		if (ImGui::ColorEdit4("Pick Color", (float*)&sphereColor)) {
			renderer.SetColor(glm::vec3(sphereColor.x, sphereColor.y, sphereColor.z));
		}

		glm::vec3 lightDirection = renderer.GetLightDirectionInput();
		if (ImGui::DragFloat3("Light Direction", &lightDirection.x, 0.1f)) {
			renderer.SetLightDirectionInput(lightDirection);
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		viewportWidth = ImGui::GetContentRegionAvail().x;
		viewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = renderer.GetFinalImage();
		if (image) {
			ImGui::Image(
				image->GetDescriptorSet(),
				{ (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0)
			);
		}

		Render();
			
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Render() {
		Timer timer;

		renderer.OnResize(viewportWidth, viewportHeight);
		camera.OnResize(viewportWidth, viewportHeight);
		renderer.Render(camera);
		
		lastRenderedTime = timer.ElapsedMillis();
	}

private:
	Renderer renderer;
	Camera camera;
	uint32_t viewportWidth = 0, viewportHeight = 0;
	float lastRenderedTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}