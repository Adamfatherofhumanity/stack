#ifdef DEBUG
    #define DBG(...) __VA_ARGS__
    #define STACK_OK(call_func, call_file, call_line, state, func, file, line, fp_dump, rtrn)\
        if (StackVerify(ptr_stack, (call_func), (call_file), (call_line), (state), (func), (file), (line), (fp_dump)) != NO_ERRORS) \
        { \
            printf("[ERROR] " state " %s() at %s:%d called by %s() at %s:%d\n", (func), (file), (line), (call_func), (call_file), (call_line)); \
            return rtrn; \
        }
    #define RECALC_HASH \
        do { \
            ptr_stack->stack_hash = CalcStackHash(ptr_stack); \
            ptr_stack->data_hash = CalcDataHash(ptr_stack); \
        } while(0);
    #define PTR_OK(ptr, name, rtrn) \
        if ((ptr) == NULL) \
        { \
            fprintf(stderr, "[ERROR] NULL pointer to " name "\n"); \
            return rtrn; \
        }
#else
    #define DBG(...)
    #define STACK_OK(call_func, call_file, call_line, state, func, file, line, fp_dump, rtrn)
    #define RECALC_HASH
    #define PTR_OK(ptr, name, rtrn) 
    #define NDEBUG
#endif

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <malloc.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

typedef unsigned char byte;
typedef int err_t;

enum Errors
{
    NO_ERRORS = 0,
    EMPTY_STACK  = 1 << 0,
    NULL_PTR = 1 << 1,
    ALLOCATION_ERROR = 1 << 2,
    NULL_DATA = 1 << 3,
    SIZE_ERROR = 1 << 4,
    CAPACITY_ERROR = 1 << 5,
    CANARY_ERROR = 1 << 6,
    HASH_ERROR = 1 << 7
};

enum TypesOfErrors
{
    NON_CRITICAL = 0,
    CRITICAL = 1
};

typedef double stack_elem_t;
#define SETS "%lf" //stack_elem_t_specificator

#ifdef DEBUG
    typedef double canary_t;
    typedef uint64_t hash_t;
    #define CTS "%lf" //canary_t_specificator
    #define HTS "%" PRIu64 //hash_t_specificator
    const canary_t CANARY_1 = NAN;
    const stack_elem_t CANARY_2 = -INFINITY;
    const stack_elem_t CANARY_3 = +INFINITY;
    const canary_t CANARY_4 = -0.0;
#endif

const stack_elem_t POISON = 0xDEAD1971DE;
const float DEC_FOR_COMP_COEF = 0.4;
struct stack_structure
{
    #ifdef DEBUG
        canary_t start_canary = CANARY_1;
        hash_t stack_hash;
        hash_t data_hash;
        const char *name;
        const char *func;
        const char *file;
        unsigned int line;
    #endif

    stack_elem_t *data;
    size_t size;
    size_t capacity;

    DBG(canary_t end_canary = CANARY_4;)
};

void StackCtor(stack_structure *ptr_stack, size_t capacity, err_t *ptr_err
               DBG(, const char *name, const char *func, const char *file, const unsigned int line), FILE *fp_dump);
void StackDtor(stack_structure *ptr_stack
               DBG(, const char *func, const char *file, const unsigned int line), FILE *fp_dump);

void StackPush(stack_structure *ptr_stack, stack_elem_t elem, err_t *ptr_err
               DBG(, const char *func, const char *file, const unsigned int line), FILE *fp_dump);
stack_elem_t StackPop(stack_structure *ptr_stack, err_t *ptr_err
                      DBG(, const char *func, const char *file, const unsigned int line), FILE *fp_dump);
size_t StackSize(stack_structure *ptr_stack);

err_t TypeOfError(err_t err, bool need_to_print);
void PrintFileError(const char *type_of_error, const char *name_of_file);

