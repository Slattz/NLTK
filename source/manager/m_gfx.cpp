#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "utils.h"
#include "InputManager.h"
#include "core/label.h"
#include "core/imagebutton.h"
#include "manager/m_gfx.h"

void draw_manager_main_menu(void)
{
    draw_base_interface();
    C2D_SceneBegin(bottom);
    DrawSprite(Common_ss, NLTK_ICON, 126, 10); //NLTK's Icon
    C2D_SceneBegin(top);
    InputManager::Instance()->DrawCursor();
    C3D_FrameEnd(0);
}