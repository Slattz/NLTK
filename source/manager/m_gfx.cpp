#include "common.h"
#include "core/label.h"
#include "core/imagebutton.h"
#include "manager/m_gfx.h"


void draw_manager_main_menu(void)
{
    draw_base_interface();
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon
    C2D_SceneBegin(top);
    draw_centered_text(0, 400, 80, 0, 1.1, 1.1, COLOR_GREY, "Manager Main Menu!");

    if (config.isdebug)
    {
        DrawText(100, 120, 0.5, 0.5, COLOR_GREY, Format("Cursor X: %d, Cursor Y: %d", g_cursorpos.x, g_cursorpos.y).c_str());
        DrawText(100, 140, 0.5, 0.5, COLOR_GREY, Format("Game + Region: 0x%016llX", g_tid).c_str());
        DrawText(100, 160, 0.5, 0.5, COLOR_GREY, Format("Is ACNL: %d", is_ACNL(g_tid)).c_str());
    }
    draw_cursor();
    C3D_FrameEnd(0);
}