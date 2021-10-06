#ifndef GLOBAL_H
#define GLOBAL_H
#include "logconsole.h"

inline bool isNoData(double d) {
	return (d==-999999);
}
inline double noData() {
	return -999999;
}
void logline(QString str);

#endif