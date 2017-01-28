// function index 0
__kernel void math_sin(__global __write_only float *sin_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	sin_data[id] = sin(radian_data[id]);
}
// function index 1
__kernel void math_cos(__global __write_only float *cos_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	cos_data[id] = cos(radian_data[id]);
}
// function index 2
__kernel void math_tan(__global __write_only float *tan_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	tan_data[id] = tan(radian_data[id]);
}
// function index 3
__kernel void math_sinh(__global __write_only float *sinh_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	sinh_data[id] = sinh(radian_data[id]);
}
// function index 4
__kernel void math_cosh(__global __write_only float *cosh_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	cosh_data[id] = cosh(radian_data[id]);
}
// function index 5
__kernel void math_tanh(__global __write_only float *tanh_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	tanh_data[id] = tanh(radian_data[id]);
}
// function index 6
__kernel void math_sinpi(__global __write_only float *sin_data, __global __read_only float *pifrac_data) 
{
	int id = get_global_id(0);
	sin_data[id] = sinpi(pifrac_data[id]);
}
// function index 7
__kernel void math_cospi(__global __write_only float *cos_data, __global __read_only float *pifrac_data) 
{
	int id = get_global_id(0);
	cos_data[id] = cospi(pifrac_data[id]);
}
// function index 8
__kernel void math_tanpi(__global __write_only float *tan_data, __global __read_only float *pifrac_data) 
{
	int id = get_global_id(0);
	tan_data[id] = tanpi(pifrac_data[id]);
}
// function index 9
__kernel void math_native_sin(__global __write_only float *sin_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	sin_data[id] = native_sin(radian_data[id]);
}
// function index 10
__kernel void math_native_cos(__global __write_only float *cos_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	cos_data[id] = native_cos(radian_data[id]);
}
// function index 11
__kernel void math_native_tan(__global __write_only float *tan_data, __global __read_only float *radian_data) 
{
	int id = get_global_id(0);
	tan_data[id] = native_tan(radian_data[id]);
}
// function index 12
__kernel void math_exp(
	__global __write_only float *exp_data, 
	__global __write_only float *exp2_data, 
	__global __write_only float *exp10_data, 
	__global __read_only float *exponent_data)
{
	int id = get_global_id(0);
	exp_data[id] = exp(exponent_data[id]);
	exp2_data[id] = exp2(exponent_data[id]);
	exp10_data[id] = exp10(exponent_data[id]);
}
// function index 13
__kernel void math_sincos(
	__global __write_only float *sin_data, 
	__global __write_only float *cos_data, 
	__global __read_only float *radian_data)
{
	int id = get_global_id(0);
	sin_data[id]=sincos(radian_data[id], &cos_data[id]);
}
