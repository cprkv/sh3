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

  if( fabsf( forwardX.z ) <= 0.99 )
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

  return glm::lookAt( dxLookFrom, dxLookTo, dxUp ) * blenderToDirectX;
}


Mat4 Camera::getViewToProjectionTransform() const
{
  // todo: this needs to be precalculated, not changed every frame
  return glm::perspective( cameraMMToFovY( focalLength ), aspectRatio, nearPlane, farPlane );
}
