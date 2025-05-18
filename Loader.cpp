#include "Loader.h"
#include <random>
#include <DirectXTex.h>

Loader::Loader()
    : m_device(nullptr)
    , m_context(nullptr)
    , m_logoTexture(nullptr)
    , m_progress(0.0f)
    , m_radius(50.0f)
    , m_center(ImVec2(0, 0))
    , m_primaryColor(ImVec4(0.2f, 0.3f, 0.8f, 1.0f))
    , m_accentColor(ImVec4(0.8f, 0.3f, 0.2f, 1.0f))
{
}

Loader::~Loader()
{
    Cleanup();
}

bool Loader::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_device = device;
    m_context = context;

    // Initialize window center (will be updated in Render)
    ImGuiIO& io = ImGui::GetIO();
    m_center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

    // Load logo texture
    if (!LoadLogoTexture(L"logo.png")) {
        // Continue even if logo loading fails
        OutputDebugStringW(L"Failed to load logo texture\n");
    }

    // Initialize snowflakes
    InitializeSnowflakes();

    // Setup theme colors
    SetupTheme();

    return true;
}

void Loader::Update(float deltaTime)
{
    // Update snowflakes
    UpdateSnowflakes(deltaTime);

    // Update progress (demo animation)
    m_progress += deltaTime * 0.2f;
    if (m_progress > 1.0f)
        m_progress = 0.0f;
}

void Loader::Render()
{
    // Update center position
    ImGuiIO& io = ImGui::GetIO();
    m_center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

    // Create fullscreen window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Loader", nullptr, 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoInputs);

    // Render components
    RenderSnowflakes();
    RenderProgressBar();
    RenderLogo();

    ImGui::End();
}

void Loader::Cleanup()
{
    if (m_logoTexture)
    {
        m_logoTexture->Release();
        m_logoTexture = nullptr;
    }
    m_snowflakes.clear();
}

bool Loader::LoadLogoTexture(const wchar_t* filepath)
{
    DirectX::ScratchImage image;
    HRESULT hr = DirectX::LoadFromWICFile(filepath, DirectX::WIC_FLAGS_NONE, nullptr, image);
    if (FAILED(hr))
        return false;

    hr = DirectX::CreateShaderResourceView(m_device, image.GetImages(), image.GetImageCount(), 
                                         image.GetMetadata(), &m_logoTexture);
    return SUCCEEDED(hr);
}

void Loader::SetupTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Set modern style
    style.WindowRounding = 0.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    
    // Set colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.87f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
}

void Loader::InitializeSnowflakes()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> xDist(0, ImGui::GetIO().DisplaySize.x);
    std::uniform_real_distribution<float> yDist(0, ImGui::GetIO().DisplaySize.y);
    std::uniform_real_distribution<float> speedDist(50.0f, 150.0f);
    std::uniform_real_distribution<float> sizeDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> alphaDist(0.3f, 1.0f);

    m_snowflakes.clear();
    for (int i = 0; i < m_maxSnowflakes; ++i)
    {
        Snowflake flake;
        flake.position = ImVec2(xDist(gen), yDist(gen));
        flake.speed = speedDist(gen);
        flake.size = sizeDist(gen);
        flake.alpha = alphaDist(gen);
        m_snowflakes.push_back(flake);
    }
}

void Loader::UpdateSnowflakes(float deltaTime)
{
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> xDist(0, displaySize.x);
    std::uniform_real_distribution<float> alphaDist(0.3f, 1.0f);

    for (auto& flake : m_snowflakes)
    {
        // Update position
        flake.position.y += flake.speed * deltaTime;

        // Reset if off screen
        if (flake.position.y > displaySize.y)
        {
            flake.position.y = -5.0f;
            flake.position.x = xDist(gen);
            flake.alpha = alphaDist(gen);
        }
    }
}

void Loader::RenderSnowflakes()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    for (const auto& flake : m_snowflakes)
    {
        ImU32 color = IM_COL32(255, 255, 255, (int)(255 * flake.alpha));
        draw_list->AddCircleFilled(flake.position, flake.size, color);
    }
}

void Loader::RenderProgressBar()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    // Draw background circle
    draw_list->AddCircle(m_center, m_radius, IM_COL32(100, 100, 100, 100), 32, 2.0f);
    
    // Draw progress arc
    float start_angle = -IM_PI * 0.5f;
    float end_angle = start_angle + (2 * IM_PI * m_progress);
    
    draw_list->PathArcTo(m_center, m_radius, start_angle, end_angle, 32);
    draw_list->PathStroke(IM_COL32(255, 255, 255, 255), false, 2.0f);
}

void Loader::RenderLogo()
{
    if (m_logoTexture)
    {
        float logoSize = m_radius * 1.5f;
        ImVec2 logoPos = ImVec2(m_center.x - logoSize * 0.5f, m_center.y - logoSize * 0.5f);
        ImGui::GetWindowDrawList()->AddImage(
            m_logoTexture,
            logoPos,
            ImVec2(logoPos.x + logoSize, logoPos.y + logoSize)
        );
    }
}
