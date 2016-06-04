#include <lodepng/lodepng.h>
#include <sstream>
#include <vector>

#include "png_handler.h"

/* Display general info about the PNG */
void PNGHandler::displayPNGInfo(const LodePNGInfo& info) {
	const LodePNGColorMode& color = info.color;

	if (info.compression_method) {
		addMeta("image_compression_method", info.compression_method);
		std::cout << "Compression method: " << info.compression_method << std::endl;
	}

	if (info.filter_method) {
		addMeta("image_filter_method", info.filter_method);
		std::cout << "Filter method: " << info.filter_method << std::endl;
	}

	if (info.interlace_method) {
		addMeta("image_interlace_method", info.interlace_method);
		std::cout << "Interlace method: " << info.interlace_method << std::endl;
	}

	if (color.colortype) {
		addMeta("image_colortype", static_cast<unsigned int>(color.colortype));
		std::cout << "Color type: " << color.colortype << std::endl;
	}

	if (color.bitdepth) {
		addMeta("image_bitdepth", color.bitdepth);
		std::cout << "Bit depth: " << color.bitdepth << std::endl;
	}

	if (lodepng_get_bpp(&color)) {
		addMeta("image_bpp", lodepng_get_bpp(&color));
		std::cout << "Bits per pixel: " << lodepng_get_bpp(&color) << std::endl;
	}

	if (lodepng_get_channels(&color)) {
		addMeta("image_channels", lodepng_get_channels(&color));
		std::cout << "Channels per pixel: " << lodepng_get_channels(&color) << std::endl;
	}

	if (lodepng_is_greyscale_type(&color)) {
		addMeta("image_greyscale", lodepng_is_greyscale_type(&color));
		std::cout << "Is greyscale type: " << lodepng_is_greyscale_type(&color) << std::endl;
	}

	if (lodepng_can_have_alpha(&color)) {
		addMeta("image_allow_alpha", lodepng_can_have_alpha(&color));
		std::cout << "Can have alpha: " << lodepng_can_have_alpha(&color) << std::endl;
	}

	if (color.palettesize) {
		addMeta("image_palette_size", color.palettesize);
		std::cout << "Palette size: " << color.palettesize << std::endl;
	}

	if (color.key_defined) {
		addMeta("image_color_key", color.key_defined);
		addMeta("image_color_key_r", color.key_r);
		addMeta("image_color_key_g", color.key_g);
		addMeta("image_color_key_b", color.key_b);
		std::cout << "Has color key: " << color.key_defined << std::endl;
		std::cout << "Color key r: " << color.key_r << std::endl;
		std::cout << "Color key g: " << color.key_g << std::endl;
		std::cout << "Color key b: " << color.key_b << std::endl;
	}

	std::cout << "Texts: " << info.text_num << std::endl;
	for (size_t i = 0; i < info.text_num; i++) {
		addMeta("image_" + std::string(info.text_keys[i]), info.text_strings[i]);
		std::cout << "Text: " << info.text_keys[i] << ": " << info.text_strings[i] << std::endl << std::endl;
	}

	std::cout << "International texts: " << info.itext_num << std::endl;
	for (size_t i = 0; i < info.itext_num; i++) {
		addMeta("image_" + std::string(info.text_keys[i]), info.text_strings[i]);
		std::cout << "Text: "
		          << info.itext_keys[i] << ", "
		          << info.itext_langtags[i] << ", "
		          << info.itext_transkeys[i] << ": "
		          << info.itext_strings[i] << std::endl << std::endl;
	}

	std::cout << "Time defined: " << info.time_defined << std::endl;
	if (info.time_defined) {
		const LodePNGTime& time = info.time;
		std::stringstream ss;
		ss << time.year << "-" << time.month << "-" << time.day << " " << time.hour << ":" << time.minute << ":" << time.second;
		addMeta("image_timestamp", ss.str());
		std::cout << "year: " << time.year << std::endl;
		std::cout << "month: " << time.month << std::endl;
		std::cout << "day: " << time.day << std::endl;
		std::cout << "hour: " << time.hour << std::endl;
		std::cout << "minute: " << time.minute << std::endl;
		std::cout << "second: " << time.second << std::endl;
	}

	std::cout << "Physics defined: " << info.phys_defined << std::endl;
	if (info.phys_defined) {
		addMeta("image_physics_x", info.phys_x);
		addMeta("image_physics_y", info.phys_y);
		addMeta("image_physics_unit", info.phys_unit);
		std::cout << "physics X: " << info.phys_x << std::endl;
		std::cout << "physics Y: " << info.phys_y << std::endl;
		std::cout << "physics unit: " << info.phys_unit << std::endl;
	}
}

void PNGHandler::handle() {
	const ScrapeData::Data *data = getPayload();

	// std::cout << "Item[" << getId() << "] handler [png]" << std::endl;

	lodepng::State state;
	std::vector<unsigned char> image;
	std::vector<unsigned char> buffer(data->payload().begin(), data->payload().end());
	unsigned width, height;

	unsigned error = lodepng::decode(image, width, height, state, reinterpret_cast<const unsigned char *>(data->payload().c_str()), data->payload().size());
	if (error) {
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	std::cout << "Filesize: " << buffer.size() << " (" << buffer.size() / 1024 << "K)" << std::endl;
	addMeta("image_width", reinterpret_cast<unsigned int>(width));
	std::cout << "Width: " << width << std::endl;
	addMeta("image_height", reinterpret_cast<unsigned int>(height));
	std::cout << "Height: " << height << std::endl;
	addMeta("image_pixels", reinterpret_cast<unsigned int>(width * height));
	std::cout << "Num pixels: " << width * height << std::endl;

	displayPNGInfo(state.info_png);
}
