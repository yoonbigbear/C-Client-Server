#include "graphics.h"

Graphics::~Graphics()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}


void Graphics::Initialize()
{
    // Setup SDL
    _ASSERT(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER));

    // Decide GL+GLSL versions
#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window_ = SDL_CreateWindow("TestClient", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    gl_context = SDL_GL_CreateContext(window_);
    SDL_GL_MakeCurrent(window_, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // Fog.
    float fogColor[4] = { 0.32f, 0.31f, 0.30f, 1.0f };
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 1000 * 0.1f);
    glFogf(GL_FOG_END, 1000 * 1.25f);
    glFogfv(GL_FOG_COLOR, fogColor);

    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window_, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    camera_.Initialize();
}

void Graphics::Input(SDL_Event* event)
{
    camera_.Input(event);
}

void Graphics::BeginScene()
{
    ImGuiIO& io = ImGui::GetIO();

    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glGetIntegerv(GL_VIEWPORT, camera_.viewport);

    //clear screen
    glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, (double)io.DisplaySize.x / (double)io.DisplaySize.y, 0.1, 1000);
    glGetDoublev(GL_PROJECTION_MATRIX, camera_.projection_matrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(camera_.euler[0], 1, 0, 0);
    glRotatef(camera_.euler[1], 0, 1, 0);
    glTranslatef(-camera_.pos[0], -camera_.pos[1], -camera_.pos[2]);
    glGetDoublev(GL_MODELVIEW_MATRIX, camera_.modelview_matrix);

    GLdouble x, y, z;
    gluUnProject(camera_.mouse_pos[0], camera_.mouse_pos[1], 0.0f,
        camera_.modelview_matrix, camera_.projection_matrix, camera_.viewport, &x, &y, &z);
    camera_.ray_start[0] = (float)x;
    camera_.ray_start[1] = (float)y;
    camera_.ray_start[2] = (float)z;
    gluUnProject(camera_.mouse_pos[0], camera_.mouse_pos[1], 1.0f,
        camera_.modelview_matrix, camera_.projection_matrix, camera_.viewport, &x, &y, &z);
    camera_.ray_end[0] = (float)x;
    camera_.ray_end[1] = (float)y;
    camera_.ray_end[2] = (float)z;

    float keybSpeed = 22.0f;
    if (SDL_GetModState() & KMOD_SHIFT)
    {
        keybSpeed *= 4.0f;
    }

    float movex = (camera_.moveRight - camera_.moveLeft) * keybSpeed * io.DeltaTime;
    float movey = (camera_.moveBack - camera_.moveFront) * keybSpeed * io.DeltaTime + camera_.scrollZoom * 2.0f;
    camera_.scrollZoom = 0;

    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
    {
        camera_.pos[0] += movex * (float)camera_.modelview_matrix[0];
        camera_.pos[1] += movex * (float)camera_.modelview_matrix[4];
        camera_.pos[2] += movex * (float)camera_.modelview_matrix[8];

        camera_.pos[0] += movey * (float)camera_.modelview_matrix[2];
        camera_.pos[1] += movey * (float)camera_.modelview_matrix[6];
        camera_.pos[2] += movey * (float)camera_.modelview_matrix[10];

        camera_.pos[1] += (camera_.moveUp - camera_.moveDown) * keybSpeed * io.DeltaTime;
    }
}


void Graphics::EndScene()
{
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window_);
}
