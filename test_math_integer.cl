// function index 0
__kernel void math_abs(
	__global __write_only char *abs_data, 
	__global __write_only char *absdif_data,
	__global __read_only char *int1_data,
	__global __read_only char *int2_data
	) 
{
	int id = get_global_id(0);
	abs_data[id] = abs(int1_data[id]);
	absdif_data[id] = abs_diff(int1_data[id],int2_data[id]);
}
// function index 1
__kernel void math_sat(
	__global __write_only char *add_sat_data, 
	__global __write_only char *sub_sat_data,
	__global __read_only char *int1_data,
	__global __read_only char *int2_data
	) 
{
	int id = get_global_id(0);
	add_sat_data[id] = add_sat(int1_data[id], int2_data[id]);
	sub_sat_data[id] = sub_sat(int1_data[id], int2_data[id]);
}
// function index 2
__kernel void math_add_shift(
	__global __write_only char *hadd_data, 
	__global __write_only char *rhadd_data,
	__global __read_only char *int1_data,
	__global __read_only char *int2_data
	) 
{
	int id = get_global_id(0);
	hadd_data[id] =   hadd(int1_data[id], int2_data[id]);
	rhadd_data[id] = rhadd(int1_data[id], int2_data[id]);
}
// function index 3
__kernel void math_clz(
	__global __write_only char *clz_data, 
	__global __read_only char *int_data
	) 
{
	int id = get_global_id(0);
	clz_data[id] = clz(int_data[id]);
}
// function index 4
__kernel void math_mul(
	__global __write_only char *mad_hi_data, 
	__global __write_only char *mad_sat_data,
	__global __read_only char *int1_data,
	__global __read_only char *int2_data,
	__global __read_only char *int3_data
	) 
{
	int id = get_global_id(0);
	mad_hi_data[id] = mad_hi(int1_data[id], int2_data[id], int3_data[id]);
	mad_sat_data[id] = mad_sat(int1_data[id], int2_data[id], int3_data[id]);
}
