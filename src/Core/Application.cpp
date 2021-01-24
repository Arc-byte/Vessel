#include "ImGui/ImGuiLayer.h"
#include "glad/glad.h"
#include "pch.h"
#include "Application.h"
#include "Log.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <X11/keysym.h>
namespace Vessel {

#define BIND_EVENT_FN(x)    std::bind(&Application::x,this,std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application(){
        VSL_CORE_ASSERT(!s_Instance,"Application already exists!")
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }
    Application::~Application(){

    }

    void Application::PushLayer(Layer *layer){
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer *layer){
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::OnEvent(Event &e){
        
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

        for(auto it = m_LayerStack.end(); it != m_LayerStack.begin();){
            (*--it)->OnEvent(e);
            if(e.Handled)
                break;
        }
    }

    void Application::Run(){
        while(m_Running){
            glClearColor(1, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            
            for(Layer* layer : m_LayerStack)
                layer->OnUpdate();
            
            auto[x,y] = Input::GetMousePosition();
            VSL_CORE_TRACE("{0}, {1}",x,y);

            m_ImGuiLayer->Begin();
            for(Layer* layer : m_LayerStack)
                layer->OnImGuiRender();
            m_ImGuiLayer->End();
            
            m_Window->OnUpdate();

        }
    }

    bool Application::OnWindowClose(WindowCloseEvent &e){
        m_Running = false;
        return true;
    }
}
