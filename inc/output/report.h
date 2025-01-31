void reportUnclosedString(int line, int column);
void reportUnexpectedChar(int line, int column, char ch);
void reportUnexpectedToken(int line, int column, const char *unexpected, const char *expected);
void reportVariableUndefined(int line, int column, const char* name);
void reportVariabledefined(int line, int column, const char* name);
