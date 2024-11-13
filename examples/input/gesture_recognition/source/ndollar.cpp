// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Michele Di Giorgio, 2024

#include "ndollar.h"

#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

class NDRecognizer {
public:
    NDRecognizer() = default;
    void addGesture(const std::vector<Point> &points);
    void flushGestures();
    Result recognize();
private:
    std::vector<std::vector<Point>> _strokes;
};

typedef struct {
    double width;
    double height;
} BBox;

constexpr bool useBoundedRotationInvariance = false;

constexpr unsigned int NumPoints = 96;
constexpr int          I         = 12;
constexpr double       size      = 250.0;
constexpr double       threshold = 0.30;
constexpr double       theta     = 45.0;
constexpr double       precision = 2.0;
constexpr Point        origin    = { 0.0, 0.0 };
constexpr double       pi        = 3.1415926535;
constexpr double       phi       = 1.6180339887; // golden ratio

class Unistroke {
public:
    Unistroke(std::vector<Point> &points);
    std::vector<Point> getUnistrokes() const;
    Point getUnitVector() const;
private:
    std::vector<Point> _unistrokes;
    Point              _unitVector;
};

class Multistroke {
public:
    Multistroke(const std::string name, const std::vector<std::vector<Point>> &points);
    std::vector<Unistroke> getUnistrokes() const;
    std::string getName() const;
    unsigned int getNumStrokes() const;
private:
    std::vector<Unistroke> _makeUnistroke(
            const std::vector<std::vector<Point>> &strokes,
            const std::vector<std::vector<int>> &orders);

    void _heapPermute(int n,
                     std::vector<int> &order,
                     std::vector<std::vector<int>> &orders);

    std::string            _name;
    unsigned int           _numStrokes;
    std::vector<Unistroke> _strokes;
};

std::vector<Multistroke> multistrokesLUT = {
    Multistroke(
        "T",
        {{ Point{30, 7}, Point{103, 7}},
         { Point{66, 7}, Point{66, 87}}}
    ),
    Multistroke(
        "N",
        {{ Point{177, 92}, Point{177, 2}},
		 { Point{182, 1}, Point{246, 95}},
		 { Point{247, 87}, Point{247, 1}}}
    ),
    Multistroke(
        "D",
        {{ Point{345,9}, Point{345,87}},
		 { Point{351,8}, Point{363,8}, Point{372,9}, Point{380,11}, Point{386,14}, Point{391,17}, Point{394,22}, Point{397,28}, Point{399,34}, Point{400,42}, Point{400,50}, Point{400,56}, Point{399,61}, Point{397,66}, Point{394,70}, Point{391,74}, Point{386,78}, Point{382,81}, Point{377,83}, Point{372,85}, Point{367,87}, Point{360,87}, Point{355,88}, Point{349,87}}}
    ),
    Multistroke(
        "P",
        {{ Point{507,8}, Point{507,87}},
         { Point{513,7}, Point{528,7}, Point{537,8}, Point{544,10}, Point{550,12}, Point{555,15}, Point{558,18}, Point{560,22}, Point{561,27}, Point{562,33}, Point{561,37}, Point{559,42}, Point{556,45}, Point{550,48}, Point{544,51}, Point{538,53}, Point{532,54}, Point{525,55}, Point{519,55}, Point{513,55}, Point{510,55}}}
    ),
    Multistroke(
        "X",
        {{ Point{30,146}, Point{106,222}},
         { Point{30,225}, Point{106,146}}}
    ),
    Multistroke(
        "H",
        {{ Point{188,137}, Point{188,225}},
         { Point{188,180}, Point{241,180}},
         { Point{241,137}, Point{241,225}}}
    ),
    Multistroke(
        "I",
        {{ Point{371,149}, Point{371,221}},
		 { Point{341,149}, Point{401,149}},
		 { Point{341,221}, Point{401,221}}}
    ),
    Multistroke(
        "exclamation",
        {{ Point{526,142}, Point{526,204}},
		 { Point{526,221}}}
    ),
    Multistroke(
        "line",
        {{ Point{12,347}, Point{119,347}}}
    ),
    Multistroke(
        "five-point star",
        {{ Point{177,396}, Point{223,299}, Point{262,396}, Point{168,332}, Point{278,332}, Point{184,397}}}
    ),
    Multistroke(
        "null",
        {{ Point{382,310}, Point{377,308}, Point{373,307}, Point{366,307}, Point{360,310}, Point{356,313}, Point{353,316}, Point{349,321}, Point{347,326}, Point{344,331}, Point{342,337}, Point{341,343}, Point{341,350}, Point{341,358}, Point{342,362}, Point{344,366}, Point{347,370}, Point{351,374}, Point{356,379}, Point{361,382}, Point{368,385}, Point{374,387}, Point{381,387}, Point{390,387}, Point{397,385}, Point{404,382}, Point{408,378}, Point{412,373}, Point{416,367}, Point{418,361}, Point{419,353}, Point{418,346}, Point{417,341}, Point{416,336}, Point{413,331}, Point{410,326}, Point{404,320}, Point{400,317}, Point{393,313}, Point{392,312}},
         { Point{418,309}, Point{337,390}}}
    ),
    Multistroke(
        "arrowhead",
        {{ Point{506,349}, Point{574,349}},
         { Point{525,306}, Point{584,349}, Point{525,388}}}
    ),
    Multistroke(
        "pitchfork",
        {{ Point{38,470}, Point{36,476}, Point{36,482}, Point{37,489}, Point{39,496}, Point{42,500}, Point{46,503}, Point{50,507}, Point{56,509}, Point{63,509}, Point{70,508}, Point{75,506}, Point{79,503}, Point{82,499}, Point{85,493}, Point{87,487}, Point{88,480}, Point{88,474}, Point{87,468}},
		 { Point{62,464}, Point{62,571}}}
    ),
    Multistroke(
        "asterisk",
        {{ Point{325,499}, Point{417,557}},
		 { Point{417,499}, Point{325,557}},
         { Point{371,486}, Point{371,571}}}
    ),
    Multistroke(
        "half-note",
        {{ Point{546,465}, Point{546,531}},
		 { Point{540,530}, Point{536,529}, Point{533,528}, Point{529,529}, Point{524,530}, Point{520,532}, Point{515,535}, Point{511,539}, Point{508,545}, Point{506,548}, Point{506,554}, Point{509,558}, Point{512,561}, Point{517,564}, Point{521,564}, Point{527,563}, Point{531,560}, Point{535,557}, Point{538,553}, Point{542,548}, Point{544,544}, Point{546,540}, Point{546,536}}}
    ),
};

