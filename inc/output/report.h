void reportUnclosedString(int line, int column);
void reportUnexpectedChar(int line, int column, char ch);
void reportUnexpectedToken(int line, int column, const char *unexpected, const char *expected);
void reportVariableUndefined(int line, int column, const char *name);
void reportVariabledefined(int line, int column, const char *name);
void reportBinaryOperatorError(int line, int column, const char *left, const char *op, const char *right);
void reportUnaryOperatorError(int line, int column, const char *op, char *operand);
void reportAccessError(int line, int column, const char *name);
void reportCallError(int line, int column, const char *name);