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

    //Add Ln-Kr
    ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL,
        ImGui::GetIO().Fonts->GetGlyphRangesKorean());

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window_, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Camera::instance().Initialize();
}


void Graphics::BeginScene()
{
    ImGuiIO& io = ImGui::GetIO();
    auto& cam = Camera::instance();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glGetIntegerv(GL_VIEWPORT, cam.viewport);

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
    glGetDoublev(GL_PROJECTION_MATRIX, cam.projection_matrix);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(cam.euler[0], 1, 0, 0);
    glRotatef(cam.euler[1], 0, 1, 0);
    glTranslatef(-cam.pos[0], -cam.pos[1], -cam.pos[2]);
    glGetDoublev(GL_MODELVIEW_MATRIX, cam.modelview_matrix);

    GLdouble x, y, z;
    gluUnProject(cam.mouse_pos[0], cam.mouse_pos[1], 0.0f,
        cam.modelview_matrix, cam.projection_matrix, cam.viewport, &x, &y, &z);
    cam.ray_start[0] = (float)x;
    cam.ray_start[1] = (float)y;
    cam.ray_start[2] = (float)z;
    gluUnProject(cam.mouse_pos[0], cam.mouse_pos[1], 1.0f,
        cam.modelview_matrix, cam.projection_matrix, cam.viewport, &x, &y, &z);
    cam.ray_end[0] = (float)x;
    cam.ray_end[1] = (float)y;
    cam.ray_end[2] = (float)z;

    float keybSpeed = 22.0f;
    if (SDL_GetModState() & KMOD_SHIFT)
    {
        keybSpeed *= 4.0f;
    }

    float movex = (cam.moveRight - cam.moveLeft) * keybSpeed * io.DeltaTime;
    float movey = (cam.moveBack - cam.moveFront) * keybSpeed * io.DeltaTime + cam.scrollZoom * 2.0f;
    cam.scrollZoom = 0;

    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
    {
        cam.pos[0] += movex * (float)cam.modelview_matrix[0];
        cam.pos[1] += movex * (float)cam.modelview_matrix[4];
        cam.pos[2] += movex * (float)cam.modelview_matrix[8];

        cam.pos[0] += movey * (float)cam.modelview_matrix[2];
        cam.pos[1] += movey * (float)cam.modelview_matrix[6];
        cam.pos[2] += movey * (float)cam.modelview_matrix[10];

        cam.pos[1] += (cam.moveUp - cam.moveDown) * keybSpeed * io.DeltaTime;
    }
}


void Graphics::EndScene()
{
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window_);
}
