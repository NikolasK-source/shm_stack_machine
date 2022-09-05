# List of instructions

## Interact with memory

### PUSH / L
```
PUSH <target>
```
Push target to stack.
Target can be variable or unsigned integer constant.

### POP / S
```
POP <target>
```
Pop value from stack and store it in variable.
Use ```NULL``` as target to discard the value.

### DUP
Duplicate top of stack.

## Arithmetic Instructions
Arithmetic instructions operate on the stack.
They consume two values and push the result to the stack.

### ADD
Integer addition

### SUB
Integer subtraction

### MUL
Unsigned integer multiplication

### MULS
Signed integer multiplication

### DIV
Unsigned integer division

### DIVS
Signed integer division

### MOD
Unsigned integer modulo

### MODS
Signed integer modulo

### POW
Unsigned integer exponentiation

### POWS
Signed integer exponentiation

### ADDF
32 bit floating point addition

### SUBF
32 bit floating point subtraction

### MULF
32 bit floating point multiplication

### DIVF
32 bit floating point division

### POWF
32 bit floating point exponentiation

### ADDD
64 bit floating point addition

### SUBD
64 bit floating point subtraction

### MULD
64 bit floating point multiplication

### DIVD
64 bit floating point division

### POWD
64 bit floating point exponentiation

## Logic instructions
Logic instructions operate on the stack.
Logic instructions treat all values that are not 0 as true.
The result of a logic instruction is always 1 or 0.

### NOT
Inverts the top of the stack.

### AND
Consumes two values, applies ```logical and``` and pushes the result to the stack.

### OR
Consumes two values, applies ```logical or``` and pushes the result to the stack.

### XOR
Consumes two values, applies ```logical xor``` and pushes the result to the stack.

## Bitwise instructions

Bitwise instructions are applied to the whole word width.

### INV
Inverts every bit of the top of the stack.

### BAND
Consumes two values, applies ```bitwise and``` and pushes the result to the stack.

### BOR
Consumes two values, applies ```bitwise or``` and pushes the result to the stack.

### BXOR
Consumes two values, applies ```bitwise xor``` and pushes the result to the stack.

## Data conversion instructions

### ITOF
Convert top of stack: Unsigned integer to 32 bit floating point.

### ITOD
Convert top of stack: Unsigned integer to 64 bit floating point.

### FTOI
Convert top of stack: 32 bit floating point to unsigned integer.

### DTOI
Convert top of stack: 64 bit floating point to unsigned integer.

### FTOD
Convert top of stack: 32 bit floating point to 64 bit floating point.

### DTOF
Convert top of stack: 64 bit floating point to 32 bit floating point.

## Relational instructions
Relational instructions operate on the stack.
They consume two values and push the result to the stack.
The result is 1 if the condition is true and 0 if the condition is false

> **NOTE** to compare 32 floating point values you must convert them the 64 bit floating point values first.

### EQ
Equal (All data types)

### NE
Net Equal (All data types)

### LT
Less than (unsigned integer)

### GT
Greater than (unsigned integer)

### LE
Less than or equal (unsigned integer)

### GE
Greater than or equal (unsigned integer)

### LTS
Less than (signed integer)

### GTS
Greater than (signed integer)

### LES
Less than or equal (signed integer)

### GES
Greater than or equal (signed integer)

### LTD
Less than (64 bit floating point)

### GTD
Greater than (signed integer)

### LED
Less than or equal (signed integer)

### GED
Greater than or equal (signed integer)


## Floating point math instructions

All floating point math instructions operate on the stack.
Unless otherwise specified, they operate on the top of the stack.

> **NOTE** All Floating point math instructions operate with 64 bit floating point values

### ABS
Get the absolute value.

### SQRT
Get the square root.

### CBRT
Get the cubic root.

### LN
Get the natural (base e) logarithm

### LOG
Get the common (base 10) logarithm

### LG
Get the binary (base 2) logarithm

### SIN
Get the sine

### COS
Get the cosine

### ASIN
Get the principal value of the arc sine

### ACOS
Get the principal value of the arc cosine

### ATAN
Get the principal value of the arc tangent

### ATANXY / ATAN2
Get the arc tangent by x and y value.  
This instruction consumes two values from the stack and pushes its result to the stack.  
X is left operand and Y is right operand (top of stack).

## Jump instructions

### J
```
J <label>
```
Unconditional jump to ```label```.

### JZ
```
JZ <label>
```
Jump  to ```label``` if top of stack is zero.  
Pops the top of the stack!

### JNZ
```
JNZ <label>
```
Jump  to ```label``` if top of stack is **not** zero.  
Pops the top of the stack!
