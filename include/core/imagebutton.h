#pragma once

#include "button.h"

class ImageButton : public Button {
public:
    ImageButton(void);
    ImageButton(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys, u32 imageId, C2D_SpriteSheet sheet);
    ImageButton(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys, u32 imageId, C2D_SpriteSheet sheet);
    
    void Draw(void);
    void SetImageId(u32 id);
    void SetSpriteSheet(C2D_SpriteSheet sheet);
    u32 GetImageId(void);
    void SetImageTint(u32 TintColor);
    void SetImageTint(C2D_ImageTint* Tint);
    void SetScale(float scale);
    C2D_SpriteSheet GetSpriteSheet(void);
    C2D_ImageTint*  GetImageTint(void);

protected:
    u32 m_imageId = 0;
    C2D_SpriteSheet m_sheet;
    C2D_ImageTint* ImageTint;
    float Scale = 1;
    u32 ActiveColor;
};