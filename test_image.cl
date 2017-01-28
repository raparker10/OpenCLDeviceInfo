constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

// function index 0
__kernel void image_set(
	write_only image2d_t image
	) 
{
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 color = (uint4)(0x0AA, 0x0FF, 0x055, 0x01);
	write_imageui(image, coord, color);
}

// function index 1
__kernel void image_copy(
	write_only image2d_t image_out,
	read_only image2d_t image_in
	) 
{
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 color = read_imageui(image_in, sampler, coord);
	write_imageui(image_out, coord, color);
}
// function index 2
constant sampler_t sampler_linear = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;

__kernel void image_interpolate(
	write_only image2d_t image_out_large,
	read_only image2d_t image_in
	) 
{
	float2 source_coord = (float2)(
			1.0f * get_global_id(0) / get_image_width(image_out_large), 
			1.0f * get_global_id(1) / get_image_height(image_out_large)
			);

	float4 color = read_imagef(image_in, sampler_linear, source_coord);
	int2 dest_coord = (int2)(get_global_id(0), get_global_id(1));

	write_imagef(image_out_large, dest_coord, color);
}
// function index 3
__kernel void image_mandelbrot(write_only image2d_t image) 
{
	int offset_x = get_global_id(0), 
		offset_y = get_global_id(1);

	uint width = get_image_width(image);
	uint height = get_image_height(image);

	float a1 = -2.0f, 
		a2 = 0.0f, 
		b1 = -2.0f, 
		b2 = 0.0f,
		da = (a2 - a1) / width,
		db = (b2 - b1) / height;

	float a, b, c, c2, d;
	c = d = 0.0f;
	a = a1 + da * offset_x;
	b = b1 + db * offset_y;

	// (c + di)(c + di) = c^2+2cdi-di
	float4 color = (1.0f, 1.0f, 1.0f, 1.0f);

	int i;
	for (i = 0; i < 255; ++i)
	{
		c2 = c * c - d * d + a;
		d = 2 * c * d + b;
		c = c2;
		if (c * c + d * d > 4.0)
		{
			color = (i / 255.0, i * 2.0 / 255.0, i * 3.0 / 255.0, i * 4.0 / 255.0);
			break;
		}
	}

	write_imagef(image, (int2)(offset_x, offset_y), color);
}
