#include <3ds.h>
#include <string>
#include <citro2d.h>
#include "gfx.h"
#include "imagebutton.h"

ImageButton::ImageButton(void) : Button() { }

ImageButton::ImageButton(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys, u32 imageId, C2D_SpriteSheet sheet)
    : Button(location, size, bgColor, activatorKeys) {

    m_imageId = imageId;
    m_sheet = sheet;
    ActiveColor = COLOR_GREY;
    SetImageTint(ActiveColor);
};

ImageButton::ImageButton(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys, u32 imageId, C2D_SpriteSheet sheet)
    : ImageButton(Point_t{ x, y }, Size_t{ width, height }, bgColor, activatorKeys, imageId, sheet) { }

void ImageButton::Draw(void) {
    if (Visible) {
        if (m_active) {
            DrawSprite(m_sheet, m_imageId, Location.X, Location.Y, ImageTint, ScaleX, ScaleY, ZPos);
        }
        else {
            DrawSprite(m_sheet, m_imageId, Location.X, Location.Y, nullptr, ScaleY, ScaleY, ZPos);
        }
    }
}

void ImageButton::SetImageId(u32 id) {
    m_imageId = id;
}

void ImageButton::SetSpriteSheet(C2D_SpriteSheet sheet) {
    m_sheet = sheet;
}

u32 ImageButton::GetImageId(void) {
    return m_imageId;
}

void ImageButton::SetImageTint(u32 TintColor) {
    ImageTint = new C2D_ImageTint[sizeof(C2D_ImageTint)];
    C2D_PlainImageTint(ImageTint, TintColor, 0.5f);
}

void ImageButton::SetImageTint(C2D_ImageTint* Tint) {
    ImageTint = Tint;
}

void ImageButton::SetScale(float scale) {
    ScaleX = scale;
    ScaleY = scale;
}

void ImageButton::SetScale(float scaleX, float scaleY) {
    ScaleX = scaleX;
    ScaleY = scaleY;
}

C2D_SpriteSheet ImageButton::GetSpriteSheet(void) {
    return m_sheet;
}

C2D_ImageTint* ImageButton::GetImageTint(void) {
    return ImageTint;
}