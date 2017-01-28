extern char *program_text[1];
extern size_t program_length[1];
extern TOutput program_output;

bool make_program_path(char *szPathName, int iPathCharacters, char *szProgramName);
bool test_programs(int iPlatformIndex, int iDeviceIndex);
bool load_program(char *program_name);
bool load_program(char *program_name, char **buf, size_t *ret_length); // *buf must be NULL or it will be freed
