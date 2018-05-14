#include "common.h"
#include "core/label.h"
#include "core/imagebutton.h"
#include "manager/m_gfx.h"


void draw_manager_main_menu(void)
{
    draw_base_interface();
    pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
    pp2d_draw_texture(NLTK_ICON, 126, 10); //NLTK's Icon
    pp2d_draw_on(GFX_TOP, GFX_LEFT);
    pp2d_draw_text_center(GFX_TOP, 80, 1.1, 1.1, COLOR_GREY, "Manager Main Menu!");

    if (config.isdebug)
    {
        pp2d_draw_textf(100, 120, 0.5, 0.5, COLOR_GREY, "Cursor X: %d, Cursor Y: %d", g_cursorpos.x, g_cursorpos.y);
        pp2d_draw_textf(100, 140, 0.5, 0.5, COLOR_GREY, "Game + Region: 0x%016llX", g_tid);
        pp2d_draw_textf(100, 160, 0.5, 0.5, COLOR_GREY, "Is ACNL: %d", is_ACNL(g_tid));
    }
    draw_cursor();
    pp2d_end_draw();
}