/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "CCPUParticle3DCircleEmitter.h"
#include "3dparticle/ParticleUniverse/CCPUParticleSystem3D.h"
#include "3dparticle/ParticleUniverse/CCPUParticle3DUtil.h"
#include "base/ccRandom.h"

NS_CC_BEGIN

// Constants
const float PUParticle3DCircleEmitter::DEFAULT_RADIUS = 100.0f;
const float PUParticle3DCircleEmitter::DEFAULT_STEP = 0.1f;
const float PUParticle3DCircleEmitter::DEFAULT_ANGLE = 0.0f;
const bool PUParticle3DCircleEmitter::DEFAULT_RANDOM = true;
const Vec3 PUParticle3DCircleEmitter::DEFAULT_NORMAL(0, 0, 0);

//-----------------------------------------------------------------------
PUParticle3DCircleEmitter::PUParticle3DCircleEmitter(void) : 
    PUParticle3DEmitter(),
    _radius(DEFAULT_RADIUS),
    _circleAngle(DEFAULT_ANGLE),
    _originalCircleAngle(DEFAULT_ANGLE),
    _step(DEFAULT_STEP),
    _random(DEFAULT_RANDOM),
    _orientation(),
    _normal(DEFAULT_NORMAL),
    _x(0.0f),
    _z(0.0f)
{
}
//-----------------------------------------------------------------------
const float PUParticle3DCircleEmitter::getRadius(void) const
{
    return _radius;
}
//-----------------------------------------------------------------------
void PUParticle3DCircleEmitter::setRadius(const float radius)
{
    _radius = radius;
}
//-----------------------------------------------------------------------
const float PUParticle3DCircleEmitter::getCircleAngle(void) const
{
    return _originalCircleAngle;
}
//-----------------------------------------------------------------------
void PUParticle3DCircleEmitter::setCircleAngle(const float circleAngle)
{
    _originalCircleAngle = circleAngle;
    _circleAngle = circleAngle;
}
//-----------------------------------------------------------------------
const float PUParticle3DCircleEmitter::getStep(void) const
{
    return _step;
}
//-----------------------------------------------------------------------
void PUParticle3DCircleEmitter::setStep(const float step)
{
    _step = step;
}
//-----------------------------------------------------------------------
const bool PUParticle3DCircleEmitter::isRandom(void) const
{
    return _random;
}
//-----------------------------------------------------------------------
void PUParticle3DCircleEmitter::setRandom(const bool random)
{
    _random = random;
}
//-----------------------------------------------------------------------
const Quaternion& PUParticle3DCircleEmitter::getOrientation(void) const
{
    return _orientation;
}
//----------------------------------------------------------------------- 
const Vec3& PUParticle3DCircleEmitter::getNormal(void) const
{ 
    return _normal;
} 
//----------------------------------------------------------------------- 
void PUParticle3DCircleEmitter::setNormal(const Vec3& normal) 
{ 
    //_orientation = Vec3::UNIT_Y.getRotationTo(normal, Vec3::UNIT_X);
    _orientation = getRotationTo(Vec3::UNIT_Y, normal, Vec3::UNIT_X);
    _normal = normal;
}
//-----------------------------------------------------------------------
void PUParticle3DCircleEmitter::notifyStart (void)
{
    // Reset the attributes to allow a restart.
    _circleAngle = _originalCircleAngle;
}
//----------------------------------------------------------------------- 
void PUParticle3DCircleEmitter::initParticlePosition(PUParticle3D* particle)
{
    float angle = 0;
    if (_random)
    {
        // Choose a random position on the circle.
        angle = cocos2d::random(0.0, M_PI * 2.0);
    }
    else
    {
        // Follow the contour of the circle.
        _circleAngle += _step;
        _circleAngle = _circleAngle > M_PI * 2.0f ? _circleAngle - (M_PI * 2.0) : _circleAngle;
        angle = _circleAngle;
    }

    _x = cosf(angle);
    _z = sinf(angle);
    //ParticleSystem* sys = mParentTechnique->getParentSystem();
    //if (sys)
    {
        // Take both orientation of the node and its own orientation, based on the normal, into account
        Mat4 rotMat;
        Mat4::createRotation(static_cast<PUParticleSystem3D *>(_particleSystem)->getDerivedOrientation() * _orientation, &rotMat);
        particle->position = getDerivedPosition() + 
            /*sys->getDerivedOrientation() * */rotMat * (Vec3(_x * _radius * _emitterScale.x, 0, _z * _radius * _emitterScale.z));
    }
    //else
    //{
    //	particle->position = getDerivedPosition() + _emitterScale * ( mOrientation * Vec3(mX * mRadius, 0, mZ * mRadius) );
    //}
    particle->originalPosition = particle->position;
}
//-----------------------------------------------------------------------
void PUParticle3DCircleEmitter::initParticleDirection(PUParticle3D* particle)
{
    if (_autoDirection)
    {
        // The value of the direction vector that has been set does not have a meaning for
        // the circle emitter.
        float angle = 0.0f;
        generateAngle(angle);
        if (angle != 0.0f)
        {
            //particle->direction = (mOrientation * Vec3(mX, 0, mZ) ).randomDeviant(angle, mUpVector);
            Mat4 mat;
            Mat4::createRotation(static_cast<PUParticleSystem3D *>(_particleSystem)->getDerivedOrientation() * _orientation, &mat);
            Vec3 temp = mat * Vec3(_x, 0, _z);

            particle->direction = PUParticle3DUtil::randomDeviant(temp, angle, _upVector);

            particle->originalDirection = particle->direction;
        }
        else
        {
            Mat4 rotMat;
            Mat4::createRotation(static_cast<PUParticleSystem3D *>(_particleSystem)->getDerivedOrientation() * _orientation, &rotMat);
            particle->direction = Vec3(_x, 0, _z);
            particle->direction = rotMat * Vec3(_x, 0, _z);
        }
    }
    else
    {
        // Use the standard way
        PUParticle3DEmitter::initParticleDirection(particle);
    }
}

