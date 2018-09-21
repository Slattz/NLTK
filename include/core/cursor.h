#pragma once

class Cursor {

public:
    Cursor(void);
    void Draw();
    s16 m_LocX;
    s16 m_LocY;
    bool m_Visible;

  private:
    void UpdatePos();

    bool m_ManualVisibility;
    bool m_ActivatorHeld;
};