// function index 0
__kernel void math_vector(
	__global __write_only char16 *char16_out,
	__global __write_only short8 *short8_out,
	__global __write_only int4 *int4_out,
	__global __write_only long2 *long2_out,
	__global __write_only float4 *float4_out,

	__global __read_only char16 *char16_in, 
	__global __read_only short8 *short8_in,
	__global __read_only int4 *int4_in,
	__global __read_only long2 *long2_in,
	__global __read_only float4 *float4_in
	) 
{
	int id = get_global_id(0);
	// work with char16
	*char16_out = *char16_in;

	// work with short 8
	short8_out[0].s0=short8_in[0].s0;
	short8_out[0].s3456 = short8_in[0].s4321;
	short8_out[0].s12 = short8_in[0].s65;
	short8_out[0].s7 = short8_in[0].s7;


	// work with int4
	int4_out[0].even = int4_in[0].odd;
	int4_out[0].odd = int4_in[0].even;

	// work with long
	long2_out[0] = long2_in[0];

	//NOTE: This will fail.  It does not appear that Intel supports shuffling LONG vectors
	long2_out[0].s0 = long2_in[0].s1;
	long2_out[0].s1 = long2_in[0].s0;

	// wotk with float4
	float4_out[0].lo = float4_in[0].hi;
	float4_out[0].hi = float4_in[0].lo;
}

// function index 1
__kernel void math_cross_dot(
	__global __write_only float4 *cross_out, 
	__global __write_only float *dot_out,
	__read_only float4 vec1,
	__read_only float4 vec2
	) 
{
	int id = get_global_id(0);

	cross_out[0] = cross(vec1, vec2);
	dot_out[0] = dot(vec1, vec2);
}
// function index 2
__kernel void math_geometry(
	__global __write_only float *distance_out, 
	__global __write_only float *length_out,
	__global __write_only float4 *normalize_out,
	__read_only float4 vec1,
	__read_only float4 vec2
	) 
{
	distance_out[0] = distance(vec1, vec2);
	length_out[0] = length(vec1);
	normalize_out[0] = normalize(vec1);
}
// function index 3
__kernel void math_shuffle(
	__global __write_only float8 *result, 
	__global __read_only uint8 *mask,
	__global __read_only float4 *input
	) 
{
	result[0] = shuffle(input[0], mask[0]);
}
