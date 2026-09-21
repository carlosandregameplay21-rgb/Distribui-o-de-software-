#include <cstring>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cfloat>   
#include <cstddef>  
#include <vector>
#include <string>

// Includes crus e diretos alinhados com a raiz da sandbox do cofre
#include "SDL.h"
#include "SDL_video.h"
#include "SDL_render.h"
#include "SDL_opengl.h"
#include "SDL_gamepad.h"
#include "SDL_joystick.h"
#include "SDL_events.h"
#include "SDL_init.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

struct Aluno {
    int id;
    char nome[64];
    char telefone[16];
    float valor;
    int diaVencimento;
    bool pago;
};

int main(int, char**) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) return -1;

    SDL_Window* window = SDL_CreateWindow("Gestao de Assinaturas - Personal", 800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        SDL_Quit();
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    std::vector<Aluno> alunos = {
        {1, "Carlos Andre", "999999999", 20.0f, 10, false},
        {2, "Joao da Silva", "888888888", 20.0f, 15, true}
    };

    bool rodando = true;
    while (rodando) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            
            if (event.type == SDL_EVENT_QUIT) {
                rodando = false;
            }
            else if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
                SDL_OpenGamepad(event.gdevice.which);
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(780, 580), ImGuiCond_FirstUseEver);
        ImGui::Begin("Painel de Controle - Assinaturas R$ 20/mes", &rodando, ImGuiWindowFlags_NoCollapse);

        ImGui::Text("Controle de Alunos e Inadimplencia (Zero Planilha, Zero Nuvem)");
        ImGui::Separator();

        if (ImGui::BeginTable("TabelaAlunos", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Nome do Aluno");
            ImGui::TableSetupColumn("Telefone");
            ImGui::TableSetupColumn("Valor");
            ImGui::TableSetupColumn("Vencimento");
            ImGui::TableSetupColumn("Acoes (WhatsApp / Status)");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < alunos.size(); i++) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", alunos[i].id);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", alunos[i].nome);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", alunos[i].telefone);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("R$ %.2f", alunos[i].valor);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("Dia %d", alunos[i].diaVencimento);

                ImGui::TableSetColumnIndex(5);
                if (alunos[i].pago) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
                    std::string label_pago = "Pago##" + std::to_string(i);
                    if (ImGui::Button(label_pago.c_str())) {
                        alunos[i].pago = false;
                    }
                    ImGui::PopStyleColor();
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                    std::string label_devendo = "Devendo##" + std::to_string(i);
                    if (ImGui::Button(label_devendo.c_str())) {
                        alunos[i].pago = true;
                    }
                    ImGui::PopStyleColor();
                }

                ImGui::SameLine();
                std::string label_cobrar = "Cobrar##" + std::to_string(i);
                if (ImGui::Button(label_cobrar.c_str())) {
                    std::string msg = "Ola " + std::string(alunos[i].nome) + ", passando para lembrar da sua mensalidade de R$ 20.00 vencimento dia " + std::to_string(alunos[i].diaVencimento) + ". Segue o PIX!";
                    std::string cmd = "start https://wa.me/55" + std::string(alunos[i].telefone) + "?text=" + msg;
                    std::system(cmd.c_str());
                }
            }
            ImGui::EndTable();
        }

        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
