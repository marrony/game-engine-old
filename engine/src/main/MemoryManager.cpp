/*
 * MemoryManager.cpp
 *
 *  Created on: 11/03/2012
 *      Author: Marrony
 */

#include "MemoryManagerInternal.h"
#include "ResourcePlugin.h"

#include <new>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef NDEBUG

enum PrintType {
	ALLOC,
	DEALLOC,
	DUMP
};

struct MemoryEntry {
	MemoryEntry* next;
	MemoryEntry* previous;

	char moduleName[FILENAME_MAX + 1];
	char fileName[FILENAME_MAX + 1];
	int line;
	unsigned int nbytes;
	bool isArray;

	void printEntry(PrintType printType) {
		if(printType == PrintType::DUMP) {
			fprintf(stdout, "%d bytes in %s:%d array: %d\n", nbytes, fileName, line, isArray);
			fflush(stdout);
		}
	}
};

struct MemoryList {
	MemoryEntry* head;
	int count;

	void insert(MemoryEntry* entry) {
		entry->printEntry(PrintType::ALLOC);

		if(!head)
			head = entry;
		else {
			entry->next = head;
			head->previous = entry;

			head = entry;
		}

		count++;
	}

	void remove(MemoryEntry* entry) {
		entry->printEntry(PrintType::DEALLOC);

		if(entry == head) {
			head = head->next;

			if(head)
				head->previous = 0;
		} else {
			entry->previous->next = entry->next;

			if(entry->next)
				entry->next->previous = entry->previous;
		}

		entry->next = 0;
		entry->previous = 0;

		count--;
	}

	bool exists(MemoryEntry* entry) {
		MemoryEntry* h = head;

		while(h != 0) {
			if(h == entry)
				return true;

			h = h->next;
		}
		return false;
	}
};

static MemoryList list = {0};

static void printMemoryBlock(char* ptr, unsigned int nbytes, int maxRows) {
	const int CHARS_PER_LINE = 16;

	int i = 0;
	while(i < nbytes) {
		int x = nbytes - i;
		int y = x > CHARS_PER_LINE ? CHARS_PER_LINE : x;

		fprintf(stdout, "\t");

		for(int j = 0; j < y; j++) {
			fprintf(stdout, "%02x ", ptr[i + j] & 0xff);
		}

		for(int j = y; j < CHARS_PER_LINE; j++)
			fprintf(stdout, "   ");

		fprintf(stdout, "\t");

		for(int j = 0; j < y; j++) {
			int ch = ptr[i + j] & 0xff;
			fprintf(stdout, "%c ", isprint(ch) ? ch : '.');
		}

		for(int j = y; j < CHARS_PER_LINE; j++)
			fprintf(stdout, "  ");

		fprintf(stdout, "\n");

		i += y;
	}
}

static void dumpEntries() {
	fprintf(stdout, "dumping\n");
	fprintf(stdout, "blockCount %d\n", list.count);

	MemoryEntry* entry = list.head;

	while(entry != 0) {
		entry->printEntry(PrintType::DUMP);
		printMemoryBlock((char*)entry + sizeof(MemoryEntry), entry->nbytes, 4);

		entry = entry->next;
	}

	fflush(stdout);
}

static void registerDump() {
	static bool isDumpRegistered = false;

	if(!isDumpRegistered) {
		atexit(dumpEntries);
		isDumpRegistered = true;
	}
}

EXPIMP void* allocMemory(Module* dynLib, std::size_t nbytes, const char* fileName, int line, bool isArray) {
	registerDump();

	MemoryEntry* entry = (MemoryEntry*)malloc(nbytes + sizeof(MemoryEntry));

	if(!entry) return 0;

	void* ptr = (char*)entry + sizeof(MemoryEntry);

	strncpy(entry->fileName, fileName, sizeof(entry->fileName));
	entry->line = line;
	entry->nbytes = nbytes;
	entry->isArray = isArray;
	entry->next = 0;
	entry->previous = 0;

	if(dynLib) {
		dynLib->getName(entry->moduleName, FILENAME_MAX);
	}

	list.insert(entry);

	return ptr;
}

EXPIMP void freeMemory(Module* dynLib, void* ptr, bool isArray) {
	MemoryEntry* entry = (MemoryEntry*)((char*)ptr - sizeof(MemoryEntry));

	if(dynLib) {
		char moduleName[FILENAME_MAX + 1];

		dynLib->getName(moduleName, FILENAME_MAX);

		if(!strncpy(moduleName, entry->moduleName, FILENAME_MAX)) {
			fprintf(stdout, "Trying to free memory allocated by other module: %s %s\n", moduleName, entry->moduleName);
			fflush(stdout);
		}
	}

	//if(list.exists(entry)) {
		list.remove(entry);
		free(entry);
	//} else {
	//	fprintf(stdout, "Trying to free memory allocated by other module\n");
	//	fflush(stdout);
	//}
}

#endif /* NDEBUG */
