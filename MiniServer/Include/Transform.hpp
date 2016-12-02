#pragma once
// Transform encapsulates position, scale and rotation vectors for models and vertex objects
#include "Rotation.hpp"
#include "maths.vector.hpp"

// Forward declare Object so we can be the best of friends
class Object;

// Simple toggle switches for enabling or disabling inheritence of certain transform properties
struct TransformInheritance
{
    TransformInheritance()
        : InheritPosition(true)
        , InheritScale(true)
        , InheritRotation(true)
        , InheritPreRotation(true)
    {}
    TransformInheritance(bool pos, bool scale, bool rot, bool preRot)
        : InheritPosition(pos)
        , InheritScale(scale)
        , InheritRotation(rot)
        , InheritPreRotation(preRot)
    {}
    bool InheritPosition;
    bool InheritScale;
    bool InheritRotation;
    bool InheritPreRotation;
};

class Transform
{
public:
    Transform()
        : pos(0.f, 0.f, 0.f)
        , scale(1.f, 1.f, 1.f)
        , rotation(0.f, Vector3(0.f, 1.f, 0.f))
    {}

    inline void SetPosition(Vector3& NewPos)  { pos = NewPos; }
    inline void SetScale(Vector3& NewScale)   { scale = NewScale; }
    inline void SetRotation(Rotation& NewRot) { rotation = NewRot; }
    inline void SetPreRotation(Rotation& NewPreRot) { preRotation = NewPreRot; }

    inline const Vector3& GetPosition()  const { return pos; }
    inline       Vector3& GetPosition()        { return pos; }
    inline const Vector3& GetScale()     const { return scale; }
    inline       Vector3& GetScale()           { return scale; }
    inline const Rotation& GetRotation() const { return rotation; }
    inline       Rotation& GetRotation()       { return rotation; }
    inline const Rotation& GetPreRotation() const { return rotation; }
    inline       Rotation& GetPreRotation()       { return rotation; }

    inline bool InheritPosition() const { return transformInheritance.InheritPosition; }
    inline bool InheritScale()    const { return transformInheritance.InheritScale; }
    inline bool InheritRotation() const { return transformInheritance.InheritRotation; }
    inline bool InheritPreRotation() const { return transformInheritance.InheritRotation; }

private:
    Vector3 pos, scale;
    Rotation rotation, preRotation;

    TransformInheritance transformInheritance;

    friend Object;
};