#ifdef DEBUG
    err_t StackVerify(stack_structure *ptr_stack, const char *call_func, const char *call_file, int call_line,
                      const char *state, const char *func, const char *file, int line, FILE *fp_dump);
    void StackDump(stack_structure *ptr_stack, err_t err, const char *call_func, const char *call_file, int call_line,
                   const char *state, const char *func, const char *file, int line, FILE *fp_dump);

    hash_t CalcHash(const void *ptr, size_t size);
    hash_t CalcStackHash(const stack_structure *ptr_stack);
    hash_t CalcDataHash(const stack_structure *ptr_stack);
#endif

int main(void)
{
    const char *DUMP_NAME = "dump.txt";
    FILE *fp_dump = fopen(DUMP_NAME, "w");
    if (fp_dump == NULL) PrintFileError("opening", DUMP_NAME);
    setvbuf(fp_dump, NULL, _IONBF, 0);
    stack_structure stk1 = {};
    err_t err = 0;
    StackCtor(&stk1, 5, &err DBG(, "stk1", __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPush(&stk1, 10, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    stk1.capacity = 0;

    StackPush(&stk1, 10, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackCtor(&stk1, 5, &err DBG(, "stk1", __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPush(&stk1, 20, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackCtor(&stk1, 5, &err DBG(, "stk1", __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPush(&stk1, 20, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackCtor(&stk1, 5, &err DBG(, "stk1", __FUNCTION__, __FILE__, __LINE__), fp_dump);
    DBG(StackVerify(&stk1, __FUNCTION__, __FILE__, __LINE__, "check", __FUNCTION__, __FILE__, __LINE__, fp_dump);)
    DBG(StackVerify(&stk1, __FUNCTION__, __FILE__, __LINE__, "check", __FUNCTION__, __FILE__, __LINE__, fp_dump);)
    StackPush(&stk1, 222, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPush(&stk1, 222, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPush(&stk1, 222, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    DBG(StackVerify(&stk1, __FUNCTION__, __FILE__, __LINE__, "check", __FUNCTION__, __FILE__, __LINE__, fp_dump);)
    StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    DBG(StackVerify(&stk1, __FUNCTION__, __FILE__, __LINE__, "check", __FUNCTION__, __FILE__, __LINE__, fp_dump);)
    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    DBG(StackVerify(&stk1, __FUNCTION__, __FILE__, __LINE__, "check", __FUNCTION__, __FILE__, __LINE__, fp_dump);)
    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    StackPush(&stk1, 222, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    printf(SETS "\n", StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump));
    printf(SETS "\n", StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump));
    StackPop(&stk1, &err DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);

    // TypeOfError(NO_ERRORS, true);
    // TypeOfError(EMPTY_STACK, true);
    // TypeOfError(NULL_PTR, true);
    // TypeOfError(ALLOCATION_ERROR, true);
    // TypeOfError(NULL_DATA, true);
    // TypeOfError(SIZE_ERROR, true);
    // TypeOfError(CAPACITY_ERROR, true);
    // TypeOfError(CANARY_ERROR, true);
    // TypeOfError(HASH_ERROR, true);

    StackDtor(&stk1 DBG(, __FUNCTION__, __FILE__, __LINE__), fp_dump);
    if (fclose(fp_dump) < 0) PrintFileError("closing", DUMP_NAME);
    return 0;
}

void StackCtor(stack_structure *ptr_stack, size_t capacity, err_t *ptr_err
               DBG(, const char *name, const char *func, const char *file, const unsigned int line), FILE *fp_dump)
{
    PTR_OK(ptr_stack, "stack", );
    PTR_OK(ptr_err, "err", );
    PTR_OK(name, "name", );
    PTR_OK(func, "func", );
    PTR_OK(file, "file", );
    PTR_OK(fp_dump, "dump", );

    #ifdef DEBUG
        ptr_stack->name = name;
        ptr_stack->func = func;
        ptr_stack->file = file;
        ptr_stack->line = line;

        capacity += 2;
    #endif

    ptr_stack->data = (stack_elem_t *) calloc(capacity, sizeof(stack_elem_t));
    stack_elem_t *data = ptr_stack->data;

    if (data == NULL)
    {
        *ptr_err = ALLOCATION_ERROR;
        return;
    }

    ptr_stack->size = 0;
    ptr_stack->capacity = capacity;
    *ptr_err = NO_ERRORS;

    #ifdef DEBUG
        ptr_stack->size += 1;
        data[0] = CANARY_2;
        for (unsigned int i = 1; i < capacity - 1; i++) data[i] = POISON;
        data[capacity - 1] = CANARY_3;
    #endif

    RECALC_HASH
    STACK_OK(func, file, line, "after",  __FUNCTION__, __FILE__, __LINE__, fp_dump, )
}

void StackDtor(stack_structure *ptr_stack
               DBG(, const char *func, const char *file, const unsigned int line), FILE *fp_dump)
{
    PTR_OK(ptr_stack, "stack", );
    PTR_OK(fp_dump, "dump", );
    PTR_OK(func, "func", );
    PTR_OK(file, "file", );
    STACK_OK(func, file, line, "before",  __FUNCTION__, __FILE__, __LINE__, fp_dump, )

    free(ptr_stack->data);
    ptr_stack->data = NULL;
    ptr_stack->capacity = 0;
    ptr_stack->size = 0;

    #ifdef DEBUG
        ptr_stack->stack_hash = 0;
        ptr_stack->data_hash = 0;
        ptr_stack->name = NULL;
        ptr_stack->func = NULL;
        ptr_stack->file = NULL;
        ptr_stack->line = 0;
    #endif
}

void StackPush(stack_structure *ptr_stack, stack_elem_t elem, err_t *ptr_err
               DBG(, const char *func, const char *file, const unsigned int line), FILE *fp_dump)
{
    PTR_OK(ptr_stack, "stack", );
    PTR_OK(ptr_err, "err", );
    PTR_OK(fp_dump, "dump", );
    PTR_OK(func, "func", );
    PTR_OK(file, "file", );
    STACK_OK(func, file, line, "before",  __FUNCTION__, __FILE__, __LINE__, fp_dump, )

    size_t size = ptr_stack->size;
    size_t capacity = ptr_stack->capacity;
    stack_elem_t *data = ptr_stack->data;

    if (size == capacity DBG(- 1))
    {
        data = (stack_elem_t *) realloc(data, capacity * 2 * sizeof(stack_elem_t));
        if (data == NULL)
        {
            *ptr_err = ALLOCATION_ERROR;
            return;
        }
        
        #ifdef DEBUG
            for (unsigned int i = capacity - 1; i < capacity * 2 - 1; i++) data[i] = POISON;
            data[capacity * 2 - 1] = CANARY_3;
        #endif

        ptr_stack->data = data;
        ptr_stack->capacity *= 2;
    }

    data[size] = elem;
    ptr_stack->size++;
    *ptr_err = NO_ERRORS;

    RECALC_HASH
    STACK_OK(func, file, line, "after",  __FUNCTION__, __FILE__, __LINE__, fp_dump, )
}

stack_elem_t StackPop(stack_structure *ptr_stack, err_t *ptr_err
                      DBG(, const char *func, const char *file, const unsigned int line), FILE *fp_dump)
{
    PTR_OK(ptr_stack, "stack", POISON);
    PTR_OK(ptr_err, "err", POISON);
    PTR_OK(fp_dump, "dump", POISON);
    PTR_OK(func, "func", POISON);
    PTR_OK(file, "file", POISON);
    STACK_OK(func, file, line, "before",  __FUNCTION__, __FILE__, __LINE__, fp_dump, POISON)

    size_t size = ptr_stack->size;
    size_t capacity = ptr_stack->capacity;
    stack_elem_t *data = ptr_stack->data;

    if (size DBG(- 1) == 0)
    {
        *ptr_err = EMPTY_STACK;
        return POISON;
    }

    *ptr_err = NO_ERRORS;
    if (size DBG(+ 1) < capacity * DEC_FOR_COMP_COEF)
    {
        data = (stack_elem_t *) realloc(data, capacity / 2 * sizeof(stack_elem_t));
        if (data == NULL) *ptr_err = ALLOCATION_ERROR;
        else
        {
            ptr_stack->data = data;
            ptr_stack->capacity /= 2;
            capacity = ptr_stack->capacity;

            DBG(data[capacity - 1] = CANARY_3;)
        }
    }

    data = ptr_stack->data;
    ptr_stack->size--;
    stack_elem_t returned_value = data[size - 1];

    DBG(data[size - 1] = POISON;)

    RECALC_HASH
    STACK_OK(func, file, line, "after",  __FUNCTION__, __FILE__, __LINE__, fp_dump, POISON)
    return returned_value;
}

size_t StackSize(stack_structure *ptr_stack)
{
    PTR_OK(ptr_stack, "stack", NULL_PTR);

    return ptr_stack->size DBG(- 1);
}

err_t TypeOfError(err_t err, bool need_to_print)
{
    if (need_to_print)
    {
        const char *NC = "[NON_CRITICAL]";
        const char *C = "[CRITICAL]";
        const char *broken = ", stack is broken.\n";

        switch (err)
        {
            case NO_ERRORS:
            {
                printf("%s No error.\n", NC);
                break;
            }

            case EMPTY_STACK:
            {
                printf("%s Stack is empty, pop returned a poison.\n", NC);
                break;
            }

            case NULL_PTR:
            {
                printf("%s Null pointer in function args.\n", NC);
                break;
            }

            case ALLOCATION_ERROR:
            {
                printf("%s Allocation error, capacity hasn't changed (elem hasn't pushed).\n", NC);
                break;
            }

            case NULL_DATA:
            {
                printf("%s Null pointer to data in stack%s", C, broken);
                break;
            }

            case SIZE_ERROR:
            {
                printf("%s Incorrect size (size bigger then real capacity)%s", C, broken);
                break;
            }

            case CAPACITY_ERROR:
            {
                printf("%s Incorrect capacity(capacity bigger then real capacity or equal to 0)%s", C, broken);
                break;
            }

            case CANARY_ERROR:
            {
                printf("%s Canaries injured%s", C, broken);
                break;
            }

            case HASH_ERROR:
            {
                printf("%s Hash doesn't match%s", C, broken);
                break;
            }

            default:
            {
                printf("Undefined error");
                break;
            }
        }
    }
    if (err >= NULL_DATA) return CRITICAL;
    else return NON_CRITICAL;
}

void PrintFileError(const char *type_of_error, const char *name_of_file)
{
    PTR_OK(name_of_file, "name", );
    PTR_OK(type_of_error, "type of error", );

    printf("Error %s file \"%s\".\n", type_of_error, name_of_file);
}

#ifdef DEBUG
err_t StackVerify(stack_structure *ptr_stack, const char *call_func, const char *call_file, int call_line,
                  const char *state, const char *func, const char *file, int line, FILE *fp_dump)
{
    PTR_OK(call_func, "call_func", NULL_PTR);
    PTR_OK(call_line, "call_line", NULL_PTR);
    PTR_OK(ptr_stack, "stack", NULL_PTR);
    PTR_OK(state, "state", NULL_PTR);
    PTR_OK(func, "func", NULL_PTR);
    PTR_OK(file, "file", NULL_PTR);
    PTR_OK(fp_dump, "dump", NULL_PTR);

    err_t returned_value = NO_ERRORS;
    size_t capacity = ptr_stack->capacity;
    stack_elem_t *data = ptr_stack->data;

    if (data == NULL) returned_value |= NULL_DATA;

    if (ptr_stack->size > capacity || data != NULL && ptr_stack->size > _msize(data) / sizeof(stack_elem_t)) returned_value |= SIZE_ERROR;

    if (capacity == 0 || data != NULL && capacity > _msize(data) / sizeof(stack_elem_t)) returned_value |= CAPACITY_ERROR;

    if (data != NULL && (!isnan(ptr_stack->start_canary) || ptr_stack->end_canary != 0 || signbit(ptr_stack->end_canary) == 0
        || !isinf(data[0]) || signbit(data[0]) == 0 || capacity != 0 && !isinf(data[capacity - 1])
        || capacity != 0 && signbit(data[capacity - 1]) != 0)) returned_value |= CANARY_ERROR;

    if (ptr_stack->stack_hash != CalcStackHash(ptr_stack)
        || ptr_stack->data_hash != CalcDataHash(ptr_stack)) returned_value |= HASH_ERROR;

    StackDump(ptr_stack, returned_value, call_func, call_file, call_line, state, func, file, line, fp_dump);
    return returned_value;
}

void StackDump(stack_structure *ptr_stack, err_t err, const char *call_func, const char *call_file, int call_line,
               const char *state, const char *func, const char *file, int line, FILE *fp_dump)
{
    PTR_OK(call_func, "call_func", );
    PTR_OK(call_line, "call_line", );
    PTR_OK(ptr_stack, "stack", );
    PTR_OK(state, "state", );
    PTR_OK(func, "func", );
    PTR_OK(file, "file", );
    PTR_OK(fp_dump, "dump", );

    size_t size = ptr_stack->size;
    size_t capacity = ptr_stack->capacity;
    stack_elem_t *data = ptr_stack->data;

    fprintf(fp_dump,
            "\n" __TIME__ " " __DATE__ " %s %s() at %s:%d called by %s() at %s:%d\n"
            "<%d> in stack_structure \"%s\"[%p] created by %s() at %s:%d\n"
            "{\n"
            "    stack_hash   = " HTS "\n"
            "    data_hash    = " HTS "\n"
            "    start_canary = " CTS "\n"
            "    capacity     = %zu\n"
            "    size         = %zu\n"
            "    data[%p]\n",
            state, func, file, line, call_func, call_file, call_line, err, ptr_stack->name, ptr_stack, ptr_stack->func, ptr_stack->file, ptr_stack->line,
            ptr_stack->stack_hash, ptr_stack->data_hash, ptr_stack->start_canary, capacity, size, data);

    if ((err & NULL_DATA) == NULL_DATA)
    {
        fprintf(fp_dump, "}");
        return;
    }

    size_t i_max = 0, i_real = 0;

    if ((err & SIZE_ERROR) == SIZE_ERROR) i_real = capacity;
    else i_real = size;

    if ((err & CAPACITY_ERROR) == CAPACITY_ERROR) i_max = _msize(data) / sizeof(stack_elem_t);
    else i_max = capacity;

    fprintf(fp_dump, "    {\n");
    fprintf(fp_dump, "         [0] = " CTS " (canary)\n", data[0]);
    for (size_t i = 1; i < i_real; i++)
    {
        fprintf(fp_dump, "        *[%zu] = " SETS "\n", i, data[i]);
    }
    for (size_t i = i_real; i < i_max - 1; i++)
    {
        fprintf(fp_dump, "         [%zu] = " SETS " (poison)\n", i, data[i]);
    }
    fprintf(fp_dump, "        *[%zu] = " CTS " (canary)\n", i_max - 1, data[i_max - 1]);
    fprintf(fp_dump,
            "    }\n"
            "    end_canary   = " CTS "\n"
            "}\n", ptr_stack->end_canary);
}

hash_t CalcHash(const void *ptr, size_t size)
{
    PTR_OK(ptr, "...", NULL_PTR);

    const byte *bytes = (const byte *) ptr;
    hash_t hash = 5381;

    for (size_t i = 0; i < size; ++i)
    {
        hash = ((hash << 5) + hash) + bytes[i];
    }

    return hash;
}

hash_t CalcStackHash(const stack_structure *ptr_stack)
{
    PTR_OK(ptr_stack, "stack", NULL_PTR);

    stack_structure temp = *ptr_stack;
    temp.stack_hash = 0;
    temp.data_hash = 0;

    return CalcHash(&temp, sizeof(stack_structure));
}

hash_t CalcDataHash(const stack_structure *ptr_stack)
{
    PTR_OK(ptr_stack, "stack", NULL_PTR);
    PTR_OK(ptr_stack->data, "data", NULL_PTR);

    return CalcHash(ptr_stack->data, ptr_stack->capacity * sizeof(stack_elem_t));
}
#endif