PUParticle3DCircleEmitter* PUParticle3DCircleEmitter::create()
{
    auto pe = new PUParticle3DCircleEmitter();
    pe->autorelease();
    return pe;
}

cocos2d::Quaternion PUParticle3DCircleEmitter::getRotationTo( const Vec3 &src, const Vec3& dest, const Vec3& fallbackAxis /*= Vec3::ZERO*/ ) const
{
    // Based on Stan Melax's article in Game Programming Gems
    Quaternion q;
    // Copy, since cannot modify local
    Vec3 v0 = src;
    Vec3 v1 = dest;
    v0.normalize();
    v1.normalize();

    float d = v0.dot(v1);
    // If dot == 1, vectors are the same
    if (d >= 1.0f)
    {
        return Quaternion();
    }
    if (d < (1e-6f - 1.0f))
    {
        if (fallbackAxis != Vec3::ZERO)
        {
            // rotate 180 degrees about the fallback axis
            q.set(fallbackAxis, (float)M_PI);
            //q.FromAngleAxis(Radian(Math::PI), fallbackAxis);
        }
        else
        {
            // Generate an axis
            Vec3 axis/* = Vec3::UNIT_X.crossProduct(*this)*/;
            Vec3::cross(Vec3::UNIT_X, src, &axis);
            if (axis.lengthSquared() < (1e-06 * 1e-06)) // pick another if colinear
                //axis = Vec3::UNIT_Y.crossProduct(*this);
                Vec3::cross(Vec3::UNIT_Y, src, &axis);
            axis.normalize();

            //q.FromAngleAxis(Radian(Math::PI), axis);
            q.set(axis, (float)M_PI);
        }
    }
    else
    {
        /*float s = Math::Sqrt( (1+d)*2 );*/
        float s = sqrtf( (1+d)*2 );
        float invs = 1 / s;

        Vec3 c /*= v0.crossProduct(v1)*/;
        Vec3::cross(v0, v1, &c);

        q.x = c.x * invs;
        q.y = c.y * invs;
        q.z = c.z * invs;
        q.w = s * 0.5f;
        q.normalize();
    }
    return q;
}

NS_CC_END