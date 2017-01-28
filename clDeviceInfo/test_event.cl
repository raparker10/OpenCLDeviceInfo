// function index 0
__kernel void image_mandelbrot(write_only image2d_t image, __global read_only float4 *range) 
{
	int offset_x = get_global_id(0), 
		offset_y = get_global_id(1);

	uint width = get_image_width(image);
	uint height = get_image_height(image);

	float a1 = range[0].s0, 
		b1 = range[0].s1, 
		a2 = range[0].s2, 
		b2 = range[0].s3,
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
		if (c * c + d * d > 4.0f)
		{
			color = (i / 255.0f, i * 2.0f / 255.0f, i * 3.0f / 255.0f, i * 4.0f / 255.0f);
			break;
		}
	}

	write_imagef(image, (int2)(offset_x, offset_y), color);
}


__kernel void image_julia_scalar(
	write_only image2d_t image,  
	__global read_only float4 *range,  
	__global read_only float2 *point, 
	__global read_only float2 *dadb) 
{
	int offset_x = get_global_id(0), 
		offset_y = get_global_id(1);

	float a, b, c, c2, d;
	c = range[0].s0 + dadb[0].s0 * offset_x;
	d = range[0].s1 + dadb[0].s1 * offset_y;

	// these are constant across all pixels and should be optimized
	a = point[0].s0;
	b = point[0].s1;

	// (c + di)(c + di) = c^2+2cdi-di
	float4 color = (1.0f, 1.0f, 1.0f, 1.0f);

	int i;
	for (i = 0; i < 255; ++i)
	{
		float csqu = c * c;
		float dsqu = d * d;
		if (csqu + dsqu > 4.0f)
		{
			color = (i / 255.0f, i * 2.0f / 255.0f, i * 3.0f / 255.0f, i * 4.0f / 255.0f);
			break;
		}
		c2 = csqu - dsqu + a;
		d = 2 * c * d + b;
		c = c2;
	}

	write_imagef(image, (int2)(offset_x, offset_y), color);
}
