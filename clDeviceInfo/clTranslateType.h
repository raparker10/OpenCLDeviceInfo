#include <CL\cl.h>

enum clTypeID {
	id_char = 1,
	id_char_array,
	id_cl_bool,
	id_cl_uint,
	id_cl_ulong,
	id_size_t,
	id_size_t_array,
	id_cl_device_fp_config,
	id_cl_device_exec_capabilities,
	id_cl_device_mem_cache_type,
	id_cl_platform_id,
	id_cl_command_queue_properties,
	id_cl_device_type,
	id_cl_device_partition_property,
	id_cl_device_partition_property_array,
	id_cl_device_affinity_domain,
	id_cl_device_svm_capabilities,
	id_cl_device_local_mem_type,
	id_cl_device_id,
	id_cl_device_id_array,
	id_cl_context_properties, 
	id_cl_context_properties_array
};

union clVal {
	cl_uint val_uint;
	cl_bool val_bool;
	cl_device_fp_config val_device_fp_config;
	cl_device_exec_capabilities val_device_exec_capabilities;
	char c[1024];
	cl_ulong val_ulong;
	cl_device_mem_cache_type val_device_mem_cache_type;
	size_t val_size_t;
	size_t val_size_t_vals[128];
	cl_platform_id val_platform_id;
	cl_command_queue_properties val_command_queue_properties;
	cl_device_type val_device_type;
	cl_device_partition_property val_device_partition_property_array;
	cl_device_affinity_domain val_device_affinity_domain;
	cl_device_svm_capabilities val_device_svm_capabilities;
	cl_device_local_mem_type val_device_local_mem_type;
	cl_device_id val_device_id;
};

int append_str(bool bCondition, char *pszDest, const char * pszSource, const char *pszSeparator, bool bUseSeparator);
void decode_ocl_type(const clTypeID type_id, const size_t size_ret, const union clVal val, char *szOut, int *num_lines);
