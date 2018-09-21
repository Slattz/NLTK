#include <3ds.h>
#include <citro2d.h>
#include "gfx.h"
#include "common.h"
#include "cursor.h"

Cursor::Cursor() {
    m_LocX = 160;
    m_LocY = 120;
    m_Visible = true;
    m_ManualVisibility = false;
    m_ActivatorHeld = false;
}

void Cursor::Draw() {
    C2D_SceneBegin(bottom);
    UpdatePos();

    if (m_Visible)
    {
        if (m_ActivatorHeld)
            DrawSprite(Common_ss, CURSOR_SELECT, m_LocX, m_LocY, nullptr, 1.0f, 1.0f, 1.0f);

        else
            DrawSprite(Common_ss, CURSOR_POINT, m_LocX, m_LocY, nullptr, 1.0f, 1.0f, 1.0f);
    }
    C2D_Flush();
}

void Cursor::UpdatePos() {
    hidScanInput();

    if (hidKeysHeld() & KEY_A)
    {
        m_ActivatorHeld = true;
        return;
    }

    else
        m_ActivatorHeld = false;

    if (hidKeysHeld() & KEY_TOUCH) {
        m_Visible = false;
        return;
    }

    //CPad
    if (hidKeysHeld() & KEY_CPAD)
    {
        if (hidKeysHeld() & KEY_CPAD_RIGHT)
            m_LocX += 3;

        if (hidKeysHeld() & KEY_CPAD_LEFT)
            m_LocX -= 3;

        if (hidKeysHeld() & KEY_CPAD_UP)
            m_LocY -= 3;

        if (hidKeysHeld() & KEY_CPAD_DOWN)
            m_LocY += 3;

        m_Visible = true;
    }

    //C-Stick
    if (hidKeysHeld() & KEY_CSTICK)
    {
        if (hidKeysHeld() & KEY_CSTICK_RIGHT)
            m_LocX += 4;

        if (hidKeysHeld() & KEY_CSTICK_LEFT)
            m_LocX -= 4;

        if (hidKeysHeld() & KEY_CSTICK_UP)
            m_LocY -= 4;

        if (hidKeysHeld() & KEY_CSTICK_DOWN)
            m_LocY += 4;

        m_Visible = true;
    }

    if (m_LocY < -5)
        m_LocY = 235;

    if (m_LocY > 235)
        m_LocY = -5;

    if (m_LocX < -5)
        m_LocX = 315;

    if (m_LocX > 315)
        m_LocX = -5;
}