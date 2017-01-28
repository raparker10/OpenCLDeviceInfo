
// TODO: Add OpenCL kernel code here.
// this should work
__kernel void mem_read_write(__global __read_write char16 *msg) 
{
	*msg = (char16)('R', 'e', 'a', 'd', ' ','W','r', 'i', 't', 'e', '.', '.', '.', '.', '.','\0');
}

// this should work
__kernel void mem_write_only(__global __write_only char16 *msg) 
{
	*msg = (char16)('W', 'r', 'i', 't', 'e',' ','O', 'n', 'l', 'y', '.', '.', '.', '.','.','\0');
}

// the write attempt should fail
__kernel void mem_read_only(__global __read_only char16 *msg) 
{
	*msg = (char16)('R', 'e', 'a', 'd', ' ','O','n', 'l', 'y', '.', '.', '.', '.', '.','.','\0');
}


__kernel void mem_read_write_use_host_ptr(__global char16 *msg) 
{
	*msg = (char16)('U', 's', 'e', ' ', 'H','o','s', 't', ' ', 'P', 't', 'r', '.', '.','.','\0');
}

__kernel void mem_read_write_copy_host_ptr(__global char16 *msg) 
{
	*msg = (char16)('C', 'o', 'p', 'y', ' ','H','o','s', 't', ' ', 'P', 't', 'r', '.','.','\0');
}

__kernel void mem_read_write_alloc_host_ptr(__global char16 *msg) 
{
	*msg = (char16)('A', 'l', 'l', 'o', 'c',' ','H','o','s', 't', ' ', 'P', 't', 'r', '.','\0');
}

__kernel void add_vectors(__global __write_only float *out_data, __global __read_only float *in_data) 
{
	int id = get_local_id(0);
	in_data[id]=2.0f*in_data[id]; // try writing to read_only memory
	out_data[id]=in_data[id];
}
__kernel void write_image(write_only image2d_t dest_image) 
{
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 pixel;
	pixel.x = 1; // (uint)get_global_id(0);
	pixel.y = 2; //(uint)get_global_id(1) * 12;
	pixel.z = 3; //(uint)pixel.x * pixel y;
	pixel.w = 4; // (u0;
	write_imageui(dest_image, coord, pixel);
}

__kernel void copy_image(read_only image2d_t src_image, write_only image2d_t dest_image) 
{
	__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	uint offset = get_global_id(0) * 5;
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	uint4 pixel = read_imageui(src_image, sampler, coord);
//	pixel.x = 123; pixel.y=124; pixel.z=125; pixel.w=126; // += offset;
	write_imageui(dest_image, coord, pixel);
}
