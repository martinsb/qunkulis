#include "unqlite-db/unqlite.h"

#include <stdio.h>

int main(int argc, char** argv)
{
	unqlite* db;
	unqlite_vm* vm;
	unqlite_value *a, *b, *c;
	int rc;

	rc = unqlite_open(&db, NULL, UNQLITE_OPEN_CREATE);
	if (rc != UNQLITE_OK) {
		printf("Could not create database\n");
		return 1;
	}
	rc = unqlite_compile(db, "$c = $b + $a;", -1, &vm);
	if (rc != UNQLITE_OK) {
		printf("Could not compile the script\n");
		return 1;
	}

	a = unqlite_vm_new_scalar(vm);
	unqlite_value_int(a, 2);
	unqlite_vm_config(vm, UNQLITE_VM_CONFIG_CREATE_VAR, "a", a);
	unqlite_vm_release_value(vm, a);	

	b = unqlite_vm_new_scalar(vm);
	unqlite_value_int(b, 3);	
	unqlite_vm_config(vm, UNQLITE_VM_CONFIG_CREATE_VAR, "b", b);
	unqlite_vm_release_value(vm, b);

	rc = unqlite_vm_exec(vm);

	c = unqlite_vm_extract_variable(vm, "c");


	printf("Value of c is: %d\n", unqlite_value_to_int(c));

	return 0;
}