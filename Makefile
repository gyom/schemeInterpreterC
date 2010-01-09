
MemorySpace.o : MemorySpace.c
	gcc -c -Wall MemorySpace.c
	
test_MemorySpace : MemorySpace.c
	gcc -D TEST_MEMORYSPACE_ALONE -Wall MemorySpace.c -o test_MemorySpace
	
SchemeObject.o : SchemeObject.c
	gcc -c -Wall SchemeObject.c

test_SchemeObject : SchemeObject.c MemorySpace.c
	gcc -D TEST_SCHEMEOBJECT_ALONE -Wall MemorySpace.c SchemeObject.c -o test_SchemeObject
	
	
	gcc -g -D TEST_SCHEMEOBJECT_ALONE -D DEBUG_EXECUTE -D DEBUG_MEMORYSPACE -Wall MemorySpace.c SchemeObject_tests.c atomic_functions.c SchemeObject.c -o test_SchemeObject