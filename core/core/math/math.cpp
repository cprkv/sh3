#include "core/math/math.hpp"

using namespace core;
using namespace core::math;


namespace
{
  float cameraMMToFovY( float mm )
  {
    float focalLength  = mm * 0.001f;    // in meters
    float sensorLength = 23.9f * 0.001f; // in meters
    return 2.0f * atanf( sensorLength / ( 2.0f * focalLength ) );
  }
} // namespace


void SphereStraightRotation::rotate( f32 upRotationDelta, f32 rightRotationDelta )
{
  auto forwardZ = glm::rotateZ( direction_, upRotationDelta );
  auto rightZ   = glm::rotateZ( right_, upRotationDelta );
  auto forwardX = glm::rotate( forwardZ, rightRotationDelta, rightZ );

  if( fabsf( forwardX.z ) <= 0.99f )
  {
    direction_ = forwardX;
    right_     = rightZ;
  }
}


Mat4 Camera::getWorldToViewTransform() const
{
  constexpr auto blenderToDirectX3 = Mat3( Vec3( 1, 0, 0 ),
                                           Vec3( 0, 0, 1 ),
                                           Vec3( 0, 1, 0 ) );

  constexpr auto blenderToDirectX = Mat4( Vec4( 1, 0, 0, 0 ),
                                          Vec4( 0, 0, 1, 0 ),
                                          Vec4( 0, 1, 0, 0 ),
                                          Vec4( 0, 0, 0, 1 ) );

  auto dxLookFrom = blenderToDirectX3 * position;
  auto dxLookTo   = blenderToDirectX3 * ( position + direction );
  auto dxUp       = blenderToDirectX3 * gGlobalUp;

  return glm::lookAtLH( dxLookFrom, dxLookTo, dxUp ) * blenderToDirectX;
}


Mat4 Camera::getViewToProjectionTransform() const
{
  // todo: this needs to be precalculated, not changed every frame
  float fovY = cameraMMToFovY( focalLength );
  return glm::perspectiveLH_ZO( fovY, aspectRatio, nearPlane, farPlane );
}


bool BoundingBox::isInside( Vec3 point ) const
{
  // relative point coordinates in bound box coordinates
  Vec3 p = point - center;

  auto isInsideProjection = [p]( Vec3 axis ) {
    // normalized
    auto projPtoAxis = glm::dot( p, axis ) /
                       glm::length2( axis );
    return 0.0f <= projPtoAxis && projPtoAxis <= 1.0f;
  };

  return isInsideProjection( bx ) &&
         isInsideProjection( by ) &&
         isInsideProjection( bz );
}


Vec3 core::math::decodeColorHex( u32 value )
{
  return Vec3(
      ( ( value >> 16 ) & 0xFF ) / 255.0, // Extract the RR byte
      ( ( value >> 8 ) & 0xFF ) / 255.0,  // Extract the GG byte
      ( ( value ) & 0xFF ) / 255.0        // Extract the BB byte
  );
}
