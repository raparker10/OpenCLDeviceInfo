__kernel void hello_kernel(__global char16 *msg) { 
*msg = (char16)('H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!','!','!','!','\0');
}
__kernel void goodbye_kernel(__global char16 *msg) { 
*msg = (char16)('G', 'o', 'o', 'd', 'b','y','e', ' ', 'W', 'o', 'r', 'l', 'd', '!','!','\0');
}
