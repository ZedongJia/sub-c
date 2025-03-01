#include "defs.h"
#include <stdlib.h>
#include <string.h>

const symbol_t *symbol_table_look_up(symbol_table_t *self, const char *name)
{
    symbol_t *symbol;
    /***
     * Try to find this symbol with this name
     */
    list_iterator_t *iter = self->symbols->get_iter(self->symbols);
    while (!iter->is_end(iter))
    {
        symbol = (symbol_t *)iter->get(iter);
        if (strcmp(name, symbol->name) == 0)
            return symbol;
        iter->next(iter);
    }
    iter->del(iter);
    /***
     * Didn't find this
     */
    return NULL;
}

const symbol_t *__symbol_table_declare(symbol_table_t *self, const ctype_t *ctype, const char *name,
                                       const list_t *params_ctype)
{
    // declare
    symbol_t *symbol = (symbol_t *)malloc(sizeof(symbol_t));
    symbol->ctype = ctype;
    symbol->name = name;
    symbol->params_ctype = params_ctype;
    self->offset += ctype_offset(ctype);
    symbol->offset = self->offset;
    // insert
    self->symbols->append(self->symbols, symbol, free);
    return symbol;
}

const symbol_t *symbol_table_try_declare(symbol_table_t *self, const ctype_t *ctype, const char *name,
                                         const list_t *params_ctype)
{
    const symbol_t *symbol = symbol_table_look_up(self, name);
    if (symbol == NULL)
    {
        symbol = __symbol_table_declare(self, ctype, name, params_ctype);
    }
    return symbol;
}

int symbol_table_declare(symbol_table_t *self, const ctype_t *ctype, const char *name, const list_t *params_ctype)
{
    int success = 0;
    if (symbol_table_look_up(self, name) == NULL)
    {
        __symbol_table_declare(self, ctype, name, params_ctype);
        success = 1;
    }
    return success;
}

void symbol_table_del(symbol_table_t *self)
{
    self->symbols->del(self->symbols);
    free(self);
}

symbol_table_t *new_symbol_table(symbol_table_t *prt)
{
    symbol_table_t *symbol_list = (symbol_table_t *)malloc(sizeof(symbol_table_t));
    symbol_list->symbols = new_list();
    symbol_list->offset = 0;
    symbol_list->call_offset = 0;
    symbol_list->look_up = &symbol_table_look_up;
    symbol_list->try_declare = &symbol_table_try_declare;
    symbol_list->declare = &symbol_table_declare;
    symbol_list->del = &symbol_table_del;
    symbol_list->prt = prt;
    return symbol_list;
}
