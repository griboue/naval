#ifndef DATA_H_
#define DATA_H_

const char *boats[] = {
	"Destroyers",
	"Submarines",
	"Battleships",
	"Carrier",
};

const int boats_size[] = {
	2,
	3,
	4,
	5,
};

const char boats_sign[] = {
	'D',
	'S',
	'B',
	'C',
};

#define number_boats (sizeof(boats) / sizeof(const char *))

#endif
