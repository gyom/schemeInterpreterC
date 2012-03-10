
MemorySpace.o : MemorySpace.c
	gcc -c -Wall MemorySpace.c
	
test_MemorySpace : MemorySpace.c
	gcc -D TEST_MEMORYSPACE_ALONE -Wall MemorySpace.c -o test_MemorySpace
	
SchemeObject.o : SchemeObject.c
	gcc -c -Wall SchemeObject.c


test_SchemeObject : SchemeObject.c SchemeObject.h MemorySpace.c MemorySpace.h GarbageCollection.c SchemeObject_tests.c SchemeObject_tests.h atomic_functions.c sn1t_parsing.c
	gcc -g -D TEST_SCHEMEOBJECT_ALONE -D DEBUG_MEMORYSPACE -Wall MemorySpace.c GarbageCollection.c SchemeObject_tests.c atomic_functions.c sn1t_parsing.c SchemeObject.c -o test_SchemeObject
	
test_SchemeObject_debug_execute : SchemeObject.c SchemeObject.h MemorySpace.c MemorySpace.h GarbageCollection.c SchemeObject_tests.c SchemeObject_tests.h atomic_functions.c sn1t_parsing.c
	gcc -g -D TEST_SCHEMEOBJECT_ALONE -D DEBUG_EXECUTE -D DEBUG_MEMORYSPACE -Wall MemorySpace.c GarbageCollection.c SchemeObject_tests.c atomic_functions.c sn1t_parsing.c SchemeObject.c -o test_SchemeObject
	
test_SchemeObject_debug_gc : SchemeObject.c SchemeObject.h MemorySpace.c MemorySpace.h GarbageCollection.c SchemeObject_tests.c SchemeObject_tests.h atomic_functions.c sn1t_parsing.c
	gcc -g -D TEST_SCHEMEOBJECT_ALONE -D DEBUG_GC -D DEBUG_MEMORYSPACE -D DEBUG_SN1T -Wall MemorySpace.c GarbageCollection.c SchemeObject_tests.c atomic_functions.c sn1t_parsing.c SchemeObject.c -o test_SchemeObject
	
test_SchemeObject_debug_callcc : SchemeObject.c SchemeObject.h MemorySpace.c MemorySpace.h GarbageCollection.c SchemeObject_tests.c SchemeObject_tests.h atomic_functions.c sn1t_parsing.c
	gcc -g -D TEST_SCHEMEOBJECT_ALONE -D DEBUG_MEMORYSPACE -D DEBUG_CALLCC -Wall MemorySpace.c GarbageCollection.c SchemeObject_tests.c atomic_functions.c sn1t_parsing.c SchemeObject.c -o test_SchemeObject
	