double angleBetweenVectors(const Point &a, const Point &b)
{
    return std::acos(a.x * b.x + a.y * b.y);
}

inline double deg2Rad(const double deg)
{
    return deg * pi / 180.0;
}

double distance(const Point &a, const Point &b)
{
    const double dx = b.x - a.x;
    const double dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

double pathDistance(const std::vector<Point> &pathA, const std::vector<Point> &pathB)
{
    double d = 0;
    for (unsigned int i = 0; i < pathA.size(); ++i) {
        d += distance(pathA[i], pathB[i]);
    }
    return d / static_cast<double>(pathA.size());
}

double pathLength(const std::vector<Point> &path)
{
    double d = 0;
    for (unsigned int i = 1; i < path.size(); ++i) {
        d += distance(path[i - 1], path[i]);
    }
    return d;
}

Point centroid(const std::vector<Point> &points)
{
    double xSum = 0.0;
    double ySum = 0.0;

    for (const auto &p : points) {
        xSum += p.x;
        ySum += p.y;
    }
    const Point c = {
        xSum / static_cast<double>(points.size()),
        ySum / static_cast<double>(points.size())
    };
    return c;
}

double indicativeAngle(const std::vector<Point> &points)
{
    const Point c = centroid(points);
    return std::atan2(c.y - points[0].y, c.x - points[0].x);
}

void rotateBy(std::vector<Point> &points, const double deg)
{
    const Point  c   = centroid(points);
    const double rad = deg2Rad(deg);
    const double cos = std::cos(rad);
    const double sin = std::sin(rad);

    for (unsigned int i = 0; i < points.size(); ++i) {
        const double dx = points[i].x - c.x;
        const double dy = points[i].y - c.y;

        points[i].x = dx * cos - dy * sin + c.x;
        points[i].y = dx * sin + dy * cos + c.y;
    }
}

double distanceAtAngle(const std::vector<Point> &points,
                       const std::vector<Point> &T,
                       const double deg)
{
    std::vector<Point> tmp(points);
    rotateBy(tmp, deg);
    return pathDistance(tmp, T);
}

double distanceAtBestAngle(const std::vector<Point> &points,
                           const std::vector<Point> &T,
                           const double thetaA,
                           const double thetaB,
                           const double deltaTheta)
{
    double thetaARad           = deg2Rad(thetaA);
    double thetaBRad           = deg2Rad(thetaB);
    const double deltaThetaRad = deg2Rad(deltaTheta);

    double x1 = phi * thetaARad + (1.0 - phi) * thetaBRad;
    double f1 = distanceAtAngle(points, T, x1);
    double x2 = (1.0 - phi) * thetaARad + phi * thetaBRad;
    double f2 = distanceAtAngle(points, T, x2);

    while (std::abs(thetaARad - thetaBRad) < deltaThetaRad) {
        if (f1 < f2) {
            thetaBRad = x2;
            x2 = x1;
            f2 = f1;
            x1 = phi * thetaARad + (1.0 - phi) * thetaBRad;
            f1 = distanceAtAngle(points, T, x1);
        } else {
            thetaARad = x1;
            x1 = x2;
            f1 = f2;
            x2 = (1.0 - phi) * thetaARad + phi * thetaBRad;
            f2 = distanceAtAngle(points, T, x2);

        }
    }
    return std::min(f1, f2);
}

void resample(std::vector<Point> &points,
              unsigned int n,
              std::vector<Point> &newPoints)
{
    double I = pathLength(points) / static_cast<double>(n - 1);
    double D = 0.0;
    newPoints.push_back(points[0]);
    for (unsigned int i = 1; i < points.size(); ++i) {
        const double d = distance(points[i - 1], points[i]);
        if ((D + d) >= I) {
            const Point p = {
                points[i - 1].x + ((I - D) / d) * (points[i].x - points[i - 1].x),
                points[i - 1].y + ((I - D) / d) * (points[i].y - points[i - 1].y)
            };
            newPoints.push_back(p);
            // The new point will be the next points[i]
            points.insert(std::next(points.begin(), i), p);
            D = 0.0;
        } else {
            D += d;
        }
    }

     // Rounding-error may cause issues with adding the last point. Add it manually if so
    if (newPoints.size() == n - 1) {
        newPoints.push_back(points[points.size() - 1]);
    }
}

void translateTo(std::vector<Point> &points, const Point &k)
{
    const Point c = centroid(points);
    for (unsigned int i = 0; i < points.size(); ++i) {
        points[i].x += k.x - c.x;
        points[i].y += k.y - c.y;
    }
}

Point calcStartUnitVector(const std::vector<Point> &points, int index)
{
    const Point q = {
        points[index].x - points[0].x,
        points[index].y - points[0].y
    };
    const double len = std::sqrt(q.x * q.x + q.y * q.y);
    const Point  v   = { q.x / len, q.y / len };
    return v;
}

BBox boundingBox(const std::vector<Point> &points)
{
    double minX = points[0].x;
    double maxX = points[0].x;
    double minY = points[0].y;
    double maxY = points[0].y;
    for (unsigned int i = 1; i < points.size(); ++i) {
        if (minX < points[i].x) {
            minX = points[i].x;
        }
        if (maxX > points[i].x) {
            maxX = points[i].x;
        }
        if (minY < points[i].y) {
            minY = points[i].y;
        }
        if (maxY > points[i].y) {
            maxY = points[i].y;
        }
    }
    const BBox b = { maxX - minX, maxY - minY };
    return b;
}

void scaleDimTo(std::vector<Point> &points, double size, double threshold)
{
    const BBox b        = boundingBox(points);
    const bool uniform  = std::min(b.width / b.height, b.height / b.width) <= threshold;
    const double width  = (uniform) ? std::max(b.width, b.height) : b.width;
    const double height = (uniform) ? std::max(b.width, b.height) : b.height;

    for (unsigned int i = 0; i < points.size(); ++i) {
        points[i].x = points[i].x * size / width;
        points[i].y = points[i].y * size / height;
    }
}

void checkRestoreOrientation(std::vector<Point> &points, double angle)
{
    if (useBoundedRotationInvariance) {
        rotateBy(points, angle);
    }
}

std::vector<Point> combineStrokes(const std::vector<std::vector<Point>> &strokes)
{
    std::vector<Point> points;
    for (const auto &u : strokes) {
        for (const auto &p : u) {
            points.push_back(p);
        }
    }
    return points;
}

Unistroke::Unistroke(std::vector<Point> &points)
{
    resample(points, NumPoints, _unistrokes);
    double angle = indicativeAngle(_unistrokes);
    rotateBy(_unistrokes, -angle);
    scaleDimTo(_unistrokes, size, threshold);
    checkRestoreOrientation(_unistrokes, angle);
    translateTo(_unistrokes, origin);
    _unitVector  = calcStartUnitVector(_unistrokes, I);
}

std::vector<Point> Unistroke::getUnistrokes() const { return _unistrokes; }
Point Unistroke::getUnitVector() const { return _unitVector; }

Multistroke::Multistroke(const std::string name, const std::vector<std::vector<Point>> &points)
{
    _name = name;
    _numStrokes = points.size();

    // Generate unistrokes (step 1)
    std::vector<int> order(points.size());
    for (unsigned int i = 0; i < points.size(); ++i) {
        order[i] = i;
    }
    std::vector<std::vector<int>> orders;
    _heapPermute(points.size(), order, orders);
    _strokes = _makeUnistroke(points, orders);
}

std::vector<Unistroke> Multistroke::_makeUnistroke(
        const std::vector<std::vector<Point>> &strokes,
        const std::vector<std::vector<int>> &orders)
{
    std::vector<Unistroke> unistrokes;
    for (const auto &order : orders) {
        const int len = order.size();
        const int pow2Len = std::pow(2, len);
        for (int bit = 0; bit < pow2Len; ++bit) {
            std::vector<Point> points;
            for (int i = 0; i < len; ++i) {
                if (i & bit) {
                    for (int j = strokes[order[i]].size() - 1; j >= 0; --j) {
                        points.push_back(strokes[order[i]][j]);
                    }
                } else {
                    for (const auto &p : strokes[order[i]]) {
                        points.push_back(p);
                    }
                }
            }
            unistrokes.push_back(Unistroke(points));
        }
    }
    return unistrokes;
}

void Multistroke::_heapPermute(int n,
                               std::vector<int> &order,
                               std::vector<std::vector<int>> &orders)
{
    if (n == 1) {
        orders.push_back(order);
    } else {
        for (int i = 0; i < n; ++i) {
            _heapPermute(n - 1, order, orders);
            const int idx = (n % 2 != 0) ? 0 : i;
            const int tmp = order[idx];
            order[idx]    = order[n - 1];
            order[n - 1]  = tmp;
        }
    }
}

std::vector<Unistroke> Multistroke::getUnistrokes() const { return _strokes; }
std::string Multistroke::getName() const { return _name; }
unsigned int Multistroke::getNumStrokes() const { return _numStrokes; }

void NDRecognizer::addGesture(const std::vector<Point> &points)
{
    _strokes.push_back(std::move(points));
}

void NDRecognizer::flushGestures()
{
    _strokes.clear();
}

Result NDRecognizer::recognize()
{
    Result match = { "No match", 0.0 };

    if (_strokes.empty()) {
        return match;
    }

    std::vector<Point> points = combineStrokes(_strokes);
    Unistroke candidate(points);

    double b     = std::numeric_limits<double>::max();
    int    idx   = -1;
    for (unsigned int i = 0; i < multistrokesLUT.size(); ++i) {
        const std::vector<Unistroke> &unistrokes = multistrokesLUT[i].getUnistrokes();
        if (_strokes.size() == multistrokesLUT[i].getNumStrokes()) {
            // Require same number of strokes
            for (unsigned int j = 0; j < unistrokes.size(); ++j) {
                if (angleBetweenVectors(candidate.getUnitVector(),
                                        unistrokes[j].getUnitVector()) <= threshold) {
                    // Strokes start in the same direction
                    const double d = distanceAtBestAngle(candidate.getUnistrokes(),
                                                         unistrokes[j].getUnistrokes(),
                                                         -theta,
                                                         theta,
                                                         precision);
                    if (d < b) {
                        b = d; // Best least distance
                        idx = static_cast<int>(i);
                    }
                }
            }
        }
    }
    if (idx != -1) {
        const std::string name = multistrokesLUT[idx].getName();
        std::strncpy(match.name, name.c_str(), name.size());
        match.name[name.size()] = '\0';
        match.score = 1.0 - (b / ((std::sqrt(2.0 * size * size) / 2.0)));
    }
    return match;
}

#ifdef _cplusplus
extern "C" {
#endif // _cplusplus

NDRecognizer_s* createNDRecognizer()
{
    return reinterpret_cast<NDRecognizer_s*>(new NDRecognizer());
}

void destroyNDRecognizer(NDRecognizer_s* rec)
{
    delete reinterpret_cast<NDRecognizer*>(rec);
}

void addGesture(NDRecognizer_s *rec, const Point *stroke, unsigned int length)
{
    if (rec == NULL) {
        return;
    }
    if (stroke == NULL) {
        return;
    }

    std::vector<Point> points(length);
    for (unsigned int i = 0; i < length; ++i) {
        points[i] = stroke[i];
    }
    auto ndrec = reinterpret_cast<NDRecognizer*>(rec);
    ndrec->addGesture(points);
}

Result recognize(NDRecognizer_s *rec)
{
    if (rec == NULL) {
        return { "No match", 0.0 };
    }
    auto ndrec = reinterpret_cast<NDRecognizer*>(rec);
    return ndrec->recognize();
}

void flushGestures(NDRecognizer_s* rec)
{
    if (rec == NULL) {
        return;
    }
    auto ndrec = reinterpret_cast<NDRecognizer*>(rec);
    ndrec->flushGestures();
}

#ifdef _cplusplus
}
#endif // _cplusplus
