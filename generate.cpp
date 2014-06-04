#include "generate.h"

namespace
{
struct Edge
{
    VectorF p1, p2;
    float distance;
    Edge(VectorF p1, VectorF p2, float distance)
        : p1(p1), p2(p2), distance(distance)
    {
    }
};

Edge makeStartEdge(VectorF startPoint, VectorF otherPoint, float distance, float lineWidth)
{
    VectorF tangent = normalize(otherPoint - startPoint);
    VectorF edgeDir = normalize(cross(tangent, VectorF(0, 0, 1)));
    return Edge(startPoint - edgeDir * lineWidth, startPoint + edgeDir * lineWidth, distance);
}

Edge makeEndEdge(VectorF otherPoint, VectorF endPoint, float distance, float lineWidth)
{
    VectorF tangent = normalize(endPoint - otherPoint);
    VectorF edgeDir = normalize(cross(tangent, VectorF(0, 0, 1)));
    return Edge(endPoint - edgeDir * lineWidth, endPoint + edgeDir * lineWidth, distance);
}

Edge makeMiddleEdge(VectorF p1, VectorF p2, VectorF p3, float distance, float lineWidth)
{
    VectorF delta1 = normalize(p2 - p1);
    VectorF delta2 = normalize(p2 - p3);
    VectorF edgeDir = delta1 + delta2;

    if(absSquared(edgeDir) < eps * eps)
    {
        edgeDir = normalize(cross(delta1, VectorF(0, 0, 1)));
    }
    else
    {
        edgeDir = normalize(edgeDir);

        if(dot(edgeDir, cross(delta1, VectorF(0, 0, 1))) < 0)
        {
            edgeDir = -edgeDir;
        }
        edgeDir *= min(max(sqrt(min(absSquared(p2 - p1), absSquared(p2 - p3))) / lineWidth, 3.0f), sqrt(2.0f) / sqrt(1 - dot(delta1, delta2)));
    }
    return Edge(p2 - edgeDir * lineWidth, p2 + edgeDir * lineWidth, distance);
}
}

Mesh Generate::line(const vector<VectorF> &linePointsIn, TextureDescriptor texture, Color color,
                    float lineWidth)
{
    if(linePointsIn.size() < 2) // if less than 2 points then we can't have a line
    {
        return Mesh(new Mesh_t());
    }
    vector<VectorF> linePoints;
    linePoints.reserve(linePointsIn.size());
    linePoints.push_back(linePointsIn[0]);
    for(size_t i = 1; i < linePointsIn.size(); i++)
    {
        if(absSquared(linePointsIn[i] - linePoints.back()) >= eps * eps) // remove duplicates
            linePoints.push_back(linePointsIn[i]);
    }
    if(linePoints.size() < 2) // if less than 2 points then we can't have a line
    {
        return Mesh(new Mesh_t());
    }

    vector<Edge> edges;
    edges.reserve(linePoints.size());
    float distance = 0;
    edges.push_back(makeStartEdge(linePoints[0], linePoints[1], distance, lineWidth));
    distance += abs(linePoints[1] - linePoints[0]);
    for(size_t i = 2; i < linePoints.size(); i++)
    {
        edges.push_back(makeMiddleEdge(linePoints[i - 2], linePoints[i - 1], linePoints[i], distance, lineWidth));
        distance += abs(linePoints[i - 1] - linePoints[i]);
    }
    edges.push_back(makeEndEdge(linePoints[linePoints.size() - 2], linePoints[linePoints.size() - 1], distance, lineWidth));
    Mesh retval = nullptr;
    for(size_t i = 1; i < edges.size(); i++)
    {
        TextureDescriptor currentTexture = texture.subTexture(edges[i - 1].distance / distance, edges[i].distance / distance, 0, 1);
        Mesh mesh = Generate::quadrilateral(currentTexture, edges[i - 1].p2, color, edges[i].p2, color, edges[i].p1, color, edges[i - 1].p1, color);
        if(retval == nullptr)
            retval = mesh;
        else
            retval->add(mesh);
    }
    return retval;
}

Mesh Generate::lineLoop(vector<VectorF> linePointsIn, TextureDescriptor texture, Color color, float lineWidth)
{
    if(linePointsIn.size() < 2) // if less than 2 points then we can't have a line
    {
        return Mesh(new Mesh_t());
    }
    vector<VectorF> linePoints;
    linePoints.reserve(linePointsIn.size());
    linePoints.push_back(linePointsIn[0]);
    for(size_t i = 1; i < linePointsIn.size(); i++)
    {
        if(absSquared(linePointsIn[i] - linePoints.back()) >= eps * eps) // remove duplicates
            linePoints.push_back(linePointsIn[i]);
    }
    if(linePoints.size() < 2) // if less than 2 points then we can't have a line
    {
        return Mesh(new Mesh_t());
    }
    if(linePoints.size() == 2)
        return Generate::line(linePoints, texture, color, lineWidth);

    vector<Edge> edges;
    edges.reserve(linePoints.size());
    float distance = 0;
    for(size_t i = 0; i < linePoints.size(); i++)
    {
        size_t j = (i + 1) % linePoints.size();
        size_t k = (i + 2) % linePoints.size();
        edges.push_back(makeMiddleEdge(linePoints[i], linePoints[j], linePoints[k], distance, lineWidth));
        distance += abs(linePoints[j] - linePoints[k]);
    }
    Mesh retval = nullptr;
    for(size_t i = 0; i < edges.size(); i++)
    {
        size_t j = (i + 1) % edges.size();
        TextureDescriptor currentTexture = texture.subTexture(edges[i].distance / distance, edges[j].distance / distance, 0, 1);
        Mesh mesh = Generate::quadrilateral(currentTexture, edges[i].p2, color, edges[j].p2, color, edges[j].p1, color, edges[i].p1, color);
        if(retval == nullptr)
            retval = mesh;
        else
            retval->add(mesh);
    }
    return retval;
}
