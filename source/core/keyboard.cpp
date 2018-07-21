#include "keyboard.h"

Keyboard::Keyboard(void) { }

Keyboard::~Keyboard(void) { }

Keyboard::Keyboard(const std::string &HintText, const std::string &DefaultText) {
    Text Msg(COLOR_GREY, HintText, 1.f, 1.f);
    m_Text = DefaultText;
    m_HintText = Msg;
}

void Keyboard::SetHint(const std::string &HintText) {
    m_HintText = HintText;
}

void Keyboard::SetText(const std::string &DefaultText) {
    m_Text = DefaultText;
}

void Keyboard::CanAbort(bool canAbort) {
    m_CanAbort = canAbort;
}

void Keyboard::SetMaxLength(u32 maxLength) {
    m_MaxLength = maxLength;
}

void Keyboard::Draw() {
    draw_base_interface();
    C2D_SceneBegin(bottom);
    if (!m_TopDrawn) {
        //C2D_DrawRectSolid(10, 30, 0.5f, 380, 200, C2D_Color32(0, 0, 0, 150));
        //m_HintText.CenterInBounds(15, 35, 375, 195);
        //m_HintText.Draw();
        //m_TopDrawn = true;
    }
    C2D_SceneBegin(bottom);
    C2D_DrawRectSolid(5.f, 30.f, 0.f, 310.f, 205.f, COLOR_BROWN); //Keyboard Background
    C2D_DrawRectSolid(0.f, 0.f, 0.f, 320.f, 25.f, COLOR_MENU_BARS); //Bar for Hint Text Background
    m_HintText.CenterInBounds(2.5f, 2.5f, 317.5f, 22.5f);
    m_HintText.Draw();
    C3D_FrameEnd(0);
}

int Keyboard::Open(std::string &output) {
    m_Type = KeyboardType::Text;
    while (aptMainLoop()) {
        Draw();
        hidScanInput();
        if (hidKeysDown() & KEY_B && m_CanAbort) {
            return -1; //user aborted
        }
    }

    return 0;
}