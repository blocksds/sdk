// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Michele Di Giorgio, 2024

#ifndef _NDOLLAR_H_
#define _NDOLLAR_H_

/**
 * C API Wrapper
 */
#ifdef _cplusplus
extern "C" {
#endif // _cplusplus

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    char name[16];
    double score;
} Result;

struct NDRecognizer_s; // Opaque type to be used as handle
typedef struct NDRecognizer_s NDRecognizer_s;

NDRecognizer_s* createNDRecognizer();
void destroyNDRecognizer(NDRecognizer_s* rec);
void addGesture(NDRecognizer_s *rec, const Point *stroke, unsigned int length);
Result recognize(NDRecognizer_s *rec);
void flushGestures(NDRecognizer_s* rec);

#ifdef _cplusplus
}
#endif // _cplusplus

#endif // _NDOLLAR_H_
