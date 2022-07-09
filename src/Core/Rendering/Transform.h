#pragma once

#include <DirectXMath.h>

namespace Kraid
{

    using namespace DirectX;

    class Transform
    {
    public:
        float translation[3] = {0,0,0};
        float scale[3] = {1,1,1};
        float rotation[3] = {0,0,0};
    private:
        mutable bool has_changed = true;
        mutable XMMATRIX cached_model_matrix = XMMatrixIdentity();
    public:
        Transform() = default;
        Transform(const Transform& other);
        Transform(
                const float translation[3],
                const float scale[3],
                const float rotation[3]);

        void SetTranslation(const float x, const float y, const float z);
        void TranslateBy(const float x, const float y, const float z);
        void SetScale(const float x, const float y, const float z);
        void ScaleBy(const float x, const float y, const float z);
        void SetRotation(const float x, const float y, const float z);
        void RotateBy(const float x, const float y, const float z);

        XMMATRIX GetModelMatrix() const;
        Transform operator+(const Transform& other);
        Transform& operator=(const Transform& other);

        bool HasChanged() const
        {
            return this->has_changed;
        }

        void SetChangedFlag(const bool flag_value)
        {
            this->has_changed = flag_value;
        }
    };

}
