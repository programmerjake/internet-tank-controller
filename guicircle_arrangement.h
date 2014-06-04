#ifndef GUICIRCLE_ARRANGEMENT_H_INCLUDED
#define GUICIRCLE_ARRANGEMENT_H_INCLUDED

#include "guicontainer.h"
#include "platform.h"
#include <cmath>

using namespace std;

class GUICircleArrangement final : public GUIContainer
{
private:
    float angle;
    float radius;
    float minAngle, maxAngle;
    VectorF followSpot;
    VectorF circleCenter;
    VectorF forwardOffset;
    VectorF reverseOffset;
    Mesh forwardCenterMesh;
    Mesh reverseCenterMesh;

    float getUnadjustedElementAngle(size_t index)
    {
        if(getElementCount() <= 1)
            return 0;
        return (1 - (float)index / (getElementCount() - 1)) * (maxAngle - minAngle) / 2 + minAngle;
    }

    float neededAngle()
    {
        if(getCurrentElementIndex() == NoElement)
            return angle;
        return (maxAngle + minAngle) * 0.5 - (getUnadjustedElementAngle(getCurrentElementIndex()));
    }
public:
    GUICircleArrangement(float minX, float maxX, float minY, float maxY, float minAngle, float maxAngle, VectorF circleCenter, float radius, Mesh forwardCenterMesh, VectorF forwardOffset, Mesh reverseCenterMesh, VectorF reverseOffset, VectorF followSpot)
        : GUIContainer(minX, maxX, minY, maxY), radius(radius), minAngle(minAngle), maxAngle(maxAngle), followSpot(followSpot), circleCenter(circleCenter), forwardOffset(forwardOffset), reverseOffset(reverseOffset), forwardCenterMesh(forwardCenterMesh), reverseCenterMesh(reverseCenterMesh)
    {
        angle = neededAngle();
    }
    virtual Mesh render(float minZ, float maxZ, bool hasFocus) override
    {
        if(hasFocus && Display::frameDeltaTime() > eps)
        {
            float na = neededAngle();
            float newAngle = interpolate<float>(1 - pow(0.01, Display::frameDeltaTime()), angle, na);
            float dangle = newAngle - angle;
            if(abs(dangle) < 0.2 * Display::frameDeltaTime())
                dangle = sgn(dangle) * 0.2 * Display::frameDeltaTime();
            angle += dangle;
            if(abs(angle - na) < 0.2 * Display::frameDeltaTime())
                angle = na;
        }
        for(size_t i = 0; i < getElementCount(); i++)
        {
            shared_ptr<GUIElement> element = getElement(i);
            float a = angle + getUnadjustedElementAngle(i);
            VectorF currentFollowSpot = VectorF(interpolate(followSpot.x, element->minX, element->maxX), interpolate(followSpot.y, element->minY, element->maxY), 0);
            VectorF newFollowSpot = circleCenter + radius * VectorF(cos(a), sin(a), 0);
            VectorF deltaPos = newFollowSpot - currentFollowSpot;
            element->minX += deltaPos.x;
            element->maxX += deltaPos.x;
            element->minY += deltaPos.y;
            element->maxY += deltaPos.y;
        }
        Mesh retval = GUIContainer::render(minZ, maxZ, hasFocus);
        retval->add(transform(Matrix::scale(radius, radius, 1).concat(Matrix::rotateZ(-angle)).concat(Matrix(1, 0, -circleCenter.x - reverseOffset.x, 0,
                                                             0, 1, -circleCenter.y - reverseOffset.y, 0,
                                                             0, 0, 1, 0)).concat(Matrix::scale(maxZ)), reverseCenterMesh));
        retval->add(transform(Matrix::scale(radius, radius, 1).concat(Matrix::rotateZ(angle)).concat(Matrix(1, 0, -circleCenter.x - forwardOffset.x, 0,
                                                             0, 1, -circleCenter.y - forwardOffset.y, 0,
                                                             0, 0, 1, 0)).concat(Matrix::scale(maxZ)), forwardCenterMesh));
        return retval;
    }
    virtual void reset() override
    {
        GUIContainer::reset();
        angle = neededAngle();
    }
};

#endif // GUICIRCLE_ARRANGEMENT_H_INCLUDED
