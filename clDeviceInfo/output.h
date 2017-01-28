
class TOutput {
private:
	char **pszLines;
	int iLineCount;
	int iCurrentLine;
public:
	TOutput();
	~TOutput();
	bool appendLine(const char * const pszLine);
	int appendMultiLine(const char * const pszLines);
	int getCount(void) { 
		return iLineCount; 
	}
	char *getLine(int iLine) {
		if (iLine < iLineCount)
		{
			return pszLines[iLine];
		}
		return NULL;
	};
	char *getFirstLine(void)
	{
		if (iLineCount > 0)
		{
			iCurrentLine = 1;
			return pszLines[0];
		}
		return NULL;
	}
	char *getNextLine(void)
	{
		if (iCurrentLine < iLineCount)
			return pszLines[iCurrentLine++];
		return NULL;
	}
	void Clear(void) {
		for (int i = 0; i < iLineCount; ++i)
		{
			free(pszLines[i]);
			pszLines[i] = NULL;
		}
	}
};