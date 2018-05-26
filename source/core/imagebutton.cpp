#include "imagebutton.h"

ImageButton::ImageButton(void) : Button() { }

ImageButton::ImageButton(Point_t location, Size_t size, u32 bgColor, u32 activatorKeys, u32 imageId)
	: Button(location, size, bgColor, activatorKeys) {

	m_imageId = imageId;
	ActiveColor = COLOR_GREY;
};

ImageButton::ImageButton(u32 x, u32 y, u32 width, u32 height, u32 bgColor, u32 activatorKeys, u32 imageId)
	: ImageButton(Point_t{ x, y }, Size_t{ width, height }, bgColor, activatorKeys, imageId) { }

void ImageButton::Draw(void) {
	C2D_ImageTint* tint;
	C2D_PlainImageTint(tint, ActiveColor, 1.0);
	if (Visible) {
		if (m_active) {
			DrawSprite(m_imageId, Location.X, Location.Y, tint, Scale, Scale);
		}
		else {
			DrawSprite(m_imageId, Location.X, Location.Y, nullptr, Scale, Scale);
		}
	}
}

void ImageButton::SetImageId(u32 id) {
	m_imageId = id;
}

u32 ImageButton::GetImageId(void) {
	return m_imageId;
}