#define BASE64_QUADS_BEFORE_LINEBREAK 19

static unsigned long wby_base64_bufsize(unsigned long input_size) {
	unsigned long triplets = (input_size + 2) / 3;
	unsigned long base_size = 4 * triplets;
	unsigned long line_breaks = 2 * (triplets / BASE64_QUADS_BEFORE_LINEBREAK);
	unsigned long null_termination = 1;
	return base_size + line_breaks + null_termination;
}

int wby_base64_encode(char *output, unsigned long output_size,
	const unsigned char *input, unsigned long input_size) {
	unsigned long i = 0;
	int line_out = 0;
	static const char enc[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/=";

	if (output_size < wby_base64_bufsize(input_size))
		return 1;

	while (i < input_size) {
		unsigned int idx_0, idx_1, idx_2, idx_3;
		unsigned int i0;

		i0 = (unsigned int)(input[i]) << 16; i++;
		i0 |= (unsigned int)(i < input_size ? input[i] : 0) << 8; i++;
		i0 |= (i < input_size ? input[i] : 0); i++;

		idx_0 = (i0 & 0xfc0000) >> 18; i0 <<= 6;
		idx_1 = (i0 & 0xfc0000) >> 18; i0 <<= 6;
		idx_2 = (i0 & 0xfc0000) >> 18; i0 <<= 6;
		idx_3 = (i0 & 0xfc0000) >> 18;

		if (i - 1 > input_size)
			idx_2 = 64;
		if (i > input_size)
			idx_3 = 64;

		*output++ = enc[idx_0];
		*output++ = enc[idx_1];
		*output++ = enc[idx_2];
		*output++ = enc[idx_3];

		if (++line_out == BASE64_QUADS_BEFORE_LINEBREAK) {
		  *output++ = '\r';
		  *output++ = '\n';
		}
	}

	*output = '\0';
	return 0;
}
