#pragma once
#include "core/math/numbers.hpp"

namespace core::math
{
  static constexpr Vec3 gGlobalForward = { 0, 1, 0 };
  static constexpr Vec3 gGlobalRight   = { 1, 0, 0 };
  static constexpr Vec3 gGlobalUp      = { 0, 0, 1 };


  class SphereStraightRotation
  {
    Vec3 direction_ = gGlobalForward;
    Vec3 right_     = gGlobalRight;

  public:
    void rotate( f32 upRotationDelta, f32 rightRotationDelta );
    Vec3 getForward() const { return direction_; }
    Vec3 getRight() const { return right_; }
    void setForward( Vec3 v ) { direction_ = v; }
    void setRight( Vec3 v ) { right_ = v; }
  };


  struct Camera
  {
    Vec3 position    = { 0, 0, 0 };
    Vec3 direction   = { 0, 0, 1 }; // on 1 meter circle
    f32  focalLength = 35;          // in millimeters
    f32  aspectRatio = 16.f / 9.f;
    f32  nearPlane   = 0.05f;
    f32  farPlane    = 1000.f;

    Mat4 getWorldToViewTransform() const;
    Mat4 getViewToProjectionTransform() const;
  };

  //     _______
  //    /     / |
  //   |     |  |
  // bz|     |  |
  //   |     | /by
  //   |_____|/
  //  c    bx
  struct BoundingBox
  {
    // bound box center
    Vec3 center;

    // vectors from 'center', which makes edges, like coordinate system of bounding box
    Vec3 bx;
    Vec3 by;
    Vec3 bz;

    bool isInside( Vec3 point ) const;
  };


  Vec3 decodeColorHex( u32 value );
} // namespace core::math
