#pragma once
#include <d3d11.h>
#include <vector>
#include "imgui.h"

struct Snowflake {
    ImVec2 position;
    float speed;
    float size;
    float alpha;  // For fade effect
};

class Loader {
public:
    Loader();
    ~Loader();

    bool Init(ID3D11Device* device, ID3D11DeviceContext* context);
    void Update(float deltaTime);
    void Render();
    void Cleanup();

private:
    // DirectX resources
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    ID3D11ShaderResourceView* m_logoTexture;

    // Snowfall system
    std::vector<Snowflake> m_snowflakes;
    const int m_maxSnowflakes = 100;
    
    // Progress bar
    float m_progress;
    float m_radius;
    ImVec2 m_center;
    
    // Theme colors
    ImVec4 m_primaryColor;
    ImVec4 m_accentColor;
    
    // Helper functions
    bool LoadLogoTexture(const wchar_t* filepath);
    void SetupTheme();
    void InitializeSnowflakes();
    void UpdateSnowflakes(float deltaTime);
    void RenderSnowflakes();
    void RenderProgressBar();
    void RenderLogo();
};
