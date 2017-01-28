#include "stdafx.h"


int append_str(bool bCondition, char *pszDest, const char * pszSource, const char *pszSeparator, bool bUseSeparator)
{
	if (bCondition)
	{
		if (bUseSeparator)
			strcat(pszDest, pszSeparator);
		strcat(pszDest, pszSource);
		return 1;
	}
	return 0;
}

void decode_ocl_type(const clTypeID type_id, const size_t size_ret, const union clVal val, char *szOut, int *num_lines)
{
	int iCount;
	int i;

	*num_lines = 0;

	switch (type_id)
	{
	case id_char:
		break;
	case id_char_array:
		*num_lines = 1;
		strcpy(szOut, val.c);
		break;
	case id_cl_bool:
		*num_lines = 1;
		if (val.val_bool == 0)
			strcpy(szOut, "FALSE");
		else
			strcpy(szOut, "TRUE");
		break;

	case id_cl_uint:
		*num_lines = 1;
		wsprintf(szOut, "%u", val.val_uint);
		break;

	case id_cl_ulong:
		*num_lines = 1;
		wsprintf(szOut, "%u", val.val_ulong);
		break;

	case id_size_t:
		*num_lines = 1;
		wsprintf(szOut, "%u", val.val_size_t);
		break;

	case id_size_t_array:
		*num_lines = 1;
		strcpy(szOut, "[");
		for (i = 0; i < (int)size_ret / sizeof(size_t); ++i)
		{
			char buf[20];
			itoa(val.val_size_t_vals[i], buf, 10);
			append_str(true, szOut, buf, ", ", i > 0);
		}
		strcat(szOut, "]");
		break;

	case id_cl_device_fp_config:
		*szOut = 0;
		iCount = 0;

		iCount += append_str(val.val_device_fp_config & CL_FP_DENORM, szOut, "CL_FP_DENORM", "", false);
		iCount += append_str(val.val_device_fp_config & CL_FP_INF_NAN, szOut, "CL_FP_INF_NAN", "\n", iCount > 0);
		iCount += append_str(val.val_device_fp_config & CL_FP_ROUND_TO_NEAREST, szOut, "CL_FP_ROUND_TO_NEAREST", "\n", iCount > 0);
		iCount += append_str(val.val_device_fp_config & CL_FP_ROUND_TO_ZERO, szOut, "CL_FP_ROUND_TO_ZERO", "\n", iCount > 0);
		iCount += append_str(val.val_device_fp_config & CL_FP_ROUND_TO_INF, szOut, "CL_FP_ROUND_TO_INF", "\n", iCount > 0);
		iCount += append_str(val.val_device_fp_config & CL_FP_FMA, szOut, "CL_FP_FMA", "\n", iCount > 0);

		iCount += append_str(val.val_device_fp_config & CL_FP_SOFT_FLOAT, szOut, "CL_FP_SOFT_FLOAT", "\n", iCount > 0);
		iCount += append_str(val.val_device_fp_config & CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT, szOut, "CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT", "\n", iCount > 0);
		*num_lines = iCount;

		break;

	case id_cl_device_exec_capabilities:
		*num_lines = 1;
		*szOut = 0;
		iCount = 0;

		iCount += append_str(val.val_device_exec_capabilities & CL_EXEC_KERNEL, szOut, "CL_EXEC_KERNEL", "\n", false);
		iCount += append_str(val.val_device_exec_capabilities & CL_EXEC_NATIVE_KERNEL, szOut, "CL_EXEC_NATIVE_KERNEL", "\n", iCount > 0);
		*num_lines = iCount;
		break;

	case id_cl_device_mem_cache_type:
		*num_lines = 1;
		*szOut = 0;
		iCount = 0;

		iCount += append_str(val.val_device_mem_cache_type & CL_NONE, szOut, "CL_NONE", "\n", false);
		iCount += append_str(val.val_device_mem_cache_type & CL_READ_ONLY_CACHE, szOut, "CL_READ_ONLY_CACHE", "\n", iCount > 0);
		iCount += append_str(val.val_device_mem_cache_type & CL_READ_WRITE_CACHE, szOut, "CL_READ_WRITE_CACHE", "\n", iCount > 0);
		*num_lines = iCount;
		break;

	case id_cl_platform_id:
		break;

	case id_cl_command_queue_properties:
		*szOut = 0;
		iCount = 0;

		iCount += append_str(val.val_command_queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, szOut, "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE", "\n", iCount > 0);
		iCount += append_str(val.val_command_queue_properties & CL_QUEUE_PROFILING_ENABLE, szOut, "CL_QUEUE_PROFILING_ENABLE", "\n", iCount > 0);
		*num_lines = iCount;
		break;

	case id_cl_device_type:
		*szOut = 0;
		iCount = 0;

		iCount += append_str(val.val_device_type & CL_DEVICE_TYPE_CPU, szOut, "CL_DEVICE_TYPE_CPU", " | ", iCount > 0);
		iCount += append_str(val.val_device_type & CL_DEVICE_TYPE_GPU, szOut, "CL_DEVICE_TYPE_GPU", " | ", iCount > 0);
		iCount += append_str(val.val_device_type & CL_DEVICE_TYPE_ACCELERATOR, szOut, "CL_DEVICE_TYPE_ACCELERATOR", " | ", iCount > 0);
		iCount += append_str(val.val_device_type & CL_DEVICE_TYPE_DEFAULT, szOut, "CL_DEVICE_TYPE_DEFAULT", " | ", iCount > 0);
		*num_lines = iCount;
		break;

	case id_cl_device_partition_property:
		break;
	case id_cl_device_partition_property_array:
		break;
	case id_cl_device_affinity_domain:
		break;
	case id_cl_device_svm_capabilities:
		break;
	case id_cl_device_local_mem_type:
		*szOut = 0;
		iCount = 0;

		iCount += append_str(val.val_device_local_mem_type & CL_LOCAL, szOut, "CL_LOCAL", " | ", false);
		iCount += append_str(val.val_device_local_mem_type & CL_GLOBAL, szOut, "CL_GLOBAL", " | ", iCount > 0);
		*num_lines = iCount;
		break;
	case id_cl_device_id:
		break;
	}
}
