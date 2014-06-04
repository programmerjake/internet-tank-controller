#ifndef GUICANVAS_H_INCLUDED
#define GUICANVAS_H_INCLUDED

#include "guielement.h"
#include "generate.h"
#include <functional>
#include <stdexcept>

using namespace std;

class GUICanvas : public GUIElement
{
    function<Mesh()> generateMeshFn;
public:
    GUICanvas(float minX, float maxX, float minY, float maxY, function<Mesh()> generateMeshFn)
        : GUIElement(minX, maxX, minY, maxY), generateMeshFn(generateMeshFn)
    {
    }
protected:
    GUICanvas(float minX, float maxX, float minY, float maxY)
        : GUIElement(minX, maxX, minY, maxY), generateMeshFn([]() -> Mesh {throw logic_error("generateMeshFn called instead of overridden generateMesh");})
    {
    }
    VectorF getMousePosition(MouseEvent &event) const
    {
        VectorF retval = Display::transformMouseTo3D(event.x, event.y, 1);
        float height = maxY - minY;
        float width = maxX - minX;
        float scale = min(height, width);
        retval.x -= (minX + maxX) / 2;
        retval.x /= scale;
        retval.x *= 2;
        retval.y -= (minY + maxY) / 2;
        retval.y /= scale;
        retval.y *= 2;
        return retval;
    }
    virtual Mesh generateMesh()
    {
        return generateMeshFn();
    }
    virtual Mesh render(float, float maxZ, bool) override final
    {
        Mesh retval = generateMesh();
        assert(retval != nullptr);
        float width = maxX - minX;
        float height = maxY - minY;
        float scale = 0.5 * min(width, height);
        float centerX = 0.5 * (maxX + minX);
        float centerY = 0.5 * (maxY + minY);
        retval = (Mesh)transform(Matrix(scale, 0, -centerX, 0,
                                        0, scale, -centerY, 0,
                                        0, 0, 1, 0), retval);
        retval = cut(retval, VectorF(-1, 0, -minX), 0);
        retval = cut(retval, VectorF(1, 0, maxX), 0);
        retval = cut(retval, VectorF(0, -1, -minY), 0);
        retval = cut(retval, VectorF(0, 1, maxY), 0);
        retval = cut(retval, VectorF(0, 0, 1), 0.001);
        if(retval->size() == 0)
            return retval;
        float minZ;
        bool first = true;
        for(Triangle tri : *retval)
        {
            if(first)
                minZ = tri.p[0].z;
            else
                minZ = min(tri.p[0].z, minZ);
            minZ = min(tri.p[1].z, minZ);
            minZ = min(tri.p[2].z, minZ);
        }
        assert(minZ < 0);
        return (Mesh)transform(Matrix::scale(maxZ / -minZ), retval);
    }
};

#endif // GUICANVAS_H_INCLUDED
