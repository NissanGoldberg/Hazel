#pragma once

#ifdef HZ_PLATFORM_WINDOWS

//Hazel::CreateApplication is defined in SandboxApp
extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {
	Hazel::Log::Init();
	
	//Macro defined in Log.h
	HZ_CORE_INFO("Initialized Log!"); //Macro, Same as //Hazel::Log::GetCoreLogger()->warn("Initialized Log!");
	int a = 5;
	HZ_INFO("Hello! Var={0}", a); //Same as //Hazel::Log::GetClientLogger()->info("Hello!");

	std::cout << "Hazel Engine\n";
	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif // HZ_PLATFORM_WINDOWS

