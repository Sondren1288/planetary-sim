#ifndef TYPES_
#define TYPES_
#include <cmath>
struct position{
    // Position in 3D eutclidean
    double x;
    double y;
    double z;
};
struct velocity {
    // Velocity in 3D euclidean
    double x;
    double y;
    double z;
};
struct acceleration {
    // Acceleration in 3D euclidean
    double x;
    double y;
    double z;
};
struct force {
    // Force in 3D euclidean
    double x;
    double y;
    double z;
};

template <typename Vec3D> Vec3D div(Vec3D current, double divisor) {
    return {
        current.x / divisor, 
        current.y / divisor, 
        current.z / divisor
    };
}

template <typename Vec3D> Vec3D plu(Vec3D current, Vec3D other) {
    return {
        current.x + other.x, 
        current.y + other.y, 
        current.z + other.z 
    };
}

template <typename Vec3D> Vec3D mul(Vec3D current, double factor) {
    return {
        current.x * factor,
        current.y * factor,
        current.z * factor
    };
}
template <typename Vec3D> Vec3D rotateAroundX(Vec3D vec, double rad) {
    double c = cos(rad);
    double s = sin(rad);
    return {
        vec.x,
        c * vec.y - s * vec.z,
        s * vec.y + c * vec.z
    };
}
template <typename Vec3D> Vec3D rotateAroundY(Vec3D vec, double rad) {
    double c = cos(rad);
    double s = sin(rad);
    return {
        c * vec.x + s * vec.z,
        vec.y,
        c * vec.z - s * vec.x
    };
}
template <typename Vec3D> Vec3D rotateAroundZ(Vec3D vec, double rad) {
    double c = cos(rad);
    double s = sin(rad);
    return {
        c * vec.x - s * vec.y,
        s * vec.x + c * vec.y,
        vec.z
    };
}


template <typename Vec3D> double length(Vec3D vec) {
    return sqrt(
          pow(vec.x, 2.0) 
        + pow(vec.y, 2.0) 
        + pow(vec.z, 2.0)
    );
}

template <typename Vec3D> Vec3D normalize(Vec3D vec) {
    double len = length(vec);
    return div(vec, len);
}

#endif
