
// TODO: Add OpenCL kernel code here.


// function index 1s
/* __kernel void image_julia(write_only image2d_t image, __global read_only float4 *range, __global read_only float2 *point) 
{
	int offset_x = get_global_id(0), 
		offset_y = get_global_id(1);

	uint width = get_image_width(image);
	uint height = get_image_height(image);

	float a1 = range[0].s0, 
		a2 = range[0].s2, 
		b1 = range[0].s1, 
		b2 = range[0].s3,
		da = (a2 - a1) / width,
		db = (b2 - b1) / height;

	float a, b, c, c2, d;
	c = a1 + da * offset_x;
	d = b1 + db * offset_y;
	a = point[0].s0;
	b = point[0].s1;

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
*/

/*
__kernel void image_julia(write_only image2d_t image,  __global read_only float4 *range,  __global read_only float2 *point) 
{
	int offset_x = get_global_id(0), 
		offset_y = get_global_id(1);

	uint width = get_image_width(image);
	uint height = get_image_height(image);

	float a1 = range[0].s0, 
		a2 = range[0].s2, 
		b1 = range[0].s1, 
		b2 = range[0].s3,
		da = (a2 - a1) / width,
		db = (b2 - b1) / height;

	float2 img_point = (a1 + da * offset_x, b1 + db * offset_y);
	float a, b, c, c2, d;
//	c = a1 + da * offset_x;
//	d = b1 + db * offset_y;
	a = point[0].s0;
	b = point[0].s1;

	// (c + di)(c + di) = c^2+2cdi-di
	float4 color = 1.0f;

	int i;
	float2 tmp;
	for (i = 0; i < 1024; ++i)
	{
		tmp = pown(img_point, 2); // calculate (c * c, d * d) 

		if (tmp.s0 + tmp.s1 > 4.0f)
		{
			color = (i / 255.0f, i * 2.0f / 255.0f, i * 3.0f / 255.0f, i * 4.0f / 255.0f);
			break;
		}
//		tmp = pown(img_point, 2); // calculate (c * c, d * d) 
		tmp.s0 -= tmp.s1;			// calculate (c * c - d * d, ??)
		tmp.s1 = 2 * img_point.s0 * img_point.s1; // calculate (c * c - d * d, 2 * c * d
		img_point = tmp + point[0];		// (c * c - d * d + a, 2 8 c * d + b)
	}

	write_imagef(image, (int2)(offset_x, offset_y), color);
}
*/

