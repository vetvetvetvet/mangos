#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <algorithm>
#define M_PI 3.14159265358979323846

namespace math { 



    struct Vector3 final {
        float x, y, z;

        Vector3() noexcept : x(0), y(0), z(0) {}

        Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

        inline const float& operator[](int i) const noexcept {
            return reinterpret_cast<const float*>(this)[i];
        }

        inline float& operator[](int i) noexcept {
            return reinterpret_cast<float*>(this)[i];
        }

        Vector3 operator/(float s) const noexcept {
            return *this * (1.0f / s);
        }

        float dot(const Vector3& vec) const noexcept {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        float distance(const Vector3& vector) const noexcept {
            return std::sqrt((vector.x - x) * (vector.x - x) +
                (vector.y - y) * (vector.y - y) +
                (vector.z - z) * (vector.z - z));
        }

        Vector3 operator*(float value) const noexcept {
            return { x * value, y * value, z * value };
        }

        bool operator!=(const Vector3& other) const noexcept {
            return x != other.x || y != other.y || z != other.z;
        }

        float squared() const noexcept {
            return x * x + y * y + z * z;
        }

        Vector3 normalize() const noexcept {
            float mag = magnitude();
            if (mag == 0) return { 0, 0, 0 };
            return { x / mag, y / mag, z / mag };
        }

        Vector3 direction() const noexcept {
            return normalize();
        }

        static const Vector3& one() noexcept {
            static const Vector3 v(1, 1, 1);
            return v;
        }

        static const Vector3& unitX() noexcept {
            static const Vector3 v(1, 0, 0);
            return v;
        }

        static const Vector3& unitY() noexcept {
            static const Vector3 v(0, 1, 0);
            return v;
        }

        static const Vector3& unitZ() noexcept {
            static const Vector3 v(0, 0, 1);
            return v;
        }

        Vector3 cross(const Vector3& b) const noexcept {
            return { y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x };
        }

        Vector3 operator+(const Vector3& vec) const noexcept {
            return { x + vec.x, y + vec.y, z + vec.z };
        }

        Vector3 operator-(const Vector3& vec) const noexcept {
            return { x - vec.x, y - vec.y, z - vec.z };
        }

        Vector3 operator*(const Vector3& vec) const noexcept {
            return { x * vec.x, y * vec.y, z * vec.z };
        }

        Vector3 operator/(const Vector3& vec) const noexcept {
            return { x / vec.x, y / vec.y, z / vec.z };
        }

        Vector3& operator+=(const Vector3& vec) noexcept {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& vec) noexcept {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        Vector3& operator*=(float fScalar) noexcept {
            x *= fScalar;
            y *= fScalar;
            z *= fScalar;
            return *this;
        }

        Vector3& operator/=(const Vector3& other) noexcept {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        bool operator==(const Vector3& other) const noexcept {
            return x == other.x && y == other.y && z == other.z;
        }

        float magnitude() const noexcept {
            return std::sqrt(x * x + y * y + z * z);
        }
    };


    struct Vector2 final {
        float x, y;

        Vector2() noexcept : x(0), y(0) {}

        Vector2(float x, float y) noexcept : x(x), y(y) {}

        Vector2 operator-(const Vector2& other) const noexcept {
            return { x - other.x, y - other.y };
        }

        Vector2 operator+(const Vector2& other) const noexcept {
            return { x + other.x, y + other.y };
        }

        Vector2 operator/(float factor) const noexcept {
            return { x / factor, y / factor };
        }

        Vector2 operator/(const Vector2& factor) const noexcept {
            return { x / factor.x, y / factor.y };
        }

        Vector2 operator*(float factor) const noexcept {
            return { x * factor, y * factor };
        }

        Vector2 operator*(const Vector2& factor) const noexcept {
            return { x * factor.x, y * factor.y };
        }

        bool operator!=(const Vector2& other) const noexcept {
            return x != other.x || y != other.y;
        }

        float magnitude() const noexcept {
            return std::sqrt(x * x + y * y);
        }

        float getMagnitude() const noexcept {
            return std::sqrt(x * x + y * y);
        }
    };

    struct Vector4 final { float x, y, z, w; };

    // Math utility functions
    inline float CalculateDistanceA(Vector2 first, Vector2 sec) {
        return std::sqrt(std::pow(first.x - sec.x, 2) + std::pow(first.y - sec.y, 2));
    }
    
    inline float CalculateDistanceB(Vector3 first, Vector3 sec) {
        return std::sqrt(std::pow(first.x - sec.x, 2) + std::pow(first.y - sec.y, 2) + std::pow(first.z - sec.z, 2));
    }

    struct RaycastResult {
        bool hit;
        Vector3 hitPosition;
        Vector3 normal;
        float distance;
        void* hitObject;
    };

    class Ray {
    public:
        static RaycastResult cast_ray(Vector3 origin, Vector3 direction, float maxDistance, const std::vector<Vector3>& objects) {
            RaycastResult result{ false, Vector3(), Vector3(), maxDistance, nullptr };
            for (const auto& obj : objects) {
                Vector3 toObject = obj - origin;
                float projLength = toObject.dot(direction.normalize());
                if (projLength < 0 || projLength > maxDistance) continue;
                Vector3 projectedPoint = origin + direction.normalize() * projLength;
                if ((projectedPoint - obj).magnitude() < 1.0f) {
                    result.hit = true;
                    result.hitPosition = projectedPoint;
                    result.normal = (projectedPoint - obj).normalize();
                    result.distance = projLength;
                    result.hitObject = (void*)&obj;
                    break;
                }
            }
            return result;
        }

        static std::vector<RaycastResult> cast_ray_multi(Vector3 origin, Vector3 direction, float maxDistance, const std::vector<Vector3>& objects) {
            std::vector<RaycastResult> results;
            for (const auto& obj : objects) {
                Vector3 toObject = obj - origin;
                float projLength = toObject.dot(direction.normalize());
                if (projLength < 0 || projLength > maxDistance) continue;
                Vector3 projectedPoint = origin + direction.normalize() * projLength;
                if ((projectedPoint - obj).magnitude() < 1.0f) {
                    results.push_back({ true, projectedPoint, (projectedPoint - obj).normalize(), projLength, (void*)&obj });
                }
            }
            return results;
        }

        static bool is_point_inside_object(Vector3 point, const std::vector<Vector3>& objects) {
            for (const auto& obj : objects) {
                if ((point - obj).magnitude() < 1.0f) {
                    return true;
                }
            }
            return false;
        }
    };

    struct Matrix3 final {
        float data[9];

        Vector3 MatrixToEulerAngles() const {
            Vector3 angles;

            angles.y = std::asin(std::clamp(data[6], -1.0f, 1.0f));

            if (std::abs(data[6]) < 0.9999f) {
                angles.x = std::atan2(-data[7], data[8]);
                angles.z = std::atan2(-data[3], data[0]);
            }
            else {
                angles.x = 0.0f;
                angles.z = std::atan2(data[1], data[4]);
            }

            angles.x = angles.x * (180.0f / M_PI);
            angles.y = angles.y * (180.0f / M_PI);
            angles.z = angles.z * (180.0f / M_PI);

            return angles;
        }

        Matrix3 EulerAnglesToMatrix(const Vector3& angles) const {
            Matrix3 rotationMatrix;

            float pitch = angles.x * (M_PI / 180.0f);
            float yaw = angles.y * (M_PI / 180.0f);
            float roll = angles.z * (M_PI / 180.0f);

            float cosPitch = std::cos(pitch), sinPitch = std::sin(pitch);
            float cosYaw = std::cos(yaw), sinYaw = std::sin(yaw);
            float cosRoll = std::cos(roll), sinRoll = std::sin(roll);

            rotationMatrix.data[0] = cosYaw * cosRoll;
            rotationMatrix.data[1] = cosYaw * sinRoll;
            rotationMatrix.data[2] = -sinYaw;

            rotationMatrix.data[3] = sinPitch * sinYaw * cosRoll - cosPitch * sinRoll;
            rotationMatrix.data[4] = sinPitch * sinYaw * sinRoll + cosPitch * cosRoll;
            rotationMatrix.data[5] = sinPitch * cosYaw;

            rotationMatrix.data[6] = cosPitch * sinYaw * cosRoll + sinPitch * sinRoll;
            rotationMatrix.data[7] = cosPitch * sinYaw * sinRoll - sinPitch * cosRoll;
            rotationMatrix.data[8] = cosPitch * cosYaw;

            return rotationMatrix;
        }

        Vector3 GetForwardVector() const { return { data[2], data[5], data[8] }; }
        Vector3 GetRightVector() const { return { data[0], data[3], data[6] }; }
        Vector3 GetUpVector() const { return { data[1], data[4], data[7] }; }
        
        Vector3 getColumn(int column) const {
            if (column == 0) return { data[0], data[3], data[6] };
            if (column == 1) return { data[1], data[4], data[7] };
            if (column == 2) return { data[2], data[5], data[8] };
            return { 0, 0, 0 };
        }

        Matrix3 Transpose() const {
            Matrix3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.data[i + j * 3] = data[j + i * 3];
            return result;
        }

        Vector3 multiplyVector(const Vector3& vec) const {
            return {
                data[0] * vec.x + data[1] * vec.y + data[2] * vec.z,
                data[3] * vec.x + data[4] * vec.y + data[5] * vec.z,
                data[6] * vec.x + data[7] * vec.y + data[8] * vec.z
            };
        }

        Matrix3 operator*(const Matrix3& other) const {
            Matrix3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.data[i + j * 3] = data[i * 3 + 0] * other.data[0 + j] +
                    data[i * 3 + 1] * other.data[1 + j] +
                    data[i * 3 + 2] * other.data[2 + j];
            return result;
        }

        Matrix3 operator+(const Matrix3& other) const {
            Matrix3 result;
            for (int i = 0; i < 9; ++i)
                result.data[i] = data[i] + other.data[i];
            return result;
        }

        Matrix3 operator-(const Matrix3& other) const {
            Matrix3 result;
            for (int i = 0; i < 9; ++i)
                result.data[i] = data[i] - other.data[i];
            return result;
        }

        Matrix3 operator/(float scalar) const {
            Matrix3 result;
            for (int i = 0; i < 9; ++i)
                result.data[i] = data[i] / scalar;
            return result;
        }
    };

    struct Matrix4 final { float data[16]; };

    struct CFrame {
        Matrix3 rotation_matrix;
        Vector3 position = { 0, 0, 0 };


        CFrame() = default;

        CFrame(Vector3 position) : position{ position } {}




        static CFrame lerp(const CFrame& start, const CFrame& end, float alpha) {
            Vector3 newPosition = start.position + (end.position - start.position) * alpha;
            return CFrame(newPosition);
        }

        static CFrame FromVector3(const Vector3& vec) {
            return CFrame(Vector3(vec.x, vec.y, vec.z));
        }
    };
};