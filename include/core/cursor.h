#pragma once

class Cursor {

public:
    Cursor(void);
    void Draw();
    void SetVisibility(bool visible);
    void RemoveManualVisibility(void);

    s16 m_LocX;
    s16 m_LocY;
    bool m_Visible;

  private:
    void UpdatePos();

    bool m_ManualVisibility; //Allows for a visibility to be set manually
    bool m_ActivatorHeld;
};