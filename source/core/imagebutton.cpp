#include "imagebutton.h"

ImageButton::ImageButton(void) : Button() { }

ImageButton::ImageButton(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys, u32 imageId, C2D_SpriteSheet sheet)
	: Button(location, size, bgColor, activatorKeys) {

	m_imageId = imageId;
	m_sheet = sheet;
	ActiveColor = COLOR_GREY;
};

ImageButton::ImageButton(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys, u32 imageId, C2D_SpriteSheet sheet)
	: ImageButton(Point_t{ x, y }, Size_t{ width, height }, bgColor, activatorKeys, imageId, sheet) { }

void ImageButton::Draw(void) {
	C2D_ImageTint* tint;
	C2D_PlainImageTint(tint, ActiveColor, 1.0);
	if (Visible) {
		if (m_active) {
			DrawSprite(m_sheet, m_imageId, Location.X, Location.Y, tint, Scale, Scale);
		}
		else {
			DrawSprite(m_sheet, m_imageId, Location.X, Location.Y, nullptr, Scale, Scale);
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

C2D_SpriteSheet ImageButton::GetSpriteSheet(void) {
	return m_sheet;
}