#ifndef GENERATE_H_INCLUDED
#define GENERATE_H_INCLUDED

#include "mesh.h"
#include "util.h"
#include <utility>
#include <vector>

using namespace std;

inline Mesh invert(Mesh mesh)
{
    vector<Triangle> triangles;
    triangles.reserve(mesh->size());

    for(Triangle tri : *mesh)
    {
        swap(tri.p[0], tri.p[1]);
        swap(tri.c[0], tri.c[1]);
        swap(tri.t[0], tri.t[1]);
        triangles.push_back(tri);
    }

    return Mesh(new Mesh_t(mesh->texture(), triangles));
}

inline TransformedMesh invert(TransformedMesh mesh)
{
    mesh.mesh = invert(mesh.mesh);
    return mesh;
}

inline Mesh cut(Mesh mesh, VectorF normal, float d) /// keep where dot(normal, p) + d < 0
{
    vector<Triangle> triangles;
    triangles.reserve(mesh->size() * 2);

    for(Triangle tri : *mesh)
    {
        VectorF resultPoints[4];
        Color resultColors[4];
        TextureCoord resultTextureCoords[4];
        int pointCount = 0;
        bool inside[3];

        for(int i = 0; i < 3; i++)
        {
            if(dot(tri.p[i], normal) + d < 0)
            {
                inside[i] = true;
            }
            else
            {
                inside[i] = false;
            }
        }

        for(int currentPt = 0; currentPt < 3; currentPt++)
        {
            int lastPt = currentPt - 1;

            if(lastPt < 0)
            {
                lastPt += 3;
            }

            if(inside[currentPt])
            {
                if(!inside[lastPt])
                {
                    float t = findIntersectionPoint(tri.p[lastPt], tri.p[currentPt], normal, d);
                    resultPoints[pointCount] = interpolate(t, tri.p[lastPt], tri.p[currentPt]);
                    resultColors[pointCount] = interpolate(t, tri.c[lastPt], tri.c[currentPt]);
                    resultTextureCoords[pointCount++] = interpolate(t, tri.t[lastPt], tri.t[currentPt]);
                }

                resultPoints[pointCount] = tri.p[currentPt];
                resultColors[pointCount] = tri.c[currentPt];
                resultTextureCoords[pointCount++] = tri.t[currentPt];
            }
            else if(inside[lastPt])
            {
                float t = findIntersectionPoint(tri.p[lastPt], tri.p[currentPt], normal, d);
                resultPoints[pointCount] = interpolate(t, tri.p[lastPt], tri.p[currentPt]);
                resultColors[pointCount] = interpolate(t, tri.c[lastPt], tri.c[currentPt]);
                resultTextureCoords[pointCount++] = interpolate(t, tri.t[lastPt], tri.t[currentPt]);
            }
        }

        if(pointCount == 3)
        {
            triangles.push_back(Triangle(resultPoints[0], resultColors[0], resultTextureCoords[0],
                                         resultPoints[1], resultColors[1], resultTextureCoords[1],
                                         resultPoints[2], resultColors[2], resultTextureCoords[2]));
        }
        else if(pointCount == 4)
        {
            triangles.push_back(Triangle(resultPoints[0], resultColors[0], resultTextureCoords[0],
                                         resultPoints[1], resultColors[1], resultTextureCoords[1],
                                         resultPoints[2], resultColors[2], resultTextureCoords[2]));
            triangles.push_back(Triangle(resultPoints[0], resultColors[0], resultTextureCoords[0],
                                         resultPoints[2], resultColors[2], resultTextureCoords[2],
                                         resultPoints[3], resultColors[3], resultTextureCoords[3]));
        }
        else
            assert(pointCount < 3);
    }

    return Mesh(new Mesh_t(mesh->texture(), triangles));
}

namespace Generate
{
inline Mesh quadrilateral(TextureDescriptor texture, VectorF p1, Color c1, VectorF p2, Color c2, VectorF p3, Color c3, VectorF p4, Color c4)
{
    const TextureCoord t1 = TextureCoord(texture.minU, texture.minV);
    const TextureCoord t2 = TextureCoord(texture.maxU, texture.minV);
    const TextureCoord t3 = TextureCoord(texture.maxU, texture.maxV);
    const TextureCoord t4 = TextureCoord(texture.minU, texture.maxV);
    return Mesh(new Mesh_t(texture.image, vector<Triangle> {Triangle(p1, c1, t1, p2, c2, t2, p3, c3, t3), Triangle(p3, c3, t3, p4, c4, t4, p1, c1, t1)}));
}

/// make a box from <0, 0, 0> to <1, 1, 1>
inline Mesh unitBox(TextureDescriptor nx, TextureDescriptor px, TextureDescriptor ny, TextureDescriptor py, TextureDescriptor nz, TextureDescriptor pz)
{
    const VectorF p0 = VectorF(0, 0, 0);
    const VectorF p1 = VectorF(1, 0, 0);
    const VectorF p2 = VectorF(0, 1, 0);
    const VectorF p3 = VectorF(1, 1, 0);
    const VectorF p4 = VectorF(0, 0, 1);
    const VectorF p5 = VectorF(1, 0, 1);
    const VectorF p6 = VectorF(0, 1, 1);
    const VectorF p7 = VectorF(1, 1, 1);
    Mesh retval = Mesh(new Mesh_t());
    const Color c = Color(1);

    if(nx)
    {
        retval->add(quadrilateral(nx,
                                  p0, c,
                                  p4, c,
                                  p6, c,
                                  p2, c
                                 ));
    }

    if(px)
    {
        retval->add(quadrilateral(px,
                                  p5, c,
                                  p1, c,
                                  p3, c,
                                  p7, c
                                 ));
    }

    if(ny)
    {
        retval->add(quadrilateral(ny,
                                  p0, c,
                                  p1, c,
                                  p5, c,
                                  p4, c
                                 ));
    }

    if(py)
    {
        retval->add(quadrilateral(py,
                                  p6, c,
                                  p7, c,
                                  p3, c,
                                  p2, c
                                 ));
    }

    if(nz)
    {
        retval->add(quadrilateral(nz,
                                  p1, c,
                                  p0, c,
                                  p2, c,
                                  p3, c
                                 ));
    }

    if(pz)
    {
        retval->add(quadrilateral(pz,
                                  p4, c,
                                  p5, c,
                                  p7, c,
                                  p6, c
                                 ));
    }

    return retval;
}

Mesh line(const vector<VectorF> &linePoints, TextureDescriptor texture, Color color, float lineWidth);
Mesh lineLoop(vector<VectorF> linePoints, TextureDescriptor texture, Color color, float lineWidth);
}

#endif // GENERATE_H_INCLUDED
