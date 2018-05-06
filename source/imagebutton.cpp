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
	if (Visible) {
		if (m_active) {
			pp2d_texture_select(m_imageId, Location.X, Location.Y);
			pp2d_texture_scale(Scale, Scale);
			pp2d_texture_blend(ActiveColor);
			pp2d_texture_draw();
		}
		else {
			pp2d_draw_texture_scale(m_imageId, Location.X, Location.Y, Scale, Scale);
		}
	}
}

void ImageButton::SetImageId(u32 id) {
	m_imageId = id;
}

u32 ImageButton::GetImageId(void) {
	return m_imageId;
}