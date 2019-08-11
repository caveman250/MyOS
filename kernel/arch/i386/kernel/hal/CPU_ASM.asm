global cpu_get_vendor:function
cpu_get_vendor:
mov		eax, 0
cpuid
mov		[eax], ebx
mov		[eax+4], edx
mov		[eax+8], ecx

ret