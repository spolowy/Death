#ifndef POSITION_INDEPENDENT_H
# define POSITION_INDEPENDENT_H

/*
** If you want to understand smth and you're not a Kernel dev
** Take some black magic classes at :
** https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
** http://jhnet.co.uk/articles/cpp_magic
*/

# define __FIRST(a, ...) a
# define __SECOND(a, b, ...) b

# define __EMPTY()

# define __EVAL(...) __EVAL512(__VA_ARGS__)
# define __EVAL512(...) __EVAL256(__EVAL256(__VA_ARGS__))
# define __EVAL256(...) __EVAL128(__EVAL128(__VA_ARGS__))
# define __EVAL128(...) __EVAL64(__EVAL64(__VA_ARGS__))
# define __EVAL64(...) __EVAL32(__EVAL32(__VA_ARGS__))
# define __EVAL32(...) __EVAL16(__EVAL16(__VA_ARGS__))
# define __EVAL16(...) __EVAL8(__EVAL8(__VA_ARGS__))
# define __EVAL8(...) __EVAL4(__EVAL4(__VA_ARGS__))
# define __EVAL4(...) __EVAL2(__EVAL2(__VA_ARGS__))
# define __EVAL2(...) __EVAL1(__EVAL1(__VA_ARGS__))
# define __EVAL1(...) __VA_ARGS__

# define __DEFER1(m) m __EMPTY()
# define __DEFER2(m) m __EMPTY __EMPTY()()
# define __DEFER3(m) m __EMPTY __EMPTY __EMPTY()()()
# define __DEFER4(m) m __EMPTY __EMPTY __EMPTY __EMPTY()()()()

# define __IS_PROBE(...) __SECOND(__VA_ARGS__, 0)
# define __PROBE() ~, 1

# define __CAT(a,b) a ## b

# define __NOT(x) __IS_PROBE(__CAT(___NOT_, x))
# define ___NOT_0 __PROBE()

# define __BOOL(x) __NOT(__NOT(x))

# define __IF_ELSE(condition) ___IF_ELSE(__BOOL(condition))
# define ___IF_ELSE(condition) __CAT(___IF_, condition)

# define ___IF_1(...) __VA_ARGS__ ___IF_1_ELSE
# define ___IF_0(...)             ___IF_0_ELSE

# define ___IF_1_ELSE(...)
# define ___IF_0_ELSE(...) __VA_ARGS__

# define __HAS_ARGS(...) __BOOL(__FIRST(___END_OF_ARGUMENTS_ __VA_ARGS__)())
# define ___END_OF_ARGUMENTS_() 0

# define __MAP(m, name, off, value, ...)            \
  m(value, name, off)                               \
  __IF_ELSE(__HAS_ARGS(__VA_ARGS__))(               \
    __DEFER2(_MAP)()(m, name, off+1, __VA_ARGS__)   \
  )()
# define _MAP() __MAP

# define __INITVAR(x, name, off) name[off] = x;
# define __COUNT_LEN(x, name, off) +1

# define PD_ARRAY(type, name, ...)                                       \
	type name [0 __EVAL(__MAP(__COUNT_LEN, name, 0, __VA_ARGS__))]; \
	__EVAL(__MAP(__INITVAR, name, 0, __VA_ARGS__))

/*
** Usage example:
**
** int main()
** {
** 	PD_ARRAY(char, huge_array,
** 		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
** 		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
** 		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
** 		0x00);
**
** 	puts(huge_array);
** }
*/

#endif
